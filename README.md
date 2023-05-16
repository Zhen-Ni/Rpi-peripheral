# Rpi-peripheral
This is a header-only C++ library for controlling peripherals of raspberry pi.

This library provides control over GPIO, pwm, the internal clock, and etc. The code is tested on RPi 3B and should work on other modules with little modification.

Since it is a header-only library and has minimal dependency, it is easy to apply the code to bare metal development of RPi. 
This can be done by a little modification on `base.h`, by ignoring its memory map and using physical addresses directly.
