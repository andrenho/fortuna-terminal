#include "spi.hh"

#include <chrono>
#include <thread>
#include <pigpio.h>

using namespace std::chrono_literals;

SPI::SPI(SPIOptions spi_options)
    : speed_(spi_options.speed),
      delay_(spi_options.delay)
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
    uint8_t tx, rx;

    size_t i = 0;

    do {
        tx = 0xff;
        if (i < data.size())
            tx = data.at(i);

        spiXfer(handle_, (char *) &tx, (char *) &rx, 1);
        if (rx != 0xff)
            rx_vec.push_back(rx);

        std::this_thread::sleep_for(delay_);

        ++i;
    } while (!(tx == 0xff && rx == 0xff));

    return rx_vec;
}

std::string SPI::description() const
{
    char buf[200];
    sprintf(buf, "SPI (speed: %d hz, delay %lldus)", speed_, std::chrono::duration_cast<std::chrono::microseconds>(delay_).count());
    return buf;
}

