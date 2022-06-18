#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_GRAPH_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_GRAPH_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"
#include "ring_buffer.h"

#define PLOT_MIN_X 0
#define PLOT_MAX_X 128
#define PLOT_MIN_Y 16
#define PLOT_MAX_Y 64
#define LABEL_WIDTH 19

class GraphDisplayer : public Displayer {
public:
  // TODO: Generalize for more than just uint16_t's.
  GraphDisplayer(Adafruit_SSD1306 *display, RingBuffer<uint16_t> *aqi_values,
                 const String title, const uint32_t time_range_ms,
                 const uint16_t deafult_min, const uint16_t default_max)
      : Displayer(display), aqi_values_(aqi_values), title_(title),
        time_range_ms_(time_range_ms), default_min_(deafult_min),
        default_max_(default_max) {}

  void Refresh() override {
    display_->clearDisplay();

    display_->setTextSize(1);
    display_->setCursor(JustifiedX(CENTER, title_, 128), 8);
    display_->print(title_);

    DrawPlot();

    display_->display();
  }

  void Update() override {
    display_->fillRect(PLOT_MIN_X, PLOT_MIN_Y, PLOT_MAX_X - PLOT_MIN_X,
                       PLOT_MAX_Y - PLOT_MIN_Y, SSD1306_BLACK);
    DrawPlot();
    display_->display();
  }

private:
  uint16_t CalcX(const uint32_t now_millis, const uint32_t at_millis) {
    return PLOT_MAX_X - (PLOT_MAX_X - PLOT_MIN_X - LABEL_WIDTH) *
                            (now_millis - at_millis) / time_range_ms_;
  }
  uint16_t CalcY(const uint16_t min, const uint16_t range,
                 const uint16_t value) {
    return PLOT_MAX_Y - 1 -
           (PLOT_MAX_Y - PLOT_MIN_Y - 1) * (value - min) / range;
  }

  void DrawPlot() {
    const uint32_t now_millis = millis();
    auto [min, max] = aqi_values_->MinMax(time_range_ms_);
    // std::min and std::max were giving me a type missmatch error I didn't
    // understand; this is good enough.
    if (min > default_min_) {
      min = 0;
    }
    if (max < default_max_) {
      max = 10;
    }
    const uint16_t range = max - min;

    display_->setTextSize(1);
    const String max_label = String(max);
    display_->setCursor(PLOT_MIN_X + JustifiedX(RIGHT, max_label, LABEL_WIDTH),
                        PLOT_MIN_Y);
    display_->print(max_label);
    const String min_label = String(min);
    display_->setCursor(PLOT_MIN_X + JustifiedX(RIGHT, min_label, LABEL_WIDTH),
                        PLOT_MAX_Y - 8);
    display_->print(min_label);
    display_->drawLine(PLOT_MIN_X + LABEL_WIDTH, PLOT_MIN_Y,
                       PLOT_MIN_X + LABEL_WIDTH, PLOT_MAX_Y, SSD1306_WHITE);

    auto riter = aqi_values_->rbegin();
    auto prev = *riter;
    for (++riter; riter != aqi_values_->rend() &&
                  now_millis - riter->at_millis < time_range_ms_;
         ++riter) {
      const int16_t prev_x = CalcX(now_millis, prev.at_millis);
      const int16_t prev_y = CalcY(min, range, prev.value);
      const int16_t cur_x = CalcX(now_millis, riter->at_millis);
      const int16_t cur_y = CalcY(min, range, riter->value);
      display_->writeLine(prev_x, prev_y, cur_x, cur_y, SSD1306_WHITE);
      prev = *riter;
    }
  }

  // Does not take ownership.
  RingBuffer<uint16_t> *aqi_values_;

  const String title_;
  const uint32_t time_range_ms_;
  const uint16_t default_min_;
  const uint16_t default_max_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_GRAPH_DISPLAYER_H_
