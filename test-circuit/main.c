#include <ftclient.h>

#include <avr/interrupt.h>

#include "i2c.h"
#include "spi.h"
#include "uart.h"

int main(void)
{
    sei();

    FTClient ft;

    // uart_init(&ft);
    spi_init(&ft);

    ft_print(&ft, "Hello world!\n");

    while (1) {
        FT_Event e;
        while (ft_poll_event(&ft, &e)) {
            if (e.type == FTE_KEY_PRESS) {
                ft_print(&ft, "%c", e.key);
            }
        }
    }
}
