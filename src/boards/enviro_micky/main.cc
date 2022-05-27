#include "Adafruit_PM25AQI.h"
#include "Arduino.h"
#include "DHT.h"
#include "config.h"
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <pb_encode.h>

#include "ath_big_numbers_displayer.h"
#include "ath_raw_displayer.h"
#include "blank_displayer.h"
#include "button.h"
#include "displayer.h"
#include "enviro_platformio_proto.h"
#include "platformio_consts.h"
#include "ring_buffer.h"
#include "util.h"

// Every X seconds, read sensor and update screen
#define UPDATE_INTERVAL_SECONDS 5
#define UPLOAD_INTERVAL_MINUTES 5
#define NUM_BUFFERED_VALUES 120 // 10min * 60s / UPDATE_INTERVAL_SECONDS

// OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// AQI sensor
SoftwareSerial aqi_serial(2, 3);
Adafruit_PM25AQI aqi_sensor = Adafruit_PM25AQI();
RingBuffer<uint16_t> aqi_values(NUM_BUFFERED_VALUES);

// DHT22 Temp/Humidity sensor
// Value that should be added to each temp reading.
#define TEMP_CALIBRATION_OFFSET -3.53
// Value that should be added to each humidity reading.
#define HUMIDITY_CALIBRATION_OFFSET 12.28
DHT dht(0, DHT22);
RingBuffer<float> temp_c_values(NUM_BUFFERED_VALUES);
RingBuffer<float> humidity_values(NUM_BUFFERED_VALUES);

// Buttons
Button right_button(D5);
IRAM_ATTR void right_button_isr() { right_button.Isr(); }
Button middle_button(D6);
IRAM_ATTR void middle_button_isr() { middle_button.Isr(); }
Button left_button(D7);
IRAM_ATTR void left_button_isr() { left_button.Isr(); }

// Timers
Timer timer_read_sensor = {seconds(UPDATE_INTERVAL_SECONDS)};
Timer timer_upload_data = {minutes(UPLOAD_INTERVAL_MINUTES)};

// Displayers
ATHBigNumbersDisplayer aqi_temp_humid_big_numbers_displayer(&display,
                                                            &aqi_values,
                                                            &temp_c_values,
                                                            &humidity_values);
ATHRawDisplayer aqi_temp_humid_displayer(&display, &aqi_values, &temp_c_values,
                                         &humidity_values);
BlankDisplayer blank_displayer(&display);
std::vector<Displayer *> displayers = {&aqi_temp_humid_big_numbers_displayer,
                                       &aqi_temp_humid_displayer,
                                       &blank_displayer};
int displayer_i = 0;

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

  // Dim display a bit.
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0x01);
}

void InitWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  const int timeout_millis = millis() + 7000;
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

  displayers[displayer_i]->Refresh();
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

  displayers[displayer_i]->Update();
}

void UploadData() {
  Serial.println("Uploading data...");
  WiFiClient wifi_client;
  MQTTClient mqtt_client;
  mqtt_client.begin(RPI_IP, MOSQUITTO_PORT, wifi_client);
  if (!mqtt_client.connect(CLIENT_ID_ENVIRO_MICKY)) {
    Serial.println("MQTT client connection failed.");
    return;
  }

  EnvironmentalData data = EnvironmentalData_init_zero;
  data.has_timestamp = true;
  data.timestamp = 27;
  data.has_aqi_pm25_standard_5_m_avg = true;
  data.aqi_pm25_standard_5_m_avg = aqi_values.Average(minutes(5));
  data.has_temp_c_5_m_avg = true;
  data.temp_c_5_m_avg = temp_c_values.Average(minutes(5));
  data.has_humidity_5_m_avg = true;
  data.humidity_5_m_avg = humidity_values.Average(minutes(5));

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
  const int displayer_change =
      right_button.UnhandledPresses() - left_button.UnhandledPresses();
  if (displayer_change != 0) {
    displayer_i = (displayer_i + displayer_change + 3 * displayers.size()) %
                  displayers.size();
    displayers[displayer_i]->Refresh();
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
