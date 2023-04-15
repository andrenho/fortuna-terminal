#include <ftclient.h>

#include "uart.h"

int main(void)
{
    FTClient ft;

    uart_init(&ft);

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
