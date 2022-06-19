#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_GRAPH_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_GRAPH_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"
#include "ring_buffer.h"

#define PLOT_MIN_X 0
#define PLOT_MAX_X 128
#define PLOT_MIN_Y 16
#define PLOT_MAX_Y 64
#define X_AXIS_WIDTH 19
#define Y_AXIS_HEIGHT 3

template <class T> class GraphDisplayer : public Displayer {
public:
  // TODO: Use an options struct.
  GraphDisplayer(
      Adafruit_SSD1306 *display, RingBuffer<T> *values, const String title,
      const uint32_t time_range_ms, const uint32_t time_tick_interval_ms,
      const T deafult_min, const T default_max,
      std::function<String(T)> label_formatter =
          [](T val) { return String(val); })
      : Displayer(display), values_(values), title_(title),
        time_range_ms_(time_range_ms),
        time_tick_interval_ms_(time_tick_interval_ms),
        default_min_(deafult_min), default_max_(default_max),
        label_formatter_(label_formatter) {}

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
    return PLOT_MAX_X - (PLOT_MAX_X - PLOT_MIN_X - X_AXIS_WIDTH) *
                            (now_millis - at_millis) / time_range_ms_;
  }
  uint16_t CalcY(const T min, const T range, const T value) {
    return PLOT_MAX_Y - Y_AXIS_HEIGHT - 1 -
           ((PLOT_MAX_Y - Y_AXIS_HEIGHT - 1) - PLOT_MIN_Y) * (value - min) /
               range;
  }

  void DrawPlot() {
    const uint32_t now_millis = millis();
    const auto &[min, max] = values_->MinMax(time_range_ms_);
    const T plot_min = std::min(default_min_, min);
    const T plot_max = std::max(default_max_, max);
    const T plot_range = plot_max - plot_min;

    // Y-axis labels.
    const String max_label = label_formatter_(max);
    int max_x = PLOT_MIN_X + JustifiedX(RIGHT, max_label, X_AXIS_WIDTH);
    int max_y = CalcY(plot_min, plot_range, max) - 4;
    const String min_label = label_formatter_(min);
    int min_x = PLOT_MIN_X + JustifiedX(RIGHT, min_label, X_AXIS_WIDTH);
    int min_y = CalcY(plot_min, plot_range, min) - 4;
    if (min_y - max_y < 8) {
      const int nudge_apart = (9 - (min_y - max_y)) / 2;
      max_y -= nudge_apart;
      min_y += nudge_apart;
    }
    if (max_y < PLOT_MIN_Y) {
      const int nudge_down = PLOT_MIN_Y - max_y;
      max_y += nudge_down;
      min_y += nudge_down;
    }
    if (min_y > PLOT_MAX_Y - 8) {
      const int nudge_up = min_y - (PLOT_MAX_Y - 8);
      max_y -= nudge_up;
      min_y -= nudge_up;
    }
    display_->setTextSize(1);
    display_->setCursor(max_x, max_y);
    display_->print(max_label);
    display_->setCursor(min_x, min_y);
    display_->print(min_label);
    display_->drawLine(PLOT_MIN_X + X_AXIS_WIDTH, PLOT_MIN_Y,
                       PLOT_MIN_X + X_AXIS_WIDTH, PLOT_MAX_Y - Y_AXIS_HEIGHT,
                       SSD1306_WHITE);

    // X-axis tick marks.
    for (uint32_t tick_millis = time_range_ms_; tick_millis > 0;
         tick_millis -= time_tick_interval_ms_) {
      // Abusing the CalcX api a little bit here, but it works.
      const int16_t tick_x = CalcX(time_range_ms_, tick_millis);
      display_->drawLine(tick_x, PLOT_MAX_Y - Y_AXIS_HEIGHT, tick_x, PLOT_MAX_Y,
                         SSD1306_WHITE);
    }
    display_->drawLine(PLOT_MIN_X + X_AXIS_WIDTH, PLOT_MAX_Y - Y_AXIS_HEIGHT,
                       PLOT_MAX_X, PLOT_MAX_Y - Y_AXIS_HEIGHT, SSD1306_WHITE);

    // Plot.
    auto riter = values_->rbegin();
    auto prev = *riter;
    for (++riter; riter != values_->rend() &&
                  now_millis - riter->at_millis < time_range_ms_;
         ++riter) {
      const int16_t prev_x = CalcX(now_millis, prev.at_millis);
      const int16_t prev_y = CalcY(plot_min, plot_range, prev.value);
      const int16_t cur_x = CalcX(now_millis, riter->at_millis);
      const int16_t cur_y = CalcY(plot_min, plot_range, riter->value);
      display_->writeLine(prev_x, prev_y, cur_x, cur_y, SSD1306_WHITE);
      prev = *riter;
    }
  }

  // Does not take ownership.
  RingBuffer<T> *values_;

  const String title_;
  const uint32_t time_range_ms_;
  const uint32_t time_tick_interval_ms_;
  const T default_min_;
  const T default_max_;
  std::function<String(T)> label_formatter_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_GRAPH_DISPLAYER_H_
