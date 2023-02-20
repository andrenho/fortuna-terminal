#include "uart.h"

#include <stdio.h>

int main(void) {
    uart_init();

    for (size_t i = 0; i < 5000; ++i)
        // putchar(i % (128 - 32) + 32);
        putchar('H');

    for (;;);
}
