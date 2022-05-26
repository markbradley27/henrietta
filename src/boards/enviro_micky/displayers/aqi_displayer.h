#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_AQI_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_AQI_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"
#include "ring_buffer.h"
#include "util.h"

class AqiDisplayer : public Displayer {
public:
  AqiDisplayer(Adafruit_SSD1306 *display, RingBuffer<uint16_t> *aqi_values)
      : Displayer(display), aqi_values_(aqi_values){};

  void Refresh() override {
    display_->clearDisplay();

    // Latest reading
    display_->setTextSize(1);
    display_->setCursor(25, 8);
    display_->println("AQI");
    display_->drawRect(0, 16, 62, 33, SSD1306_WHITE);
    DisplayAqiValue();

    // Moving average
    display_->setTextSize(1);
    display_->setCursor(70, 8);
    display_->println("10m avg");
    display_->drawLine(70, 16, 110, 16, SSD1306_WHITE);
    DisplayMovingAverage();

    display_->display();
  }

  void Update() override {
    DisplayAqiValue();
    DisplayMovingAverage();
    display_->display();
  }

private:
  // TODO: Handle numbers with more than one digit.
  void DisplayAqiValue() {
    display_->setTextSize(3);
    display_->setTextColor(WHITE, BLACK);
    display_->setCursor(25, 21);
    display_->println(" ");
    display_->setTextSize(3);
    display_->setCursor(25, 21);
    display_->println(aqi_values_->Latest().value);
  }

  // TODO: Maybe don't display in the first 10 minutes?
  // TODO: Handle numbers with more than one digit.
  void DisplayMovingAverage() {
    display_->setTextSize(2);
    display_->setTextColor(WHITE, BLACK);
    display_->setCursor(70, 24);
    display_->println(" ");
    display_->setTextSize(2);
    display_->setCursor(70, 24);
    display_->println(aqi_values_->Average(minutes(10)));
  }

  // Does not take ownership.
  RingBuffer<uint16_t> *aqi_values_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_AQI_DISPLAYER_H_
