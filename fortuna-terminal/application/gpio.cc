#include "gpio.hh"
#include "common/exceptions/fortunaexception.hh"

#ifdef GPIO
#  include <pigpio.h>
#endif

using namespace std::chrono_literals;

Gpio::Gpio()
{
#ifdef GPIO
    if (gpioInitialise() < 0)
        throw FortunaException("Failed to intialize pigpio.");

    gpioSetMode(PIN_VSYNC, PI_OUTPUT);
    gpioWrite(PIN_VSYNC, 1);

    gpioSetMode(PIN_RESET, PI_INPUT);

    vsync_thread_.run_with_wait([this]{ gpioTrigger(PIN_VSYNC, 100, 0); });
#endif
}

Gpio::~Gpio()
{
#ifdef GPIO
    gpioTerminate();
#endif
}

void Gpio::reset()
{
#ifdef GPIO
    gpioSetMode(PIN_RESET, PI_OUTPUT);
    gpioWrite(PIN_RESET, 0);
    std::this_thread::sleep_for(200ms);
    gpioSetMode(PIN_RESET, PI_INPUT);
#endif
}

void Gpio::vsync()
{
#ifdef GPIO
    vsync_thread_.notify();
#endif
}

