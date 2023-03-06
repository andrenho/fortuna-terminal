#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "buffer.h"

void spi_init(void)
{
    // SS, MOSI & SCK as input, MISO as output
    // DDRB &= ~_BV(PB2) | ~_BV(PB3) | ~_BV(PB5);
    DDRB |= _BV(PB4);
    // PORTB |= _BV(PB3);  // pullup on MOSI

    // enable SPI
    SPCR = (1<<SPE) | (1<<SPIE);
    SPDR = 0;
}

void spi_printchar(uint8_t c)
{
}

uint8_t spi_getchar_nonblocking(void)
{
    if (buffer_sz == 0)
        return 0;
    uint8_t data = buffer[0];
    --buffer_sz;
    memmove(buffer, &buffer[1], buffer_sz);
    return data;
}

uint8_t spi_getchar_blocking(void)
{
    while (1) {
        uint8_t data = spi_getchar_nonblocking();
        if (data == 0)
            return data;
    }
}

extern uint8_t data;

ISR (SPI_STC_vect)
{
    data = SPDR;
    SPDR = data;
}
