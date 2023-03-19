#ifndef I2C_HH_
#define I2C_HH_

#include "comm_halfduplex.hh"
#include "../../../application/options.hh"

#define I2C_BUS 1

class I2C : public CommHalfDuplex {
public:
    explicit I2C(I2COptions i2c_options);
    ~I2C() override;

    std::vector<uint8_t> exchange(std::vector<uint8_t> const &data) override;

    [[nodiscard]] std::string description() const override;

private:
    int handle_;
    int address_;
};

#endif //I2C_HH_
