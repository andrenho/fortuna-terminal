#include "echo.hh"

#include "../global.hh"

void Echo::run_input_from_device_thread()
{
    while (running_) {
        protocol->input_char(buffer.dequeue_block());
    }
}

void Echo::run_output_to_device_thread()
{
    while (running_) {
        buffer.enqueue(output_queue.dequeue_block());
    }
}

void Echo::finalize()
{
    CommunicationModule::finalize();
    buffer.release_lock();
}
