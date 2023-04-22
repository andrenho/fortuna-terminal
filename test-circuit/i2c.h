#ifndef I2C_H_
#define I2C_H_

#include <ftclient.h>

void i2c_ft_init(FTClient* ft, volatile bool* vsync);

#endif
