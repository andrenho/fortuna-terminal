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
    static const uint8_t NO_VALUE = 0xff;
    uint8_t buf = 0;

    std::vector<uint8_t> rx_buf(data.size(), NO_VALUE);
    spiXfer(handle_, data.data(), rx_buf.data(), data.size());
    if (rx_buf.back() == NO_VALUE) {
        spiXfer(handle_, data.data(), &buf, 1);
    } else {
        while (rx_buf.back() != 0xff) {
            spiXfer(handle_, &NO_VALUE, &buf, 1);
            rx_buf.push_back(buf);
        }
    }

    return rx_buf;
}

