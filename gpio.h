#ifndef GPIO_H
#define GPIO_H

#include "peripheral_base.h"
#include "misc.h"

// Address of the GPIO registers
#define BCM_GPIO_BASE BCM_PERIPHERAL_BASE + 0x200000

#define BCM_GPIO_COUNT 54

enum PIN_FUNCTION {INPUT, OUTPUT, ALT5, ALT4, ALT0, ALT1, ALT2, ALT3};
enum PUD_STATUS {OFF, DOWN, UP};

// Forward declaration
class GPIO_pin;
class GPIO;


class GPIO_pin {
  friend class GPIO;
  
  size_t pin_number;
  const GPIO *gpio_p;

  void init(const GPIO *gpio, const size_t num) {
    gpio_p = gpio; pin_number = num;}

public:
  // Check and select pin function
  const PIN_FUNCTION function() const;
  const GPIO_pin &input() const;
  const GPIO_pin &output() const;
  const GPIO_pin &alternate_function(unsigned int altn) const;
  const GPIO_pin &function_select(PIN_FUNCTION funcn) const;

  // Set or clear the GPIO pin
  const GPIO_pin &set() const;
  const GPIO_pin &clear() const;
  // Read the level of the GPIO pin
  const bool level() const;

  // Control the corresponding GPIO Clock pull-up/down register.
  const GPIO_pin &pud_clock(bool sel) const;
};


class GPIO: public Peripheral {
  GPIO_pin pin_array[BCM_GPIO_COUNT];

public:
  const GPIO_pin *const pin;
  
  GPIO();

  // Access to registers.
  volatile uint32_t &GPFSEL(unsigned int n) const {return ACCESS(n);}
  volatile uint32_t &GPSET(unsigned int n) const {return ACCESS(n+7);}
  volatile uint32_t &GPCLR(unsigned int n) const {return ACCESS(n+10);}
  volatile uint32_t &GPLEV(unsigned int n) const {return ACCESS(n+13);}
  volatile uint32_t &GPPUD() const {return ACCESS(37);}
  volatile uint32_t &GPPUDCLK(unsigned int n) const {return ACCESS(n+38);}

  // Pull-up/down
  // It seems that we can't pull down GPIO pin 2 and pin 3
  template<typename T>
  void pull_up_down(PUD_STATUS status, const T &ports) const;
  
};


inline const PIN_FUNCTION GPIO_pin::function() const {
  uint32_t reg = gpio_p->GPFSEL(pin_number/10);
  unsigned int pos = pin_number % 10 * 3;
  (reg &= (7<<pos)) >>= pos;
  return PIN_FUNCTION(reg);
}

inline const GPIO_pin &GPIO_pin::input() const {
  gpio_p->GPFSEL(pin_number/10) &= ~(7<<(pin_number%10*3));
  return *this;
}

inline const GPIO_pin &GPIO_pin::output() const {
  volatile uint32_t &reg = gpio_p->GPFSEL(pin_number/10);
  unsigned int pos = pin_number % 10 * 3;
  (reg &= ~(7<<pos)) |= 1<<pos;
  return *this;
}

inline const GPIO_pin &GPIO_pin::alternate_function
(unsigned int altn) const {
  volatile uint32_t &reg = gpio_p->GPFSEL(pin_number/10);
  unsigned int pos = pin_number % 10 * 3;
  (reg &= ~(7<<pos)) |= (altn<=3?altn+4:altn==4?3:2)<<pos;
  return *this;
}

inline const GPIO_pin &GPIO_pin::function_select
(PIN_FUNCTION funcn) const {
  volatile uint32_t &reg = gpio_p->GPFSEL(pin_number/10);
  unsigned int pos = pin_number % 10 * 3;
  (reg &= ~(7<<pos)) |= funcn<<pos;
  return *this;
}

inline const GPIO_pin &GPIO_pin::set() const {
  gpio_p->GPSET(pin_number/32) = 1<<pin_number%32;
  return *this;
};

inline const GPIO_pin &GPIO_pin::clear() const {
  gpio_p->GPCLR(pin_number/32) = 1<<pin_number%32;
  return *this;
};

inline const bool GPIO_pin::level() const {
  return gpio_p->GPLEV(pin_number/32) &= (1<<pin_number%32);
}

const GPIO_pin &GPIO_pin::pud_clock(bool sel) const {
  volatile uint32_t &reg = gpio_p->GPPUDCLK(pin_number/32);
  sel ? reg |= (1<<pin_number) : reg &= ~(1<<pin_number);
  return *this;
}


GPIO::GPIO(): Peripheral(BCM_GPIO_BASE), pin(pin_array) {
  // set up pins
  for (size_t i = 0; i != BCM_GPIO_COUNT; ++i) {
    pin_array[i].init(this, i);
  }
}


template <typename T>
void GPIO::pull_up_down(PUD_STATUS status, const T &ports) const {
  // Set the required control signal
  GPPUD() = status;
  // Wait 150 cycles
  Unroll<150>::call([](){asm("nop");});
  // Clock the control signal into the GPIO pads
  for(unsigned int i: ports) {
    pin[i].pud_clock(true);
  }
  // Wait 150 cycles
  Unroll<150>::call([](){asm("nop");});
  // Remove the control signal
  GPPUD() = 0;
  // Remove the clock
  for(unsigned int i: ports) {
    pin[i].pud_clock(false);
  }
}
  

#endif
