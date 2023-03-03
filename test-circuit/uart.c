#include "uart.h"

#include <stdio.h>

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/setbaud.h>

void uart_printchar(uint8_t c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

uint8_t uart_getchar_blocking(void)
{
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

uint8_t uart_getchar_nonblocking(void)
{
    if ((UCSR0A) & (1<<RXC0))
        return UDR0;
    else
        return 0;
}

void uart_init(void)
{
    // set speed
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // set config
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);   // Async-mode 
    UCSR0B = (1 << RXEN0) | (1<<TXEN0);   // Enable Receiver and Transmitter

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    _delay_ms(100);
}

// vim:ts=4:sts=4:sw=4:expandtab
