#ifndef PWM_H
#define PWM_H

#include "peripheral_base.h"

// Address of the PWM registers
#define BCM_PWM_BASE BCM_PERIPHERAL_BASE + 0x20C000

#define BCM_PWM_COUNT 2

// Forward declaration
class PWM_channel;
class PWM;

class PWM_channel {
  friend class PWM;

  size_t channel_number;
  const PWM *pwm_p;

  void init(const PWM *pwm, const size_t num) {
    pwm_p = pwm; channel_number = num;}

public:
  // Enable and select mode for the PWM controller
  const PWM_channel &control(bool enable, bool markspace) const;
  // Define the range for the corresponding channel
  const PWM_channel &range(uint32_t range_) const;
  // Store the data to be sent to the channel
  const PWM_channel &data(uint32_t data_) const;
};


class PWM: public Peripheral {
  PWM_channel channel_array[BCM_PWM_COUNT];

public:
  const PWM_channel *const channel;

  PWM();

  // Access to registers
  volatile uint32_t &CTL() const {return ACCESS(0);}
  volatile uint32_t &STA() const {return ACCESS(1);}
  volatile uint32_t &DMAC() const {return ACCESS(2);}
  volatile uint32_t &FIF1() const {return ACCESS(5);}
  volatile uint32_t &RNG(unsigned int n) const {return ACCESS(4*n+4);}
  volatile uint32_t &DAT(unsigned int n) const {return ACCESS(4*n+5);}
};

const PWM_channel &PWM_channel::control
(bool enable=true, bool markspace=false) const {
  uint32_t reg = pwm_p->CTL();
  if (enable) {
    reg |= 1<<(channel_number*8);
  }
  else {
    reg &= ~(1<<(channel_number*8));
  }
  
  if (markspace) {
    reg |= 1<<(channel_number * 8 + 7);
  }
  else {
    reg &= ~(1<<(channel_number * 8 + 7));
}
  pwm_p->CTL() = reg;
  return *this;
}

const PWM_channel &PWM_channel::range(uint32_t range_) const {
  pwm_p->RNG(channel_number) = range_;
  return *this;
}

const PWM_channel &PWM_channel::data(uint32_t data_) const {
  pwm_p->DAT(channel_number) = data_;
  return *this;
}


PWM::PWM(): Peripheral(BCM_PWM_BASE), channel(channel_array) {
  for (size_t i = 0; i != BCM_PWM_COUNT; ++i) {
    channel_array[i].init(this, i);
  }
}

#endif
