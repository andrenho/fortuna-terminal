#include "echo.hh"

#include "../global.hh"

void Echo::run_input_from_device_thread()
{
    while (running_) {
        uint8_t c = buffer.dequeue_block();
        debug_received_byte(c);
        protocol->input_char(c);
    }
}

void Echo::run_output_to_device_thread()
{
    while (running_) {
        uint8_t c = output_queue.dequeue_block();
        debug_sent_byte(c);
        buffer.enqueue(c);
    }
}

void Echo::finalize()
{
    CommunicationModule::finalize();
    buffer.release_lock();
}
