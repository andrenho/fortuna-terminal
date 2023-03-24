#include "i2c.hh"

#include <cstdio>
#include <pigpio.h>

#define I2C_BUS 1

I2C::I2C(I2COptions i2c_options)
        : address_(i2c_options.address)
{
    handle_ = i2cOpen(I2C_BUS, i2c_options.address, 0);
}

I2C::~I2C()
{
    i2cClose(handle_);
}

std::string I2C::exchange(std::string_view data_to_send)
{
    // write output buffer
    if (data.size() > 0)
        i2cWriteDevice(handle_, (char *) data.data(), data.size());

    // read input buffer - first the size (16-bit), then the content
    uint8_t szb[2];
    i2cReadDevice(handle_, (char *) szb, 2);
    uint16_t sz = ((uint16_t) szb[1] << 8) | szb[0];

    std::string rx(sz, 0);
    if (sz > 0)
        i2cReadDevice(handle_, (char *) rx.data(), sz);

    return rx;
}

std::string I2C::description() const
{
    char buf[100];
    sprintf(buf, "I²C (address: 0x%02X)", address_);
    return buf;
}
