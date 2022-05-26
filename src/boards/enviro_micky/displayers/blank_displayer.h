#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_BLANK_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_BLANK_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

#include "displayer.h"

class BlankDisplayer : public Displayer {
public:
  BlankDisplayer(Adafruit_SSD1306 *display) : Displayer(display){};

  void Refresh() override {
    display_->clearDisplay();
    display_->display();
  }

  void Update() override{};
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_BLANK_DISPLAYER_H_
