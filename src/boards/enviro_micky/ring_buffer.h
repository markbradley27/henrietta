#ifndef HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_RING_BUFFER_H_
#define HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_RING_BUFFER_H_

#include "Arduino.h"
#include <vector>

// A ring buffer for storing sensor values.
template <class T> class RingBuffer {
public:
  struct Record {
    T value;
    unsigned long at_millis;
  };

  RingBuffer(int size) { records_.resize(size); }

  // Inserts a new value, overwriting the oldest value if the ring buffer is
  // full.
  //
  // Inserted values should always be younger than all values already in
  // the buffer.
  void Insert(T value, unsigned long at_millis) {
    latest_ = (latest_ + 1) % records_.size();
    records_[latest_] = {value, at_millis};
  }

  // Returns the most recently inserted value.
  Record Latest() { return records_[latest_]; }

  // Returns the average of all values younger than maxAgeMs.
  //
  // If no values apply, returns 0.
  T Average(unsigned long max_age_ms) {
    T sum = 0;
    int count = 0;
    int i = latest_;
    unsigned long now_millis = millis();
    while (records_[i].at_millis != 0 &&
           now_millis - records_[i].at_millis < max_age_ms) {
      sum += records_[i].value;
      ++count;

      // Maybe I forgot how negative modulo works, but it wasn't doing what I
      // expected, so I just add aqis.size() to keep everything positive.
      i = (i - 1 + records_.size()) % records_.size();
      if (i == latest_) {
        break;
      }
    }

    if (count == 0) {
      return 0;
    }
    return sum / count;
  }

private:
  std::vector<Record> records_;
  int latest_ = 0;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_RING_BUFFER_H_
