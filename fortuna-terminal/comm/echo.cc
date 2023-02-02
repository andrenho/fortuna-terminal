#include "echo.hh"

void Echo::run_input_from_device_thread()
{
    while (running_) {
        protocol_.input_char(buffer.dequeue_block());
    }
}

void Echo::run_output_to_device_thread()
{
    while (running_) {
        buffer.enqueue(output_queue_.dequeue_block());
    }
}

void Echo::finalize()
{
    CommunicationModule::finalize();
    buffer.release_lock();
}
