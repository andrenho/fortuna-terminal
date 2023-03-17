#include "gpio.hh"
#include "../common/exceptions/fortunaexception.hh"

#ifdef GPIO
#  include <piopio.h>
#endif

GPIO::GPIO()
{
#ifdef GPIO
    if (gpioInitialize() < 0)
        throw FortunaException("Failed to intialize pigpio.");
#endif
}

GPIO::~GPIO()
{
#ifdef GPIO
    gpioTerminate();
#endif
}

