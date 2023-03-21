#include "comm_fullduplex.hh"

void CommFullDuplex::write(std::string const &str)
{
    std::vector<uint8_t> vec(str.begin(), str.end());
    write(vec);
}

void CommFullDuplex::write(uint8_t byte)
{
    std::vector<uint8_t> vec = { byte };
    write(vec);
}

void CommFullDuplex::execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm)
{
    input_thread_.run_without_wait([=, this]{ input_thread(input_queue_, debug_comm); });
    output_thread_.run_without_wait([=, this]{ output_thread(output_queue_, debug_comm); });
}

void CommFullDuplex::finalize_threads()
{
    input_thread_.finalize(50ms);
    output_thread_.finalize(50ms);
}

void CommFullDuplex::input_thread(SyncQueueByte* input_queue_, bool debug_comm)
{
    std::vector<uint8_t> bytes = read_for(4ms);
    if (!bytes.empty()) {

        size_t pos = input_queue_->push_all(bytes);

        while (pos < bytes.size()) {
            std::this_thread::sleep_for(1ms);

            std::vector<uint8_t> slice(bytes.begin() + (ssize_t) pos, bytes.end());
            pos += input_queue_->push_all(slice);
        }

        if (debug_comm) {
            for (uint8_t byte: bytes)
                debug_byte(byte, true);
        }
    }
}

void CommFullDuplex::output_thread(SyncQueueByte* output_queue_, bool debug_comm)
{
    std::vector<uint8_t> bytes_to_output;
    output_queue_->pop_all_into(bytes_to_output);
    write(bytes_to_output);
    if (debug_comm)
        std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(byte, false); });
}
