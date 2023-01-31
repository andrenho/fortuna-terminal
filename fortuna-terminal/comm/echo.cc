#include "echo.hh"

#include <cstdio>

void Echo::run_input_from_device_thread(Protocol* protocol, InputQueue* input_queue)
{
    while (running_) {
        std::optional<uint8_t> o_byte;
        while ((o_byte = buffer.pop()))
            protocol->input(o_byte.value(), *input_queue);
    }
}

void Echo::write_to_device(std::vector<uint8_t> data)
{
    for (uint8_t byte: data)
        buffer.push(byte);
}

