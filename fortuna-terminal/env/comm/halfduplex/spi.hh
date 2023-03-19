#ifndef SPI_HH_
#define SPI_HH_

#include "comm_halfduplex.hh"
#include "application/options.hh"
#include "common/types/time.hh"

#define SPI_CHANNEL 0

class SPI : public CommHalfDuplex {
public:
    explicit SPI(SPIOptions spi_options);
    ~SPI() override;

    std::vector<uint8_t> exchange(std::vector<uint8_t> const &data) override;

    std::string description() const override;

private:
    int      handle_ = -1;
    int      speed_;
    Duration delay_;
};

#endif //SPI_HH_
