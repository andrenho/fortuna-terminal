#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    for (size_t i = 0; i < bufsz; ++i) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = buf[i];
        if (buf[i] == 10) {
            loop_until_bit_is_set(UCSR0A, UDRE0);
            UDR0 = 13;
        } else if (buf[i] == 13) {
            loop_until_bit_is_set(UCSR0A, UDRE0);
            UDR0 = 10;
        }
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

void uart_init(FTClient* ft)
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

    // initialize ftclient
    ftclient_init(ft, write_cb, read_cb, NULL, NULL, 512);
}
