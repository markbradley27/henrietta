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

  class ReverseIterator {
  public:
    ReverseIterator(RingBuffer<T> *buf, int i, bool at_end)
        : buf_(buf), i_(i), at_end_(at_end) {
      if (buf_->records_[i_].at_millis == 0) {
        at_end_ = true;
      }
    }

    Record operator*() { return at_end_ ? Record{} : buf_->records_[i_]; }
    Record *operator->() { return at_end_ ? NULL : &buf_->records_[i_]; }

    void operator++() {
      if (at_end_) {
        return;
      }

      // Maybe I forgot how negative modulo works, but it wasn't doing what I
      // expected, so I just add records_.size() to keep everything positive.
      const int next_i =
          (i_ - 1 + buf_->records_.size()) % buf_->records_.size();
      if (buf_->records_[next_i].at_millis == 0 ||
          buf_->records_[next_i].at_millis > buf_->records_[i_].at_millis) {
        at_end_ = true;
        return;
      }
      i_ = next_i;
    }

    bool operator==(const ReverseIterator &b) {
      if (at_end_ || b.at_end_) {
        return at_end_ == b.at_end_;
      }
      return i_ == b.i_;
    }

    bool operator!=(const ReverseIterator &b) { return !(*this == b); }

  private:
    RingBuffer<T> *buf_;
    int i_;
    bool at_end_ = false;
  };
  friend ReverseIterator;

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

  // Returns the average of all values younger than max_age_ms.
  //
  // If no values apply, returns 0.
  T Average(unsigned long max_age_ms) {
    T sum = 0;
    int count = 0;
    const uint32_t now_millis = millis();
    for (auto riter = rbegin();
         riter != rend() && now_millis - riter->at_millis < max_age_ms;
         ++riter) {
      sum += riter->value;
      ++count;
    }

    if (count == 0) {
      return 0;
    }
    return sum / count;
  }

  // Returns the minimum and maximum of all values younger than max_age_ms.
  //
  // If no values apply, returns {0, 0}.
  std::pair<T, T> MinMax(unsigned long max_age_ms) {
    auto riter = rbegin();
    const uint32_t now_millis = millis();
    if (riter == rend() || now_millis - riter->at_millis > max_age_ms) {
      return {0, 0};
    }
    T min = riter->value;
    T max = riter->value;
    ++riter;
    while (riter != rend() && now_millis - riter->at_millis < max_age_ms) {
      min = std::min(min, riter->value);
      max = std::max(max, riter->value);
      ++riter;
    }
    return {min, max};
  }

  ReverseIterator rbegin() {
    return records_[latest_].at_millis == 0
               ? rend()
               : ReverseIterator(this, latest_, false);
  }
  ReverseIterator rend() { return ReverseIterator(this, 0, true); }

private:
  std::vector<Record> records_;
  int latest_ = 0;
};

#endif // HENRIETTA_SRC_BOARDS_ENVIRO_MICKY_RING_BUFFER_H_
