#include <thread>

#include "debug.hh"
#include "options.hh"
#include "comm/comm_module.hh"
#include "terminal/terminal.hh"
#include "util/sync_queue.hh"
#include "protocol/protocol.hh"
#include "event/outputevent.hh"
#include "scene/scene.hh"

int main(int argc, char* argv[])
{
    const Options options(argc, argv);
    debug_mode = options.debug_mode;

    OutputQueue output_queue;
    Scene scene;
    auto protocol = Protocol::make_protocol(options, output_queue, scene);
    auto communication_module = CommunicationModule::make_communication_module(options, protocol.get());
    auto terminal = Terminal::make_terminal(options, scene, output_queue);

    terminal->initialize();
    communication_module->initialize();

    std::thread comm_thread(&CommunicationModule::run, communication_module.get());

    while(terminal->running())
        terminal->update();

    communication_module->finalize();
    comm_thread.join();
}
