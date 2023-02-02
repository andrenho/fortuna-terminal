#include <thread>

using namespace std::chrono_literals;

#include "debugmode.hh"
#include "options.hh"
#include "comm/comm_module.hh"
#include "terminal/terminal.hh"
#include "protocol/protocol.hh"
#include "event/outputevent.hh"
#include "scene/scene.hh"
#include "event/inputevent.hh"

Options options;

std::unique_ptr<CommunicationModule> communication_module = nullptr;
std::unique_ptr<Protocol>            protocol             = nullptr;
InputQueue                           input_queue;
Scene                                scene;
std::unique_ptr<Terminal>            terminal             = nullptr;
OutputQueue                          output_queue;

int main(int argc, char* argv[])
{
    // parse options
    options.parse_commandline_args(argc, argv);
    debug_mode = options.debug_mode;

    // initialization
    protocol = Protocol::make_protocol();
    communication_module = CommunicationModule::make_communication_module();
    terminal = Terminal::make_terminal();

    terminal->initialize();
    communication_module->initialize();

    // start communication thread
    std::thread input_thread(&CommunicationModule::run_input_from_device_thread, communication_module.get());
    std::thread output_thread(&CommunicationModule::run_output_to_device_thread, communication_module.get());

    // main thread loop
    while(terminal->running()) {

        auto start_frame = std::chrono::high_resolution_clock::now();

        scene.process_input_queue();

        terminal->do_events();
        terminal->draw();

        auto end_frame = std::chrono::high_resolution_clock::now();
        if (end_frame < (start_frame + 16ms))
            std::this_thread::sleep_for(end_frame - (start_frame + 16ms));
        communication_module->notify_vsync();
    }

    // clean up
    input_queue.release_lock();
    output_queue.release_lock();
    communication_module->finalize();
    std::this_thread::sleep_for(200ms);
    input_thread.detach();
    output_thread.detach();
}
