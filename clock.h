#ifndef CLOCK_H
#define CLOCK_H

#include "peripheral_base.h"

// Address of the Clock registers
#define BCM_CLOCK_BASE BCM_PERIPHERAL_BASE + 0x101000

#define BCM_CLOCK_COUNT 4
enum CLOCK_NAME {CLK0, CLK1, CLK2, CLKPWM};

#define PASSWD 0x5A
enum CLOCK_SOURCE {GND, OSCILLATOR, TESTDEBUG0, TESTDEBUG1,
		   PLLA_PER, PLLC_PER, PLLD_PER,HDMI_AUXILIARY};
enum MASH_FILTER {MASH0, MASH1, MASH2, MASH3};

// Forward declaration
class Clock_manager;
class Clock;


class Clock_manager{
  friend class Clock;

  CLOCK_NAME clock_name;
  const Clock *clock_p;

  void init(const Clock *clock, const CLOCK_NAME name) {
    clock_p = clock; clock_name = name;}

public:
  // Check whether the clock generator is running.
  bool busy() const;

  // Setups for the clock
  const Clock_manager &control(CLOCK_SOURCE source, bool enable,
			       MASH_FILTER filter) const;
  // It seems that setting divi = 1 results in some strange results...
  const Clock_manager &divisor(unsigned short divi,
			       unsigned short divf) const;
};


class Clock: public Peripheral {
  Clock_manager manager_array[BCM_CLOCK_COUNT];

public:
  const Clock_manager *const manager;

  Clock();

  // Access to clock registers
  volatile uint32_t &GPCTL(CLOCK_NAME name) const {
    return (name==CLKPWM)?ACCESS(40):ACCESS(name*2+28);}
  volatile uint32_t &GPDIV(CLOCK_NAME name) const {
    return (name==CLKPWM)?ACCESS(41):ACCESS(name*2+29);}

};


inline bool Clock_manager::busy() const {
  return clock_p->GPCTL(clock_name) & (1<<7);
}

const Clock_manager &Clock_manager::control
(CLOCK_SOURCE source=OSCILLATOR, bool enable=true, MASH_FILTER mash=MASH0) const {
  uint32_t reg = (source & 0xF) | (enable<<4) |
    (mash<<9) | (PASSWD<<24);
  clock_p->GPCTL(clock_name) = reg;
  return *this;
}

const Clock_manager &Clock_manager::divisor
(unsigned short divi=1, unsigned short divf=0) const {
  divi &= 0xFFF;
  divf &= 0xFFF;
  uint32_t reg = (divi<<12) | divf | (PASSWD<<24);
  clock_p->GPDIV(clock_name) = reg;
  return *this;
}


Clock::Clock(): Peripheral(BCM_CLOCK_BASE), manager(manager_array) {
  for (size_t i = 0; i != BCM_CLOCK_COUNT; ++i) {
    manager_array[i].init(this, static_cast<CLOCK_NAME>(i));
  }
}

#endif
