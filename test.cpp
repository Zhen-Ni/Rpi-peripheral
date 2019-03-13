#include <iostream>
#include <thread>
#include "gpio.h"
#include "pwm.h"
#include "clock.h"
#include "timer.h"

using namespace std;


int test_clock() {
  GPIO gpio;
  gpio.pin[4].alternate_function(0);
  Clock clock;
  clock.manager[CLK0].control(PLLD_PER,true);
  clock.manager[CLK0].divisor(12,0);
  return 0;
}


int test_pwm() {
  GPIO gpio;
  gpio.pin[19].alternate_function(5);
  Clock clock;
  clock.manager[CLKPWM].control(OSCILLATOR,true);
  clock.manager[CLKPWM].divisor(2,0);
  
  PWM pwm;
  auto channel = pwm.channel[1];
  channel.control(true,true);
  uint32_t range = 1024, data = 0;
  channel.range(range);
  channel.data(data);
  bool increase = true;
  while (true) {
    if (data == 0) {increase = true;usleep(500000);}
    if (data == range) increase = false;
    if (increase)
      ++data;
    else {
      --data;
    }
    channel.data(data);
    usleep(1000);
  }
  return 0;
}


int test_gpio() {
  GPIO gpio;
  const unsigned int ports[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
				15,16,17,18,19,20,21,22,23,24,25,26,
				27};
  gpio.pull_up_down(UP, ports);
  gpio.pin[4].function_select(OUTPUT);
  gpio.pin[7].input();
  while (true) {
    gpio.pin[27].level() ? gpio.pin[4].set() : gpio.pin[4].clear(); 
    //cout << gpio.pin[4].level() << " " << gpio.pin[27].level() << " " << "\r";
    usleep(1000);
  }
  return 0;
}

int test_timer() {
  Timer timer;
  uint32_t CLO, last_CLO(timer.CLO());
  uint64_t counter, last_counter(timer.counter());
  timer.channel[0].reset();
  timer.channel[1].reset();
  timer.channel[1].compare(last_CLO+5000000);
  cout << timer.CS() << endl;
  
  while (true) {
    sleep(1);
    CLO = timer.CLO();
    counter = timer.counter();
    cout << CLO << '\t'<<CLO-last_CLO << endl;
    cout << counter <<'\t'<<counter-last_counter<< endl;
    last_CLO = CLO;
    last_counter = counter;
    cout << timer.C(0) <<'\t' << timer.C(1) <<'\t'<<timer.C(2)<<'\t'<<timer.C(3)<< endl;
    cout << timer.CS() << endl;
  }
  return 0;
}


int main() {
  thread gpio(test_gpio);
  //test_clock();
  thread pwm(test_pwm);
  thread timer(test_timer);
  gpio.join();
  pwm.join();
  timer.join();
}

