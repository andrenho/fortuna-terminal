#ifndef SPI_HH_
#define SPI_HH_

#include <string_view>
#include "comm/comm.hh"
#include "application/options.hh"
#include "common/types/time.hh"

class SPI : public CommunicationModule {
public:
    explicit SPI(SPIOptions spi_options);
    ~SPI() override;

    std::string description() const override;

protected:
    [[nodiscard]] virtual std::vector<uint8_t> exchange(std::vector<uint8_t> data_to_send) override;

private:
    int      handle_ = -1;
    int      speed_;
    Duration delay_;
};

#endif //SPI_HH_
