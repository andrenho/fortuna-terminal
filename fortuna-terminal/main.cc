#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "terminal/terminal.hh"
#include "exceptions/fortunaexception.hh"
#include "protocol/protocol.hh"
#include "gpio/gpio.hh"
#include "control.hh"

#define ALL_PROTOCOLS(...) { std::for_each(std::begin(protocols), std::end(protocols), [&](Protocol& p) { __VA_ARGS__; }); }

using namespace std::chrono_literals;

SyncQueue<Control> control;

static void on_error(Terminal* terminal, std::vector<Protocol>& protocols, size_t current_protocol, std::exception& e, bool* quit)
{
    std::cerr << "\e[1;31m" << e.what() << "\e0m\n";
    if (terminal) {
        ALL_PROTOCOLS(p.show_error(e))
        terminal->draw(protocols.at(current_protocol).scene());
        terminal->wait_for_enter(quit);
    } else {
        exit(EXIT_FAILURE);
    }
}

static void execute_control_commands(Terminal *terminal, std::vector<Protocol>& protocols, Protocol *protocol)
{
    std::optional<Control> occ;
    while ((occ = control.pop_nonblock()).has_value()) {
        Control& cc = occ.value();
        switch (cc.command) {
            case ControlCommand::Reset:
                ALL_PROTOCOLS(p.reset());
                std::cout << "Terminal reset." << std::endl;
                break;
            case ControlCommand::ResetProtocol:
                protocol->reset();
                std::cout << "Scene reset." << std::endl;
                break;
            case ControlCommand::SetMode:
                protocol->set_mode(cc.mode);
                terminal->resize_window(protocol->scene());
                std::cout << (cc.mode == Mode::Text ? "TextLayer" : "Graphics") << " mode set." << std::endl;
                break;
            case ControlCommand::SetMouseActive:
                terminal->set_mouse_active(cc.active);
                break;
            case ControlCommand::SetMouseMoveReport:
                terminal->set_mouse_register_move(cc.active);
                break;
            case ControlCommand::SetJoystickEmulation:
                terminal->set_joystick_emulation(cc.active);
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    std::unique_ptr<const Options> options;
    std::unique_ptr<Terminal> terminal = nullptr;
    std::unique_ptr<CommunicationModule> comm;
    std::unique_ptr<GPIO> gpio;

    std::vector<Protocol> protocols;
    size_t current_protocol = 0;

    Protocol* protocol;

    try {
        options = std::make_unique<const Options>(argc, argv);
        terminal = std::make_unique<Terminal>(options->terminal_options);
        comm = CommunicationModule::create_unique(options.get());
        protocols.emplace_back(options->graphics_mode ? Mode::Graphics : Mode::Text, std::move(comm), *gpio);

        protocol = &protocols.at(current_protocol);

        if (options->debug_comm) {
            ALL_PROTOCOLS(p.set_debug_comm(true));
        }

        terminal->setup_scene(protocol->scene());

        gpio->reset();

    } catch (std::exception& e) {
        on_error(terminal.get(), protocols, current_protocol, e, nullptr);
    }

    ALL_PROTOCOLS(p.run())

restart:
    try {

        bool quit = false;
        while (!quit) {
            auto frame_start = std::chrono::high_resolution_clock::now();
            execute_control_commands(terminal.get(), protocols, protocol);

            ALL_PROTOCOLS(p.scene().text().update_blink())
            protocol->execute_inputs();
            gpio->vsync();

            terminal->do_events(*protocol, &quit);
            terminal->draw(protocol->scene());

            auto frame_end = std::chrono::high_resolution_clock::now();
            if (frame_end < (frame_start + 16ms))
                std::this_thread::sleep_for(frame_end - (frame_start + 16ms));
        }

    } catch (std::exception& e) {
        bool quit = false;
        on_error(terminal.get(), protocols, current_protocol, e, &quit);
        if (!quit) {
            control.emplace(ControlCommand::Reset);
            goto restart;
        }
    }

    ALL_PROTOCOLS(p.finalize_threads())

    return EXIT_SUCCESS;
}

