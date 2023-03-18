#include "i2c.hh"

#include <pigpio.h>

I2C::I2C(I2COptions i2c_options)
{
    handle_ = i2cOpen(I2C_BUS, i2c_options.address, 0);
}

I2C::~I2C()
{
    i2cClose(handle_);
}

std::vector<uint8_t> I2C::exchange(std::vector<uint8_t> const &data)
{
    // write output buffer
    if (data.size() > 0)
        i2cWriteDevice(handle_, (char *) data.data(), data.size());

    // read input buffer - first the size (16-bit), then the content
    uint8_t szb[2];
    i2cReadDevice(handle_, (char *) szb, 2);
    uint16_t sz = ((uint16_t) szb[1] << 8) | szb[0];

    std::vector<uint8_t> rx(sz);
    if (sz > 0)
        i2cReadDevice(handle_, (char *) rx.data(), sz);

    return rx;
}
