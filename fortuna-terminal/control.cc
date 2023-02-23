#include "control.hh"

static bool reset_on_next_loop_ = false;

void set_reset_on_next_loop(bool value)
{
    reset_on_next_loop_ = value;
}

bool reset_on_next_loop()
{
    return reset_on_next_loop_;
}
