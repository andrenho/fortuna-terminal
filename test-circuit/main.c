#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <ftclient.h>
#include <ftclient-avr.h>

#include "i2c.h"
#include "spi.h"
#include "uart.h"

#define N_SPRITES 4

typedef struct {
    int16_t x, y;
    int8_t dir_x, dir_y;
} Sprite;

static volatile bool vsync = false;

int main(void)
{
    // vsync interrupt
    DDRD &= ~(1 << PD3);
    EICRA |= (0 << ISC11) | (1 << ISC10);

    srand(0);
    sei();
    uart_init();

    FTClient ft;

    uart_ft_init(&ft, NULL);
    // spi_ft_init(&ft);

    ft_graphics(&ft, true);
    ft_print_P(&ft, PSTR("\e[1;1H\e[2J"));

    ft_print_P(&ft, PSTR("\e*$39,0;$5,31;$9,0;$9,31;$6,0;$11,31;$5,0;$11,31;$4,0;$4,31;0;0;31;0;0;$4,31;$3,0;$4,31;0;0;31;0;0;$4,31;$3,0;$13,31;$3,0;$3,31;0;$5,31;0;$3,31;$3,0;$3,31;0;$5,31;0;$3,31;$4,0;$3,31;$5,0;$3,31;$5,0;$11,31;$6,0;$9,31;$9,0;$5,31;$22,0i"));
    ft_print_P(&ft, PSTR("\e*1;$38,0;$5,6;$9,0;$9,6;$6,0;$11,6;$5,0;$11,6;$4,0;$4,6;0;0;6;0;0;$4,6;$3,0;$4,6;0;0;6;0;0;$4,6;$3,0;$13,6;$3,0;$3,6;0;$5,6;0;$3,6;$3,0;$3,6;0;$5,6;0;$3,6;$4,0;$3,6;$5,0;$3,6;$5,0;$11,6;$6,0;$9,6;$9,0;$5,6;$22,0i"));
    ft_print_P(&ft, PSTR("\e*1;$38,0;$5,6;$9,0;$9,6;$6,0;$11,6;$5,0;$11,6;$4,0;$4,6;0;0;6;0;0;$4,6;$3,0;$4,6;0;0;6;0;0;$4,6;$3,0;$13,6;$3,0;$3,6;0;$5,6;0;$3,6;$3,0;$3,6;0;$5,6;0;$3,6;$4,0;$3,6;$5,0;$3,6;$5,0;$11,6;$6,0;$9,6;$9,0;$5,6;$22,0i"));
    ft_print_P(&ft, PSTR("\e*1;$38,0;$5,6;$9,0;$9,6;$6,0;$11,6;$5,0;$11,6;$4,0;$4,6;0;0;6;0;0;$4,6;$3,0;$4,6;0;0;6;0;0;$4,6;$3,0;$13,6;$3,0;$3,6;0;$5,6;0;$3,6;$3,0;$3,6;0;$5,6;0;$3,6;$4,0;$3,6;$5,0;$3,6;$5,0;$11,6;$6,0;$9,6;$9,0;$5,6;$22,0i"));
    ft_print_P(&ft, PSTR("\e*2;$38,0;$5,7;$9,0;$9,7;$6,0;$11,7;$5,0;$11,7;$4,0;$4,7;0;0;7;0;0;$4,7;$3,0;$4,7;0;0;7;0;0;$4,7;$3,0;$13,7;$3,0;$3,7;0;$5,7;0;$3,7;$3,0;$3,7;0;$5,7;0;$3,7;$4,0;$3,7;$5,0;$3,7;$5,0;$11,7;$6,0;$9,7;$9,0;$5,7;$22,0i"));
    ft_print_P(&ft, PSTR("\e*3;$38,0;$5,8;$9,0;$9,8;$6,0;$11,8;$5,0;$11,8;$4,0;$4,8;0;0;8;0;0;$4,8;$3,0;$4,8;0;0;8;0;0;$4,8;$3,0;$13,8;$3,0;$3,8;0;$5,8;0;$3,8;$3,0;$3,8;0;$5,8;0;$3,8;$4,0;$3,8;$5,0;$3,8;$5,0;$11,8;$6,0;$9,8;$9,0;$5,8;$22,0i"));
    ft_print_P(&ft, PSTR("\e*4;$38,0;$5,9;$9,0;$9,9;$6,0;$11,9;$5,0;$11,9;$4,0;$4,9;0;0;9;0;0;$4,9;$3,0;$4,9;0;0;9;0;0;$4,9;$3,0;$13,9;$3,0;$3,9;0;$5,9;0;$3,9;$3,0;$3,9;0;$5,9;0;$3,9;$4,0;$3,9;$5,0;$3,9;$5,0;$11,9;$6,0;$9,9;$9,0;$5,9;$22,0i"));
    ft_print_P(&ft, PSTR("\e*5;$38,0;$5,10;$9,0;$9,10;$6,0;$11,10;$5,0;$11,10;$4,0;$4,10;0;0;10;0;0;$4,10;$3,0;$4,10;0;0;10;0;0;$4,10;$3,0;$13,10;$3,0;$3,10;0;$5,10;0;$3,10;$3,0;$3,10;0;$5,10;0;$3,10;$4,0;$3,10;$5,0;$3,10;$5,0;$11,10;$6,0;$9,10;$9,0;$5,10;$22,0i"));
    ft_print_P(&ft, PSTR("\e*6;$38,0;$5,11;$9,0;$9,11;$6,0;$11,11;$5,0;$11,11;$4,0;$4,11;0;0;11;0;0;$4,11;$3,0;$4,11;0;0;11;0;0;$4,11;$3,0;$13,11;$3,0;$3,11;0;$5,11;0;$3,11;$3,0;$3,11;0;$5,11;0;$3,11;$4,0;$3,11;$5,0;$3,11;$5,0;$11,11;$6,0;$9,11;$9,0;$5,11;$22,0i"));
    ft_print_P(&ft, PSTR("\e*7;$38,0;$5,12;$9,0;$9,12;$6,0;$11,12;$5,0;$11,12;$4,0;$4,12;0;0;12;0;0;$4,12;$3,0;$4,12;0;0;12;0;0;$4,12;$3,0;$13,12;$3,0;$3,12;0;$5,12;0;$3,12;$3,0;$3,12;0;$5,12;0;$3,12;$4,0;$3,12;$5,0;$3,12;$5,0;$11,12;$6,0;$9,12;$9,0;$5,12;$22,0i"));
    ft_print_P(&ft, PSTR("\e*8;$38,0;$5,19;$9,0;$9,19;$6,0;$11,19;$5,0;$11,19;$4,0;$4,19;0;0;19;0;0;$4,19;$3,0;$4,19;0;0;19;0;0;$4,19;$3,0;$13,19;$3,0;$3,19;0;$5,19;0;$3,19;$3,0;$3,19;0;$5,19;0;$3,19;$4,0;$3,19;$5,0;$3,19;$5,0;$11,19;$6,0;$9,19;$9,0;$5,19;$22,0i"));
    ft_print_P(&ft, PSTR("\e*9;$38,0;$5,20;$9,0;$9,20;$6,0;$11,20;$5,0;$11,20;$4,0;$4,20;0;0;20;0;0;$4,20;$3,0;$4,20;0;0;20;0;0;$4,20;$3,0;$13,20;$3,0;$3,20;0;$5,20;0;$3,20;$3,0;$3,20;0;$5,20;0;$3,20;$4,0;$3,20;$5,0;$3,20;$5,0;$11,20;$6,0;$9,20;$9,0;$5,20;$22,0i"));
    ft_print_P(&ft, PSTR("\e*10;$38,0;$5,21;$9,0;$9,21;$6,0;$11,21;$5,0;$11,21;$4,0;$4,21;0;0;21;0;0;$4,21;$3,0;$4,21;0;0;21;0;0;$4,21;$3,0;$13,21;$3,0;$3,21;0;$5,21;0;$3,21;$3,0;$3,21;0;$5,21;0;$3,21;$4,0;$3,21;$5,0;$3,21;$5,0;$11,21;$6,0;$9,21;$9,0;$5,21;$22,0i"));
    ft_print_P(&ft, PSTR("\e*11;$38,0;$5,22;$9,0;$9,22;$6,0;$11,22;$5,0;$11,22;$4,0;$4,22;0;0;22;0;0;$4,22;$3,0;$4,22;0;0;22;0;0;$4,22;$3,0;$13,22;$3,0;$3,22;0;$5,22;0;$3,22;$3,0;$3,22;0;$5,22;0;$3,22;$4,0;$3,22;$5,0;$3,22;$5,0;$11,22;$6,0;$9,22;$9,0;$5,22;$22,0i"));
    ft_print_P(&ft, PSTR("\e*12;$38,0;$5,23;$9,0;$9,23;$6,0;$11,23;$5,0;$11,23;$4,0;$4,23;0;0;23;0;0;$4,23;$3,0;$4,23;0;0;23;0;0;$4,23;$3,0;$13,23;$3,0;$3,23;0;$5,23;0;$3,23;$3,0;$3,23;0;$5,23;0;$3,23;$4,0;$3,23;$5,0;$3,23;$5,0;$11,23;$6,0;$9,23;$9,0;$5,23;$22,0i"));
    ft_print_P(&ft, PSTR("\e*13;$38,0;$5,29;$9,0;$9,29;$6,0;$11,29;$5,0;$11,29;$4,0;$4,29;0;0;29;0;0;$4,29;$3,0;$4,29;0;0;29;0;0;$4,29;$3,0;$13,29;$3,0;$3,29;0;$5,29;0;$3,29;$3,0;$3,29;0;$5,29;0;$3,29;$4,0;$3,29;$5,0;$3,29;$5,0;$11,29;$6,0;$9,29;$9,0;$5,29;$22,0i"));
    ft_print_P(&ft, PSTR("\e*14;$38,0;$5,31;$9,0;$9,31;$6,0;$11,31;$5,0;$11,31;$4,0;$4,31;0;0;31;0;0;$4,31;$3,0;$4,31;0;0;31;0;0;$4,31;$3,0;$13,31;$3,0;$3,31;0;$5,31;0;$3,31;$3,0;$3,31;0;$5,31;0;$3,31;$4,0;$3,31;$5,0;$3,31;$5,0;$11,31;$6,0;$9,31;$9,0;$5,31;$22,0i"));
    ft_print_P(&ft, PSTR("\e*15;$38,0;$5,15;$9,0;$9,15;$6,0;$11,15;$5,0;$11,15;$4,0;$4,15;0;0;15;0;0;$4,15;$3,0;$4,15;0;0;15;0;0;$4,15;$3,0;$13,15;$3,0;$3,15;0;$5,15;0;$3,15;$3,0;$3,15;0;$5,15;0;$3,15;$4,0;$3,15;$5,0;$3,15;$5,0;$11,15;$6,0;$9,15;$9,0;$5,15;$22,0i"));

    Sprite sprite[N_SPRITES] = {0};
    for (size_t i = 0; i < N_SPRITES; ++i) {
        int16_t x = rand() % (256 - 16);
        int16_t y = rand() % (256 - 16);
        int8_t dir_x = ((int8_t) rand() % 8) - 4;
        int8_t dir_y = ((int8_t) rand() % 8) - 4;
        ft_sprite_4(&ft, i, x, y, true, false, false, 0, i % 16);
        sprite[i] = (Sprite) { .x = x, .y = y, .dir_x = dir_x, .dir_y = dir_y };
    }

    ft_enable_vsync(&ft, true);

    /*
    while (1) {
        FT_Event e;
        while (ft_poll_event(&ft, &e)) {
            if (e.type == FTE_KEY_PRESS) {
                ft_printf(&ft, "%c", e.key);
            } else if (e.type == FTE_VSYNC) {
                for (size_t i = 0; i < N_SPRITES; ++i) {
                    Sprite* s = &sprite[i];
                    s->x += s->dir_x;
                    s->y += s->dir_y;
                    if (s->x < 0 || s->x > (256-16))
                        s->dir_x *= -1;
                    if (s->y < 0 || s->y > (256-16))
                        s->dir_y *= -1;
                    ft_sprite_0(&ft, i, s->x, s->y);
                }
            }
        }
    }
    */

    while (1) {
        if (vsync) {
            for (size_t i = 0; i < N_SPRITES; ++i) {
                Sprite* s = &sprite[i];
                s->x += s->dir_x;
                s->y += s->dir_y;
                if (s->x < 0 || s->x > (256-16))
                    s->dir_x *= -1;
                if (s->y < 0 || s->y > (256-16))
                    s->dir_y *= -1;
                ft_sprite_0(&ft, i, s->x, s->y);
            }
            vsync = false;
        }
    }
}

ISR(INT1_vect)
{
    vsync = true;
}
