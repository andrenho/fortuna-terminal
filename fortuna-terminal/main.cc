#include <iostream>
#include <memory>

#include "terminal/terminal.hh"
#include "terminal/scene/sceneevent.hh"
#include "exceptions/fortunaexception.hh"
#include "protocol/protocol.hh"
#include "comm/echo.hh"

static std::unique_ptr<Terminal> initialize_terminal(TerminalOptions terminal_options)
{
    std::unique_ptr<Terminal> terminal;

    try {
        terminal = std::make_unique<Terminal>(terminal_options);
        terminal->add_scene();
    } catch (std::exception& e) {
        std::cerr << "\e[1;31m" << e.what() << "\e[0m\n";
        exit(EXIT_FAILURE);
    }

    return terminal;
}

static std::vector<std::unique_ptr<Protocol>> initialize_protocols(SyncQueue<SceneEvent> &scene_queue, Size size)
{
    std::vector<std::unique_ptr<Protocol>> protocols;
    auto echo = std::make_unique<Echo>();
    protocols.push_back(Protocol::create_unique(ProtocolType::Ansi, std::move(echo), scene_queue, 0, size));
    return protocols;
}

int main(int argc, char* argv[])
{
    (void) argc; (void) argv;

    int exit_status = EXIT_SUCCESS;

    SyncQueue<SceneEvent> scene_queue;
    SyncQueue<FP_Message> event_queue;

    auto terminal = initialize_terminal({ true });
    auto const* cterminal = terminal.get();
    auto protocols = initialize_protocols(scene_queue, cterminal->current_scene().terminal_size());

restart:
    try {
        size_t current_protocol = 0;

        for (auto& protocol: protocols)
            protocol->run();

        bool quit = false;
        while (!quit) {
            terminal->get_events(event_queue, &quit);
            protocols.at(current_protocol)->do_events(event_queue);
            terminal->update_scene(scene_queue);
            terminal->draw();
        }

    } catch (std::exception& e) {
        bool quit = false;
        terminal->show_error(e, &quit);
        if (!quit)
            goto restart;
        exit_status = EXIT_FAILURE;
    }

    for (auto& protocol: protocols)
        protocol->finalize_threads();

    return exit_status;
}
