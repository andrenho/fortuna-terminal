#ifndef I2C_HH_
#define I2C_HH_

#include "communbuffered.hh"
#include "application/options.hh"

class I2C : public CommUnbuffered {
public:
    explicit I2C(I2COptions i2c_options);
    ~I2C() override;

    [[nodiscard]] std::string description() const override;

protected:
    std::string thread_exchange(std::string_view data_to_send) override;

private:
    int handle_;
    int address_;
};

#endif //I2C_HH_
