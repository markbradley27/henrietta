#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_RAW_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_RAW_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"
#include "ring_buffer.h"
#include "util.h"

#define TEXT_SIZE 1
#define COL_0_X 0
#define COL_1_X 40
#define COL_2_X 80

class ATHRawDisplayer : public Displayer {
public:
  ATHRawDisplayer(Adafruit_SSD1306 *display, RingBuffer<uint16_t> *aqi_values,
                  RingBuffer<float> *temp_c_values,
                  RingBuffer<float> *humidity_values)
      : Displayer(display), aqi_values_(aqi_values),
        temp_c_values_(temp_c_values), humidity_values_(humidity_values){};

  void Refresh() override {
    display_->clearDisplay();
    display_->setTextSize(TEXT_SIZE);
    display_->setTextColor(WHITE, BLACK);

    UpdateRow(0, "", "Now", "10m avg");
    UpdateTemp();
    UpdateHumidity();
    UpdateAqi();

    display_->display();
  }

  void Update() override {
    UpdateTemp();
    UpdateHumidity();
    UpdateAqi();
    display_->display();
  }

private:
  const int kRowYs[5] = {8, 21, 32, 43, 54};

  void UpdateRow(const int row, const char *col_0, const char *col_1,
                 const char *col_2) {
    display_->setCursor(COL_0_X, kRowYs[row]);
    display_->print("                     ");

    display_->setCursor(COL_0_X, kRowYs[row]);
    display_->print(col_0);
    display_->setCursor(COL_1_X, kRowYs[row]);
    display_->print(col_1);
    display_->setCursor(COL_2_X, kRowYs[row]);
    display_->print(col_2);
  }

  void UpdateTemp() {
    char temp_now_str[6];
    sprintf(temp_now_str, "%.1fF", CToF(temp_c_values_->Latest().value));
    char temp_avg_str[6];
    sprintf(temp_avg_str, "%.1fF", CToF(temp_c_values_->Average(minutes(10))));
    UpdateRow(1, "Temp:", temp_now_str, temp_avg_str);
  }

  void UpdateHumidity() {
    char humid_now_str[6];
    sprintf(humid_now_str, "%.1f%%", humidity_values_->Latest().value);
    char humid_avg_str[6];
    sprintf(humid_avg_str, "%.1f%%", humidity_values_->Average(minutes(10)));
    UpdateRow(2, "Humid:", humid_now_str, humid_avg_str);
  }

  void UpdateAqi() {
    UpdateRow(3, "AQI:", String(aqi_values_->Latest().value).c_str(),
              String(aqi_values_->Average(minutes(10))).c_str());
  }

  // Does not take ownership.
  RingBuffer<uint16_t> *aqi_values_;
  RingBuffer<float> *temp_c_values_;
  RingBuffer<float> *humidity_values_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_RAW_DISPLAYER_H_
