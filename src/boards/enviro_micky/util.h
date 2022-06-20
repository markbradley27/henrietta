#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_UTIL_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_UTIL_H_

struct Timer {
  unsigned long total_cycle_time;
  unsigned long last_cycle_time = 0;
  void Reset() { last_cycle_time = millis(); };
  bool Complete() { return (millis() - last_cycle_time) > total_cycle_time; };
};

// Time helpers
unsigned long seconds(int s) { return s * 1000; }
unsigned long minutes(int m) { return m * 1000 * 60; }
unsigned long hours(int h) { return h * 1000 * 60 * 60; }
int msToSeconds(unsigned long ms) { return ms / 1000; }
int msToMinutes(unsigned long ms) { return ms / 1000 / 60; }
int msToHours(unsigned long ms) { return ms / 1000 / 60 / 60; }
int msToDays(unsigned long ms) { return ms / 1000 / 60 / 60 / 24; }

// Temp helpers
float CToF(float celsius) { return celsius * 1.8 + 32; }
float FToC(float fahrenheit) { return (fahrenheit - 32) / 1.8; }

// Screen text justification
#define BASE_TEXT_WIDTH 6
enum JustifyXType {
  CENTER,
  RIGHT,
};
// Returns the x offset needed to justify text within a bounding box.
// Args:
// * type - type of justification to calculate
// * text_length - number of characters
// * box_width - width of bounding box in pixels
// * text_width - text width scaling factor (the value provided to setTextSize)
int16_t JustifiedX(const JustifyXType type, const size_t text_length,
                   const int16_t box_width, const uint8_t text_width = 1) {
  return (box_width - BASE_TEXT_WIDTH * text_width * text_length + text_width) /
         (type == CENTER ? 2 : 1);
}
// Convenience method that uses the length of `text`.
int16_t JustifiedX(const JustifyXType type, const String &text,
                   const int16_t box_width, const uint8_t text_width = 1) {
  return JustifiedX(type, text.length(), box_width, text_width);
}

#define BASE_TEXT_HEIGHT 8
enum JustifyYType {
  MIDDLE,
  BOTTOM,
};
// Returns the y offset needed to justify text within a bounding box.
// Args:
// * type - type of justification to calculate
// * box_height - height of bounding box in pixels
// * text_height - text height scaling factor (the value provided to
//   setTextSize)
int16_t JustifiedY(const JustifyYType type, const int16_t box_height,
                   const uint8_t text_height = 1) {
  return (box_height - BASE_TEXT_HEIGHT * text_height + text_height) /
         (type == MIDDLE ? 2 : 1);
}

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_UTIL_H_
