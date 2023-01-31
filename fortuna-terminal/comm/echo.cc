#include "echo.hh"

void Echo::run_input_from_device_thread(Protocol* protocol, InputQueue* input_queue)
{
    while (running_) {
        std::optional<uint8_t> o_byte;
        while ((o_byte = buffer.pop()))
            protocol->input(*o_byte, *input_queue);
    }
}

void Echo::run_output_to_device_thread(OutputQueue *queue)
{
    while (running_) {
        std::optional<uint8_t> o_byte;
        while ((o_byte = queue->pop())) {
            buffer.push(*o_byte);
        }
    }
}