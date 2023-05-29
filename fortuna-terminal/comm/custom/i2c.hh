#ifndef I2C_HH_
#define I2C_HH_

#include "application/options.hh"
#include "../comm.hh"

class I2C : public CommunicationModule {
public:
    explicit I2C(I2COptions i2c_options);
    ~I2C() override;

    [[nodiscard]] std::string description() const override;

protected:
    [[nodiscard]] virtual std::vector<uint8_t> exchange(std::vector<uint8_t> data_to_send) override;

private:
    int handle_;
    int address_;
};

#endif //I2C_HH_
