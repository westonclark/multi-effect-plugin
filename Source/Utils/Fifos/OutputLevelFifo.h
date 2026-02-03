#pragma once

#include <JuceHeader.h>

template <typename T> class OutputLevelFifo {
public:
  // Push a new value. Returns false only if FIFO is full (8 pending updates).
  bool push(const T &value) {
    auto write = fifo.write(1);
    if (write.blockSize1 > 0) {
      slots[static_cast<size_t>(write.startIndex1)] = value;
      return true;
    }
    return false;
  }

  // Pull the next value. Returns true if a value was available.
  bool pull(T &value) {
    auto read = fifo.read(1);
    if (read.blockSize1 > 0) {
      value = slots[static_cast<size_t>(read.startIndex1)];
      return true;
    }
    return false;
  }

private:
  static constexpr size_t fifoSize = 8;
  std::array<T, fifoSize> slots;
  juce::AbstractFifo fifo{fifoSize};
};
