#include <thread>

using namespace std::chrono_literals;

#include "debug.hh"
#include "options.hh"
#include "comm/comm_module.hh"
#include "terminal/terminal.hh"
#include "protocol/protocol.hh"
#include "event/outputevent.hh"
#include "scene/scene.hh"
#include "event/inputevent.hh"

int main(int argc, char* argv[])
{
    // parse options
    const Options options(argc, argv);
    debug_mode = options.debug_mode;

    // initialization
    OutputQueue output_queue;
    InputQueue input_queue;
    Scene scene;
    auto protocol = Protocol::make_protocol(options, output_queue);

    auto communication_module = CommunicationModule::make_communication_module(options);
    auto terminal = Terminal::make_terminal(options);

    terminal->initialize();
    communication_module->initialize();

    // start communication thread
    std::thread input_thread(
            &CommunicationModule::run_input_from_device_thread, communication_module.get(),
            protocol.get(), &input_queue);
    std::thread output_thread(
            &CommunicationModule::run_output_to_device_thread, communication_module.get(),
            &output_queue);

    // main thread loop
    while(terminal->running()) {

        auto start_frame = std::chrono::high_resolution_clock::now();

        scene.process_input_queue(input_queue);

        terminal->do_events(*protocol);
        terminal->draw(scene);

        auto end_frame = std::chrono::high_resolution_clock::now();
        if (end_frame < (start_frame + 16ms))
            std::this_thread::sleep_for(end_frame - (start_frame + 16ms));
        communication_module->notify_vsync();
    }

    // clean up
    communication_module->finalize();
    input_thread.join();
    output_thread.join();
}
