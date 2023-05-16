#makefile
cc = g++
all: test test_gpio
test:
	$(cc) test.cpp -o test
test_gpio:
	$(cc) test_gpio.cpp -o test_gpio
.PHONY:clean
clean:
	-rm -f test-autorun
	-rm -f autorun.o
