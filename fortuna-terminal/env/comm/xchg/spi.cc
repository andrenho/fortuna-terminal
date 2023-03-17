#include "spi.hh"

#include <pigpio.h>

SPI::SPI(SPIOptions spi_options)
{
    handle_ = spiOpen(SPI_CHANNEL, spi_options.speed, 0);
}

SPI::~SPI()
{
    spiClose(handle_);
}

uint8_t SPI::exchange(uint8_t data)
{
    uint8_t ret;
    spiXfer(handle_, (char*) &data, (char*) &ret, 1);
    return ret;
}

