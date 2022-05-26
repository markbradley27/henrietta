#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_DISPLAYER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_DISPLAYER_H_

#include <Adafruit_SSD1306.h>

class Displayer {
public:
  Displayer(Adafruit_SSD1306 *display) : display_(display){};

  // Clears and refreshes the entire display.
  virtual void Refresh() = 0;

  // Updates display to reflect new readings.
  virtual void Update() = 0;

protected:
  // Does not take ownership.
  Adafruit_SSD1306 *display_;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_DISPLAYERS_DISPLAYER_H_
