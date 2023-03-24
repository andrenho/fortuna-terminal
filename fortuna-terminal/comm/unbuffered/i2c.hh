#ifndef I2C_HH_
#define I2C_HH_

#include "../comm.hh"
#include "../../application/options.hh"

class I2C : public CommunicationModule {
public:
    explicit I2C(I2COptions i2c_options);
    ~I2C() override;

    std::string exchange(std::string_view data_to_send) override;
    [[nodiscard]] std::string description() const override;

private:
    int handle_;
    int address_;
};

#endif //I2C_HH_
