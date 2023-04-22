#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "fifo.h"

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    for (size_t i = 0; i < bufsz; ++i)
        fifo_push(&out_fifo, buf[i]);
    return 0;
}

static int read_cb(char* buf, size_t bufsz, void* data)
{
    size_t i = 0;
    for (i = 0; i < bufsz; ++i) {
        uint8_t c;
        bool has = fifo_pop(&in_fifo, &c);
        if (has)
            buf[i] = c;
        else
            break;
    }
    return i;
}

void spi_ft_init(FTClient* ft, volatile bool* vsync)
{
    DDRB |= _BV(PB4);  // MOSI

    // enable SPI
    SPCR = (1<<SPE) | (1<<SPIE);
    SPDR = 0xff;

	// initialize fifo queues
    fifo_init(&in_fifo);
    fifo_init(&out_fifo);

    // initialize ftclient
    ftclient_init(ft, (FTClientSetup) {
        .write_cb = write_cb,
        .read_cb = read_cb,
        .finalize = NULL,
        .data = NULL,
        .vsync = vsync,
        .bufsz = 512
    });
}

ISR (SPI_STC_vect)
{
    uint8_t data = SPDR;

    if (data != 0xff)
        fifo_push(&in_fifo, data);

    uint8_t c;
    if (fifo_pop(&out_fifo, &c))
        SPDR = c;
    else
        SPDR = 0xff;
}
