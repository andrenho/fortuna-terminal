#include "gpio.hh"
#include "common/exceptions/fortunaexception.hh"

#ifdef GPIO
#  include <pigpio.h>
#endif

Gpio::Gpio()
{
#ifdef GPIO
    if (gpioInitialise() < 0)
        throw FortunaException("Failed to intialize pigpio.");
#endif
}

Gpio::~Gpio()
{
#ifdef GPIO
    gpioTerminate();
#endif
}
