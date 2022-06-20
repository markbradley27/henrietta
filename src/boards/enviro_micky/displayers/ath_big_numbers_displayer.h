#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_BIG_NUMBERS_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_BIG_NUMBERS_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"
#include "ring_buffer.h"
#include "util.h"

#define MOVING_AVERAGE_MINUTES 2

#define COL_WIDTH 42    // (128 - 2(pixel dividers)) / 3
#define COL_0_MIN_X 0   // 0 * (COL_WIDTH + 1)
#define COL_1_MIN_X 43  // 1 * (COL_WIDTH + 1)
#define COL_2_MIN_X 86  // 2 * (COL_WIDTH + 1)
#define BIG_MIN_Y 17    // Big numbers upper bound.
#define SMALL_MIN_Y 53  // Small numbers upper bound.
#define TITLE_HEIGHT 16 // BIG_MIN_Y - 1(pixel divider)
#define BIG_HEIGHT 35   // SMALL_MIN_Y - BIG_MIN_Y - 1(pixel divider)
#define SMALL_HEIGHT 11 // 64 - SMALL_MIN_Y
// Gap between ends of vertical divider lines and horizontal divider lines (for
// aesthetics).
#define COL_DIVIDER_VERT_GAP 6

class ATHBigNumbersDisplayer : public Displayer {
public:
  ATHBigNumbersDisplayer(Adafruit_SSD1306 *display,
                         RingBuffer<uint16_t> *aqi_values,
                         RingBuffer<float> *temp_c_values,
                         RingBuffer<float> *humidity_values)
      : Displayer(display), aqi_values_(aqi_values),
        temp_c_values_(temp_c_values), humidity_values_(humidity_values){};

  void Refresh() override {
    display_->clearDisplay();
    display_->setTextColor(WHITE, BLACK);

    display_->setTextSize(1);
    const String temp_label = "Temp F";
    display_->setCursor(COL_0_MIN_X + JustifiedX(CENTER, temp_label, COL_WIDTH),
                        JustifiedY(BOTTOM, TITLE_HEIGHT));
    display_->print(temp_label);
    const String humid_label = "Humid %";
    display_->setCursor(COL_1_MIN_X +
                            JustifiedX(CENTER, humid_label, COL_WIDTH),
                        JustifiedY(BOTTOM, TITLE_HEIGHT));
    display_->print(humid_label);
    const String aqi_label = "AQI";
    display_->setCursor(COL_2_MIN_X + JustifiedX(CENTER, aqi_label, COL_WIDTH),
                        JustifiedY(BOTTOM, TITLE_HEIGHT));
    display_->print(aqi_label);

    // Top horizontal divider.
    display_->drawLine(0, BIG_MIN_Y - 1, 127, BIG_MIN_Y - 1, SSD1306_WHITE);
    // Bottom horizontal divider.
    display_->drawLine(0, SMALL_MIN_Y - 1, 127, SMALL_MIN_Y - 1, SSD1306_WHITE);
    // Left vertical divider.
    display_->drawLine(COL_1_MIN_X - 1, BIG_MIN_Y + COL_DIVIDER_VERT_GAP,
                       COL_1_MIN_X - 1, SMALL_MIN_Y - 1 - COL_DIVIDER_VERT_GAP,
                       SSD1306_WHITE);
    // Right vertical divider.
    display_->drawLine(COL_2_MIN_X - 1, BIG_MIN_Y + COL_DIVIDER_VERT_GAP,
                       COL_2_MIN_X - 1, SMALL_MIN_Y - 1 - COL_DIVIDER_VERT_GAP,
                       SSD1306_WHITE);

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
  void UpdateTemp() {
    // Clear
    display_->fillRect(COL_0_MIN_X, BIG_MIN_Y, COL_WIDTH, BIG_HEIGHT,
                       SSD1306_BLACK);
    display_->fillRect(COL_0_MIN_X, SMALL_MIN_Y, COL_WIDTH, SMALL_HEIGHT,
                       SSD1306_BLACK);

    // Big average
    const float temp_avg_f =
        CToF(temp_c_values_->Average(minutes(MOVING_AVERAGE_MINUTES)));
    const int temp_avg_f_int = int(temp_avg_f);
    const int text_width = temp_avg_f_int >= 100 ? 1 : 2;
    const int text_height = 3;
    // # of whole number digits plus one tenths digit.
    const int digits = int(log10(temp_avg_f_int)) + 2;
    display_->setTextSize(text_width, text_height);
    display_->setCursor(
        COL_0_MIN_X + JustifiedX(CENTER, digits, COL_WIDTH, text_width),
        BIG_MIN_Y + JustifiedY(MIDDLE, BIG_HEIGHT, text_height));
    display_->print(temp_avg_f_int);
    display_->setTextSize(text_width, text_height - 1);
    display_->print(int(temp_avg_f * 10) % 10);

    // Small latest
    display_->setTextSize(1);
    const String temp_latest_str =
        String(CToF(temp_c_values_->Latest().value), 1);
    display_->setCursor(COL_0_MIN_X +
                            JustifiedX(CENTER, temp_latest_str, COL_WIDTH),
                        SMALL_MIN_Y + JustifiedY(MIDDLE, SMALL_HEIGHT));
    display_->print(temp_latest_str);
  }

  void UpdateHumidity() {
    // Clear
    display_->fillRect(COL_1_MIN_X, BIG_MIN_Y, COL_WIDTH, BIG_HEIGHT,
                       SSD1306_BLACK);
    display_->fillRect(COL_1_MIN_X, SMALL_MIN_Y, COL_WIDTH, SMALL_HEIGHT,
                       SSD1306_BLACK);

    // Big average
    const float humidity_avg =
        humidity_values_->Average(minutes(MOVING_AVERAGE_MINUTES));
    const int humidity_avg_int = int(humidity_avg);
    const int text_width = humidity_avg_int >= 100 ? 1 : 2;
    const int text_height = 3;
    // # of whole number digits plus one tenths digit.
    const int digits = int(log10(humidity_avg_int)) + 2;
    display_->setTextSize(text_width, text_height);
    display_->setCursor(
        COL_1_MIN_X + JustifiedX(CENTER, digits, COL_WIDTH, text_width),
        BIG_MIN_Y + JustifiedY(MIDDLE, BIG_HEIGHT, text_height));
    display_->print(humidity_avg_int);
    display_->setTextSize(text_width, text_height - 1);
    display_->print(int(humidity_avg * 10) % 10);

    // Small latest
    display_->setTextSize(1);
    const String humidity_latest_str =
        String(humidity_values_->Latest().value, 1);
    display_->setCursor(COL_1_MIN_X +
                            JustifiedX(CENTER, humidity_latest_str, COL_WIDTH),
                        SMALL_MIN_Y + JustifiedY(MIDDLE, SMALL_HEIGHT));
    display_->print(humidity_latest_str);
  }

  void UpdateAqi() {
    // Clear
    display_->fillRect(COL_2_MIN_X, BIG_MIN_Y, COL_WIDTH, BIG_HEIGHT,
                       SSD1306_BLACK);
    display_->fillRect(COL_2_MIN_X, SMALL_MIN_Y, COL_WIDTH, SMALL_HEIGHT,
                       SSD1306_BLACK);

    // Big average
    const int aqi_avg = aqi_values_->Average(minutes(MOVING_AVERAGE_MINUTES));
    const String aqi_avg_str(aqi_avg);
    const int text_width = aqi_avg_str.length() > 3 ? 1 : 2;
    const int text_height = 3;
    display_->setTextSize(text_width, text_height);
    display_->setCursor(
        COL_2_MIN_X + JustifiedX(CENTER, aqi_avg_str, COL_WIDTH, text_width),
        BIG_MIN_Y + JustifiedY(MIDDLE, BIG_HEIGHT, text_height));
    display_->print(aqi_avg_str);

    // Small latest
    display_->setTextSize(1);
    const String aqi_latest_str = String(aqi_values_->Latest().value);
    display_->setCursor(COL_2_MIN_X +
                            JustifiedX(CENTER, aqi_latest_str, COL_WIDTH),
                        SMALL_MIN_Y + JustifiedY(MIDDLE, SMALL_HEIGHT));
    display_->print(aqi_latest_str);
  }

  // Does not take ownership.
  RingBuffer<uint16_t> *aqi_values_;
  RingBuffer<float> *temp_c_values_;
  RingBuffer<float> *humidity_values_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_BIG_NUMBERS_DISPLAYER_H_
