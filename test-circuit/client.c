#include "client.h"

#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

#include "i2c.h"
#include "spi.h"
#include "uart.h"

static CommType comm_type_;

void c_init(CommType comm_type)
{
    comm_type_ = comm_type;

    switch (comm_type) {
        case C_UART:
            uart_init();
            break;
        case C_SPI:
            spi_init();
            break;
        case C_I2C:
            i2c_init();
            break;
    }
}

void c_putchar(char c)
{
    switch (comm_type_) {
        case C_UART:
            uart_printchar(c);
            break;
        case C_SPI:
            spi_printchar(c);
            break;
        case C_I2C:
            i2c_printchar(c);
            break;
    }
}

char c_getchar_nonblock(void)
{
    switch (comm_type_) {
        case C_UART:
            return uart_getchar_nonblocking();
        case C_SPI:
            return spi_getchar_nonblocking();
        case C_I2C:
            return i2c_getchar_nonblocking();
    }
    return 0;
}

char c_getchar_block(void)
{
    switch (comm_type_) {
        case C_UART:
            return uart_getchar_blocking();
        case C_SPI:
            return spi_getchar_blocking();
        case C_I2C:
            return i2c_getchar_blocking();
    }
    return 0;
}

void c_print_P(PGM_P pstr)
{
    char c;
    while ((c = pgm_read_byte(pstr++)))
        c_putchar(c);
}

void c_printf_P(PGM_P pstr, ...)
{
    va_list ap;

    va_start(ap, pstr);
    int     sz = vsnprintf_P(NULL, 0, pstr, ap);

    char    buf[sz];
    vsprintf_P(buf, pstr, ap);
    va_end(ap);

    for (int j = 0; j < sz; ++j)
        c_putchar(buf[j]);
}
