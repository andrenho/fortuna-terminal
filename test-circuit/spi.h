#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void spi_init(void);

void spi_printchar(uint8_t c);

uint8_t spi_getchar_nonblocking(void);
uint8_t spi_getchar_blocking(void);

#endif
