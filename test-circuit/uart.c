#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include <stdio.h>
#define USE_STDIO 1

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    for (size_t i = 0; i < bufsz; ++i) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = buf[i];
    }
    return 0;
}

static int read_cb(char* buf, size_t bufsz, void* data)
{
    // TODO - non blocking
    for (size_t i = 0; i < bufsz; ++i) {
        loop_until_bit_is_set(UCSR0A, RXC0);
        buf[i] = UDR0;
    }
    return bufsz;
}

#if USE_STDIO

static int uart_putchar(char c, FILE* f)
{
    (void) f;

    if (c == '\n')
        uart_putchar('\r', f);
	loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static int uart_getchar(FILE* f)
{
    (void) f;

	loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

#endif

void uart_init(void)
{
    // set speed
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // set config
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   // Async-mode 
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Enable Receiver and Transmitter

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    _delay_ms(100);

#if USE_STDIO
	static FILE uart = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdin = stdout = &uart;
#endif
}

void uart_ft_init(FTClient* ft)
{
    // initialize ftclient
    ftclient_init(ft, write_cb, read_cb, NULL, NULL, 512);
}
