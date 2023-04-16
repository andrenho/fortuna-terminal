#include <ftclient.h>

#include <avr/interrupt.h>

#include "i2c.h"
#include "spi.h"
#include "uart.h"

int main(void)
{
    sei();
    uart_init();

    FTClient ft;

    uart_ft_init(&ft);
    // spi_ft_init(&ft);
    // i2c_init(&ft);

    ft_graphics(&ft, true);
    ft_print(&ft, "\e[1;1H\e[2J");

    while (1) {
        FT_Event e;
        while (ft_poll_event(&ft, &e)) {
            if (e.type == FTE_KEY_PRESS) {
                ft_print(&ft, "%c", e.key);
            }
        }
    }
}
