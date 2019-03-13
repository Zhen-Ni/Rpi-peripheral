#ifndef TIMER_H
#define TIMER_H

#include "peripheral_base.h"

// Address of the Timer registers
#define BCM_TIMER_BASE BCM_PERIPHERAL_BASE + 0x003000

#define BCM_TIMER_COUNT 4

// Forward declaration
class Timer_channel;
class Timer;

class Timer_channel {
  friend class Timer;

  size_t channel_number;
  const Timer *timer_p;

  void init(const Timer *timer, const size_t num) {
    timer_p = timer; channel_number = num;}

public:
  // Check the status of the channel
  const bool match() const;
  // Reset the status of the channel
  const Timer_channel &reset() const;
  // Set value for the channel which is used to compare with
  // the lower 32 bits of the System Timer
  const Timer_channel &compare(uint32_t value) const;
};


class Timer: public Peripheral {
  Timer_channel channel_array[BCM_TIMER_COUNT];

public:
  const Timer_channel *const channel;

  Timer();
  // Return a Combination of the higher bits and the
  // lower bits of the counter
  uint64_t counter() const {return ACCESS64(1);}

  // Access to registers
  // Note: write 1 to CS to clear the corresponding bit.
  volatile uint32_t &CS() const {return ACCESS(0);}
  volatile uint32_t &CLO() const {return ACCESS(1);}
  volatile uint32_t &CHI() const {return ACCESS(2);}
  volatile uint32_t &C(unsigned int n) const {return ACCESS(n+3);}
};


inline const bool Timer_channel::match() const {
  return timer_p->CS() & (1<<channel_number);
}

inline const Timer_channel &Timer_channel::reset() const {
  // Write 1 to the corresponding bit to clear the status bit
  timer_p->CS() |= (1<<channel_number);
  return *this;
}

inline const Timer_channel &Timer_channel::compare(uint32_t value)
  const {
  timer_p->C(channel_number) = value;
  return *this;
}


Timer::Timer(): Peripheral(BCM_TIMER_BASE), channel(channel_array) {
  for (size_t i = 0; i != BCM_TIMER_COUNT; ++i) {
    channel_array[i].init(this, i);
  }
}

#endif
