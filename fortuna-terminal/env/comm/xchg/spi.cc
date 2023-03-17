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

std::vector<uint8_t> SPI::exchange(std::vector<uint8_t> const &data)
{
    std::vector<uint8_t> rx_vec;
    size_t i = 0;
    do {
        uint8_t tx = 0xff;
        if (i < data.size())
            tx = data.at(i);

        uint8_t rx;
        spiXfer(handle_, &tx, &rx, 1);
        if (rx != 0xff)
            rx_vec.push_back(rx);

        ++i;
    } while (tx != 0xff || rx != 0xff);

    return rx_buf;
}

