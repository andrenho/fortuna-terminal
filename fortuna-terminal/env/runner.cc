#include "runner.hh"
#include "../common/syncqueue.hh"
#include "comm/comm.hh"
#include "comm/io/comm_io.hh"

#include <memory>
#include <thread>

void Runner::run_io_threads(SyncQueueByte& input_queue, SyncQueueByte& output_queue, CommunicationModule& comm)
{
    if (comm.channels() == Channels::InputAndOutput) {
        auto comm_io = dynamic_cast<CommIO*>(&comm);

        input_thread_ = std::make_unique<std::thread>([this, &input_queue, comm_io]() {
            while (threads_active_) {
                auto byte = comm_io->read_blocking();
                if (byte.has_value()) {
                    input_queue.push(byte.value());
                    // debug_byte(true, byte.value());
                }
            }
        });

        output_thread_ = std::make_unique<std::thread>([this, &output_queue, comm_io]() {
            while (threads_active_) {
                std::vector<uint8_t> bytes_to_output;
                output_queue.pop_all_into(bytes_to_output);
                comm_io->write(bytes_to_output);
                /*
                if (debug_comm_)
                    std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(false, byte); });
                    */
            }
        });
    }
}