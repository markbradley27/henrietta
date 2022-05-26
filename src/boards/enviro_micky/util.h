#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_UTIL_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_UTIL_H_

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

struct Timer {
  unsigned long total_cycle_time;
  unsigned long last_cycle_time = 0;
  void Reset() { last_cycle_time = millis(); };
  bool Complete() { return (millis() - last_cycle_time) > total_cycle_time; };
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_UTIL_H_
