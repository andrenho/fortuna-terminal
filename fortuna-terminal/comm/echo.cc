#include "echo.hh"

void Echo::run_input_from_device_thread(Protocol* protocol, InputQueue* input_queue)
{
    while (running_) {
        protocol->input(buffer.dequeue_block(), *input_queue);
    }
}

void Echo::run_output_to_device_thread(OutputQueue *output_queue)
{
    while (running_) {
        buffer.enqueue(output_queue->dequeue_block());
    }
}

void Echo::finalize()
{
    CommunicationModule::finalize();
    buffer.release_lock();
}
