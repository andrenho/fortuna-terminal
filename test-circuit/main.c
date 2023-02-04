#include "uart.h"

#include <stdio.h>

int main(void) {
    uart_init();

    printf("Hello\n");

    for (;;);
}
