#include "comm_io.hh"

std::optional<uint8_t> CommIO::read_blocking()
{
    std::vector<uint8_t> v = read_blocking(1);
    if (v.empty())
        return {};
    else
        return v.at(0);
}

void CommIO::write(std::string const &str)
{
    std::vector<uint8_t> vec(str.begin(), str.end());
    write(vec);
}

void CommIO::write(uint8_t byte)
{
    std::vector<uint8_t> vec = { byte };
    write(vec);
}

void CommIO::execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm)
{
    input_thread_.run_without_wait([=, this]{ input_thread(input_queue_, debug_comm); });
    output_thread_.run_without_wait([=, this]{ output_thread(output_queue_, debug_comm); });
}

void CommIO::finalize_threads()
{
    input_thread_.finalize(50ms);
    output_thread_.finalize(50ms);
}

void CommIO::input_thread(SyncQueueByte* input_queue_, bool debug_comm)
{
    auto byte = read_blocking();
    if (byte.has_value()) {
        input_queue_->push(byte.value());
        if (debug_comm)
            debug_byte(byte.value(), true);
    }

}

void CommIO::output_thread(SyncQueueByte* output_queue_, bool debug_comm)
{
    std::vector<uint8_t> bytes_to_output;
    output_queue_->pop_all_into(bytes_to_output);
    write(bytes_to_output);
    if (debug_comm)
        std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(byte, false); });
}
