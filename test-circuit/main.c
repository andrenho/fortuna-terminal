#include "uart.h"
#include "i2c.h"

#include <stdio.h>

int main(void) {
	i2c_init();
	i2c_start();
    i2c_send_byte('H');
    i2c_stop();

	/*
    uart_init();

    for (size_t i = 0; i < 900; ++i)
        putchar('x');
	*/

    for (;;);
}
