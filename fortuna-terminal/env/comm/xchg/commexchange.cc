#include "commexchange.hh"

void CommExchange::execute_threads(SyncQueueByte *input_queue_, SyncQueueByte *output_queue_, bool debug_comm)
{
    thread_.run_with_wait([=, this]{ exchange_thread(input_queue_, output_queue_, debug_comm); });
}

void CommExchange::finalize_threads()
{
    thread_.finalize(50ms);
}

void CommExchange::notify_threads()
{
    thread_.notify();
}

void CommExchange::exchange_thread(SyncQueueByte *input_queue_, SyncQueueByte *output_queue_, bool debug_comm)
{
    std::vector<uint8_t> data_to_send, data_to_receive;
    output_queue_->pop_all_into_noblock(data_to_send);
    data_to_receive = exchange(data_to_send);
    if (debug_comm) {
        for (uint8_t b: data_to_send)
            debug_byte(b, true);
        for (uint8_t b: data_to_receive)
            debug_byte(b, false);
    }
    input_queue_->push_all(data_to_receive);
}

