#include "Adafruit_PM25AQI.h"
#include "Arduino.h"
#include "DHT.h"
#include "config.h"
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <pb_encode.h>

#include "ath_big_numbers_displayer.h"
#include "ath_raw_displayer.h"
#include "button.h"
#include "displayer.h"
#include "enviro_platformio_proto.h"
#include "graph_displayer.h"
#include "platformio_consts.h"
#include "ring_buffer.h"
#include "util.h"

#define UPDATE_INTERVAL_SECONDS 5
#define UPLOAD_INTERVAL_MINUTES 5

// AQI sensor
SoftwareSerial aqi_serial(2, 3);
Adafruit_PM25AQI aqi_sensor = Adafruit_PM25AQI();
RingBuffer<uint16_t>
    aqi_values(10 * 60 / UPDATE_INTERVAL_SECONDS); // 10m of sensor readings.
RingBuffer<uint16_t> aqi_5m_avgs(24 * 60 /
                                 UPLOAD_INTERVAL_MINUTES); // 24h of 5m avgs.

// DHT22 Temp/Humidity sensor
// Value that should be added to each temp reading.
#define TEMP_CALIBRATION_OFFSET -3.53
// Value that should be added to each humidity reading.
#define HUMIDITY_CALIBRATION_OFFSET 12.28
DHT dht(0, DHT22);
RingBuffer<float> temp_c_values(5 * 60 / UPDATE_INTERVAL_SECONDS);
RingBuffer<float> temp_c_5m_avgs(24 * 60 / UPLOAD_INTERVAL_MINUTES);
RingBuffer<float> humidity_values(5 * 60 / UPDATE_INTERVAL_SECONDS);
RingBuffer<float> humidity_5m_avgs(24 * 60 / UPLOAD_INTERVAL_MINUTES);

// Buttons
Button right_button(D5);
IRAM_ATTR void right_button_isr() { right_button.Isr(); }
Button middle_button(D6);
IRAM_ATTR void middle_button_isr() { middle_button.Isr(); }
Button left_button(D7);
IRAM_ATTR void left_button_isr() { left_button.Isr(); }

// OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Displayers
#define DISPLAY_TIMEOUT_SECONDS 60
ATHBigNumbersDisplayer ath_big_numbers_displayer(&display, &aqi_values,
                                                 &temp_c_values,
                                                 &humidity_values);
ATHRawDisplayer ath_raw_displayer(&display, &aqi_values, &temp_c_values,
                                  &humidity_values);
std::function<String(float)> temp_formatter = [](float c) {
  return String(CToF(c), 0);
};
GraphDisplayer<float> temp_24h_graph_displayer(&display, &temp_c_5m_avgs,
                                               "Temp - 24h", hours(24), 0, 27,
                                               temp_formatter);
GraphDisplayer<float> temp_8h_graph_displayer(&display, &temp_c_5m_avgs,
                                              "Temp - 8h", hours(8), 0, 27,
                                              temp_formatter);
GraphDisplayer<float> temp_1h_graph_displayer(&display, &temp_c_5m_avgs,
                                              "Temp - 1h", hours(1), 0, 27,
                                              temp_formatter);
GraphDisplayer<float> temp_10m_graph_displayer(&display, &temp_c_values,
                                               "Temp - 10m", minutes(10), 0, 27,
                                               temp_formatter);
std::function<String(float)> humid_formatter = [](float h) {
  return String(h, 0);
};
GraphDisplayer<float> humid_24h_graph_displayer(&display, &humidity_5m_avgs,
                                                "Humidity - 24h", hours(24), 0,
                                                100, humid_formatter);
GraphDisplayer<float> humid_8h_graph_displayer(&display, &humidity_5m_avgs,
                                               "Humidity - 8h", hours(8), 0,
                                               100, humid_formatter);
GraphDisplayer<float> humid_1h_graph_displayer(&display, &humidity_5m_avgs,
                                               "Humidity - 1h", hours(1), 0,
                                               100, humid_formatter);
GraphDisplayer<float> humid_10m_graph_displayer(&display, &humidity_values,
                                                "Humidity - 10m", minutes(10),
                                                0, 100, humid_formatter);
GraphDisplayer<uint16_t> aqi_24h_graph_displayer(&display, &aqi_5m_avgs,
                                                 "AQI - 24h", hours(24), 0, 50);
GraphDisplayer<uint16_t> aqi_8h_graph_displayer(&display, &aqi_5m_avgs,
                                                "AQI - 8h", hours(8), 0, 50);
GraphDisplayer<uint16_t> aqi_1h_graph_displayer(&display, &aqi_5m_avgs,
                                                "AQI - 1h", hours(1), 0, 50);
GraphDisplayer<uint16_t> aqi_10m_graph_displayer(&display, &aqi_values,
                                                 "AQI - 10m", minutes(10), 0,
                                                 50);
std::vector<std::vector<Displayer *>> displayers = {
    {&ath_big_numbers_displayer, &ath_raw_displayer},
    {&temp_24h_graph_displayer, &temp_8h_graph_displayer,
     &temp_1h_graph_displayer, &temp_10m_graph_displayer},
    {&humid_24h_graph_displayer, &humid_8h_graph_displayer,
     &humid_1h_graph_displayer, &humid_10m_graph_displayer},
    {&aqi_24h_graph_displayer, &aqi_8h_graph_displayer, &aqi_1h_graph_displayer,
     &aqi_10m_graph_displayer}};
int displayer_i = 0;
int displayer_j = 0;
bool displaying = true;

// Timers
Timer timer_display_timeout = {seconds(DISPLAY_TIMEOUT_SECONDS)};
Timer timer_read_sensor = {seconds(UPDATE_INTERVAL_SECONDS)};
Timer timer_upload_data = {minutes(UPLOAD_INTERVAL_MINUTES)};

// Time
WiFiUDP ntp_udp;
NTPClient time_client(ntp_udp, RPI_IP, 0, minutes(60));

void InitDisplay() {

  if (!display.begin(
          SSD1306_SWITCHCAPVCC, // Generate display voltage from 3.3V internally
          SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
}

void InitWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  const uint32_t timeout_millis = millis() + 7000;
  Serial.print("Connecting to Wifi(" + String(WIFI_SSID) + ")");
  while (WiFi.status() != WL_CONNECTED && millis() < timeout_millis) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("connected!");
  } else {
    Serial.println(
        "this is taking too long, will keep trying in the background");
  }
}

void InitAqiSensor() {
  // Wait one second for sensor to boot up.
  delay(1000);

  // Connect to the sensor over software serial.
  if (!aqi_sensor.begin_UART(&aqi_serial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("PM25 found!");
}

void setup() {
  Serial.begin(115200);
  aqi_serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(D5), right_button_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(D6), middle_button_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(D7), left_button_isr, CHANGE);

  InitDisplay();
  InitWifi();
  InitAqiSensor();
  dht.begin();
  time_client.begin();

  displayers[displayer_i][displayer_j]->Refresh();
  timer_display_timeout.Reset();
}

void HandleDisplayChange(const int left_presses, const int middle_presses,
                         const int right_presses) {
  if (!displaying) {
    displaying = true;
    displayers[displayer_i][displayer_j]->Refresh();
    return;
  }

  const int i_change = right_presses - left_presses;
  if (i_change != 0) {
    // Add some large value to avoid negative modulo.
    displayer_i =
        (displayer_i + i_change + 3 * displayers.size()) % displayers.size();
    displayer_j = 0;
  }

  if (middle_presses != 0) {
    displayer_j =
        (displayer_j + middle_presses) % displayers[displayer_i].size();
  }

  displayers[displayer_i][displayer_j]->Refresh();
}

bool ReadAqiSensor(PM25_AQI_Data *data) {
  // Clear the serial buffer.
  while (aqi_serial.available()) {
    aqi_serial.read();
  }

  // Wait for a reading.
  unsigned long start_ms = millis();
  while (millis() - start_ms < seconds(2)) {
    if (aqi_sensor.read(data)) {
      return true;
    }
  }
  return false;
}

void ReadAllSensors() {
  Serial.println("Reading sensors...");
  PM25_AQI_Data data;
  if (ReadAqiSensor(&data)) {
    aqi_values.Insert(data.pm25_standard, millis());
    Serial.println("AQI: " + String(aqi_values.Latest().value));
  }

  float temp_c = dht.readTemperature() + TEMP_CALIBRATION_OFFSET;
  if (!isnan(temp_c)) {
    temp_c_values.Insert(temp_c, millis());
    Serial.println("Temp: " + String(temp_c_values.Latest().value) + "°C, " +
                   String(CToF(temp_c_values.Latest().value)) + "°F");
  }

  float humidity = dht.readHumidity() + HUMIDITY_CALIBRATION_OFFSET;
  if (!isnan(humidity)) {
    humidity_values.Insert(humidity, millis());
    Serial.println("Humidity: " + String(humidity_values.Latest().value) + "%");
  }

  if (displaying) {
    displayers[displayer_i][displayer_j]->Update();
  }
}

void UploadData() {
  Serial.println("Uploading data...");

  const uint32_t now_millis = millis();
  const uint16_t aqi_5m_avg = aqi_values.Average(minutes(5));
  aqi_5m_avgs.Insert(aqi_5m_avg, now_millis);
  const float temp_c_5m_avg = temp_c_values.Average(minutes(5));
  temp_c_5m_avgs.Insert(temp_c_5m_avg, now_millis);
  const float humidity_5m_avg = humidity_values.Average(minutes(5));
  humidity_5m_avgs.Insert(humidity_5m_avg, now_millis);

  WiFiClient wifi_client;
  MQTTClient mqtt_client;
  mqtt_client.begin(RPI_IP, MOSQUITTO_PORT, wifi_client);
  if (!mqtt_client.connect(CLIENT_ID_ENVIRO_MICKY)) {
    Serial.println("MQTT client connection failed.");
    return;
  }

  EnvironmentalData data = EnvironmentalData_init_zero;
  data.has_timestamp = true;
  data.timestamp = time_client.getEpochTime();
  data.has_aqi_pm25_standard_5m_avg = true;
  data.aqi_pm25_standard_5m_avg = aqi_5m_avg;
  data.has_temp_c_5m_avg = true;
  data.temp_c_5m_avg = temp_c_5m_avg;
  data.has_humidity_5m_avg = true;
  data.humidity_5m_avg = humidity_5m_avg;

  // TODO: Can I get a more exact estimation of the required buffer size?
  uint8_t buffer[128];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  const bool status = pb_encode(&stream, &EnvironmentalData_msg, &data);
  if (!status) {
    Serial.println("Encoding EnvironmentalData message failed.");
    return;
  }
  const uint8_t message_length = stream.bytes_written;

  Serial.print("EnvironmentalData encoded: ");
  char print_buffer[8];
  for (uint8_t i = 0; i < message_length; ++i) {
    sprintf(print_buffer, "%x ", buffer[i]);
    Serial.print(print_buffer);
  }
  Serial.println("");

  mqtt_client.publish(TOPIC_METRICS_ENVIRO,
                      reinterpret_cast<const char *>(buffer), message_length,
                      /*retained=*/false,
                      /*qos=*/2);
}

void loop() {
  // Time client has its own update interval logic, so this is a no-op most of
  // the time.
  time_client.update();

  const int left_presses = left_button.UnhandledPresses();
  const int middle_presses = middle_button.UnhandledPresses();
  const int right_presses = right_button.UnhandledPresses();

  if (left_presses != 0 || middle_presses != 0 || right_presses != 0) {
    timer_display_timeout.Reset();
    HandleDisplayChange(left_presses, middle_presses, right_presses);
  }

  if (timer_display_timeout.Complete() && displaying) {
    display.clearDisplay();
    display.display();
    displaying = false;
  }

  if (timer_read_sensor.Complete()) {
    timer_read_sensor.Reset();
    ReadAllSensors();
  }

  if (timer_upload_data.Complete()) {
    timer_upload_data.Reset();
    UploadData();
  }
}
