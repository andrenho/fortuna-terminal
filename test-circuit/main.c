#include "client.h"

#include <avr/pgmspace.h>

int main(void)
{
    c_init(C_UART);
    c_print_P(PSTR("\e[2J\e[H\e[1;36mWelcome to Fortuna Terminal!\e[0m\n\r\n\r"));

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
