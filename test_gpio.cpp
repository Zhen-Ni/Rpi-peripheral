#include "peripheral/peripheral.h"
#include <iostream>

int main() {
  GPIO gpio;
  const unsigned int ports[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
				15,16,17,18,19,20,21,22,23,24,25,26,
				27};
  gpio.pull_up_down(UP, ports);
  std::cout << "Pull GPIO 2 up and down each second." << std::endl;
  gpio.pin[2].function_select(OUTPUT);
  while (true) {
    std::cout << "Pull GPIO 2 up." << std::endl;
    gpio.pin[2].set();
    usleep(1000000);
    std::cout << "Pull GPIO 2 down." << std::endl;
    gpio.pin[2].clear();
    usleep(1000000);
  }
  return 0;
}
