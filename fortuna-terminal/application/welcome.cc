#include "welcome.hh"

#include "common/color.hh"

void welcome_message(SyncQueueByte& input_queue, std::string const& comm_desc)
{
    // version
    input_queue.push_all("\e[2J\e[HFortuna terminal " VERSION "\n\n\r ");

    // colors
    for (size_t j = 30; j < 38; ++j)
        input_queue.push_all("\e[2;7;" + std::to_string(j) + "m ");
    input_queue.push_all("\e[0m\n\r ");
    for (size_t j = 30; j < 38; ++j)
        input_queue.push_all("\e[0;7;" + std::to_string(j) + "m ");
    input_queue.push_all("\e[0m\n\r ");
    for (size_t j = 30; j < 38; ++j)
        input_queue.push_all("\e[1;7;" + std::to_string(j) + "m ");
    input_queue.push_all("\e[0m\n\n\r");

    // communication module
    input_queue.push_all("Communication module: \e[1;32m" + comm_desc + "\e[0m\n\n\r");
}