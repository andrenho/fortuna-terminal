#include "communbuffered.hh"

CommUnbuffered::CommUnbuffered()
{
    thread_.run_with_wait([this] {
        std::string data_to_send;
        output_queue_.pop_all_into_noblock(data_to_send);

        std::string received_data = thread_exchange(data_to_send);
        if (!received_data.empty())
            input_queue_.push_all(received_data);
    });
}

std::string CommUnbuffered::exchange(std::string_view data_to_send)
{
    if (!data_to_send.empty())
        output_queue_.push_all(data_to_send);

    std::string received_data;
    input_queue_.pop_all_into_noblock(received_data);

    return received_data;
}