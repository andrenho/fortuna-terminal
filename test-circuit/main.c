#include "client.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "spi.h"

volatile uint8_t data = 0;

int main(void)
{
    c_init(C_SPI);
    sei();

    c_print_P(PSTR("Welcome to Fortuna Terminal test device!\e[0m\n\r\n\r"));

    for (;;) {
        char c = c_getchar_block();
        if (c == '\r') {
            c_putchar('\n');
            c_putchar('\r');
        } else {
            c_putchar(c);
        }
    }
}
