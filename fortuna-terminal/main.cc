#include <iostream>
#include <memory>

#include "terminal/terminal.hh"
#include "terminal/scene/sceneevent.hh"
#include "exceptions/fortunaexception.hh"
#include "comm/echo.hh"
#include "comm/pty.hh"
#include "comm/tcpip.hh"
#include "protocol/ansiprotocol.hh"

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

static std::vector<AnsiProtocol> initialize_protocols(Terminal* terminal, SyncQueue<SceneEvent> &scene_queue, Options const* options)
{
    Text const& text = ((const Terminal *) terminal)->current_scene().text;

    std::vector<AnsiProtocol> protocols;
    try {

        Size size = { text.columns(), text.lines() };
        auto comm = CommunicationModule::create_unique(options, size);
        protocols.emplace_back(std::move(comm), scene_queue, 0, size);
        return protocols;

    } catch (std::exception& e) {
        terminal->show_error(e, nullptr);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    std::unique_ptr<const Options> options;
    try {
        options = std::make_unique<const Options>(argc, argv);
    } catch (std::exception& e) {
        std::cerr << "\e[1;31m" << e.what() << "\e0m\n";
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;

    SyncQueue<SceneEvent> scene_queue;
    SyncQueue<FP_Message> event_queue;

    auto terminal = initialize_terminal({ options->window_mode });
    auto protocols = initialize_protocols(terminal.get(), scene_queue, options.get());

restart:
    try {
        size_t current_protocol = 0;

        for (auto& protocol: protocols)
            protocol.run();

        bool quit = false;
        while (!quit) {
            terminal->get_events(event_queue, &quit);
            protocols.at(current_protocol).do_events(event_queue);
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
        protocol.finalize_threads();

    std::cout << "Total scene events processed: " << terminal->total_scene_events() << std::endl;

    return exit_status;
}
