#ifndef SPI_HH_
#define SPI_HH_

#include "commexchange.hh"
#include "../../../application/options.hh"

#define SPI_CHANNEL 0

class SPI : public CommExchange {
public:
    explicit SPI(SPIOptions spi_options);
    ~SPI() override;

    uint8_t exchange(uint8_t data) override;

private:
    int    handle_ = -1;
};

#endif //SPI_HH_
