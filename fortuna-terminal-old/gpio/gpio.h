#ifndef GPIO_H_
#define GPIO_H_

#include "../error/error.h"

FT_Result gpio_init();
void      gpio_reset();
void      gpio_notify_vsync();
void      gpio_finalize();

#endif