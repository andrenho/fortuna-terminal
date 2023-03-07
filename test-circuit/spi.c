#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "buffer.h"

void spi_init(void)
{
    DDRB |= _BV(PB4);  // MOSI

    // enable SPI
    SPCR = (1<<SPE) | (1<<SPIE);
    SPDR = 0;
}

void spi_printchar(uint8_t c)
{
    if (out_buffer_sz >= BUFFER_SZ - 1)
        return;
    memmove(&out_buffer[1], out_buffer, ++out_buffer_sz);
    out_buffer[0] = c;
}

uint8_t spi_getchar_nonblocking(void)
{
    if (in_buffer_sz == 0) {
        return 0;
    } else {
        uint8_t data = in_buffer[0];
        memmove(&in_buffer[1], in_buffer, --in_buffer_sz);
        return data;
    }
}

uint8_t spi_getchar_blocking(void)
{
    while (1) {
        uint8_t data = spi_getchar_nonblocking();
        if (data != 0)
            return data;
    }
}

ISR (SPI_STC_vect)
{
    uint8_t data = SPDR;
    if (data != 0xff)
        in_buffer[in_buffer_sz++] = data;

    if (out_buffer_sz == 0) {
        SPDR = 0xff;
    } else {
        SPDR = out_buffer[out_buffer_sz--];
    }
}
