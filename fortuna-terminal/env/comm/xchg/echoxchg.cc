#include "echoxchg.hh"

uint8_t EchoXchg::exchange(uint8_t data)
{
    uint8_t return_value;
    if (buffer_.empty()) {
        return_value = 0xff;
    } else {
        return_value = buffer_.front();
        buffer_.pop();
    }

    if (data == 13) {
        buffer_.push(13);
        buffer_.push(10);
    } else if (data != 0xff) {
        buffer_.push(data);
    }
    return return_value;
}
