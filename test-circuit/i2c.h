#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

void i2c_init(void);
int i2c_start(void);
int i2c_send_byte(uint8_t c);
void i2c_stop(void);

#endif
