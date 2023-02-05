#include "uart.h"

#include <stdio.h>

int main(void) {
    uart_init();

    for (size_t i = 0; i < 900; ++i)
        putchar('x');

    for (;;);
}
