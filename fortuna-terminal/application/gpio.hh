#ifndef GPIO_HH_
#define GPIO_HH_

#include "common/iterativethread.hh"

class Gpio {
public:
    Gpio();
    ~Gpio();

    void reset();
    void vsync();

private:
    IterativeThread vsync_thread_;

    static constexpr unsigned PIN_VSYNC = 27;
    static constexpr unsigned PIN_RESET = 25;
};

#endif //GPIO_HH_
