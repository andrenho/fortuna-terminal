#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

void i2c_init(void);

void i2c_printchar(uint8_t c);

uint8_t i2c_getchar_nonblocking(void);
uint8_t i2c_getchar_blocking(void);

#endif
