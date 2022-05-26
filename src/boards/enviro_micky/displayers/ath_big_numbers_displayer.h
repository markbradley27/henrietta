#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_BIG_NUMBERS_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_BIG_NUMBERS_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"
#include "ring_buffer.h"
#include "util.h"

#define MOVING_AVERAGE_MINUTES 2

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
    display_->setCursor(2, 8);
    display_->print("                    ");
    display_->setCursor(4, 8);
    display_->print("Temp F");
    display_->setCursor(46, 8);
    display_->print("Humid %");
    display_->setCursor(100, 8);
    display_->print("AQI");
    display_->drawLine(2, 16, 126, 16, SSD1306_WHITE);
    display_->drawLine(2, 52, 126, 52, SSD1306_WHITE);
    display_->drawLine(42, 24, 42, 44, SSD1306_WHITE);
    display_->drawLine(88, 24, 88, 44, SSD1306_WHITE);

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
    display_->setTextSize(2, 3);
    display_->setCursor(3, 24);
    display_->print("   ");

    // Big average
    const float temp_avg_f =
        CToF(temp_c_values_->Average(minutes(MOVING_AVERAGE_MINUTES)));
    display_->setCursor(3, 24);
    display_->print(int(temp_avg_f));
    display_->setTextSize(2);
    display_->print(int(temp_avg_f * 10) % 10);

    // Small latest
    display_->setTextSize(1);
    display_->setCursor(10, 56);
    display_->print("    ");
    display_->setCursor(10, 56);
    char temp_str[6];
    sprintf(temp_str, "%.1f", CToF(temp_c_values_->Latest().value));
    display_->print(temp_str);
  }

  void UpdateHumidity() {
    // Clear
    display_->setTextSize(2, 3);
    display_->setCursor(49, 24);
    display_->print("   ");

    // Big average
    const float humidity_avg =
        humidity_values_->Average(minutes(MOVING_AVERAGE_MINUTES));
    display_->setCursor(49, 24);
    display_->print(int(humidity_avg));
    display_->setTextSize(2);
    display_->print(int(humidity_avg * 10) % 10);

    // Small latest
    display_->setTextSize(1);
    display_->setCursor(56, 56);
    display_->print("    ");
    display_->setCursor(56, 56);
    char humidity_str[5];
    sprintf(humidity_str, "%.1f", humidity_values_->Latest().value);
    display_->print(humidity_str);
  }

  void UpdateAqi() {
    // Clear
    display_->setTextSize(2, 3);
    display_->setCursor(91, 24);
    display_->print("   ");

    // Big average
    const int aqi_avg = aqi_values_->Average(minutes(MOVING_AVERAGE_MINUTES));
    int digits = int(log10(aqi_avg));
    int x = 103 - 6 * digits;
    display_->setCursor(x, 24);
    display_->print(aqi_avg);

    // Small latest
    const int aqi_latest = aqi_values_->Latest().value;
    display_->setTextSize(1);
    display_->setCursor(100, 56);
    display_->print("    ");
    digits = int(log10(aqi_latest));
    x = 106 - 3 * digits;
    display_->setCursor(x, 56);
    display_->print(aqi_latest);
  }

  // Does not take ownership.
  RingBuffer<uint16_t> *aqi_values_;
  RingBuffer<float> *temp_c_values_;
  RingBuffer<float> *humidity_values_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_ATH_BIG_NUMBERS_DISPLAYER_H_
