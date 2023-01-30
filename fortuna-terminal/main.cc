#include <thread>

#include "debug.hh"
#include "options.hh"
#include "comm/comm_module.hh"
#include "event/output_event.hh"
#include "terminal/terminal.hh"
#include "util/sync_queue.hh"

int main(int argc, char* argv[])
{
    const Options options(argc, argv);
    debug_mode = options.debug_mode;

    auto output_event_queue = SyncQueue<OutputEvent>();

    auto communication_module = CommunicationModule::make_communication_module(options, output_event_queue);
    auto terminal = Terminal::make_terminal(options, *communication_module);

    terminal->initialize();
    communication_module->initialize();

    std::thread comm_thread(&CommunicationModule::run, communication_module.get());

    while(terminal->running())
        terminal->update();

    communication_module->finalize();
    comm_thread.join();
}
