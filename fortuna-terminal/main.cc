#include <iostream>
#include <memory>

#include "terminal/terminal.hh"
#include "exceptions/fortunaexception.hh"
#include "protocol/ansiprotocol.hh"

static std::unique_ptr<Terminal> initialize_terminal(TerminalOptions terminal_options)
{
    std::unique_ptr<Terminal> terminal;

    try {
        terminal = std::make_unique<Terminal>(terminal_options);
    } catch (std::exception& e) {
        std::cerr << "\e[1;31m" << e.what() << "\e[0m\n";
        exit(EXIT_FAILURE);
    }

    return terminal;
}

static std::vector<AnsiProtocol> initialize_protocols(Terminal& terminal, Options const* options)
{
    std::vector<AnsiProtocol> protocols;
    try {

        auto comm = CommunicationModule::create_unique(options);
        protocols.emplace_back(std::move(comm));
        return protocols;

    } catch (std::exception& e) {
        terminal.show_error(e, nullptr);
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

    auto terminal = initialize_terminal({ options->window_mode });
    auto protocols = initialize_protocols(*terminal, options.get());

    AnsiProtocol& protocol = protocols.at(0);

    terminal->resize_window(protocol.scene());

restart:
    try {
        for (auto& p: protocols)
            p.run();

        bool quit = false;
        while (!quit) {
            for (auto& p: protocols)
                p.scene().text.update_blink();
            terminal->do_events(protocol, &quit);
            terminal->draw(protocol.scene());
        }

    } catch (std::exception& e) {
        bool quit = false;
        terminal->show_error(e, &quit);
        if (!quit)
            goto restart;
        exit_status = EXIT_FAILURE;
    }

    for (auto& p: protocols)
        p.finalize_threads();

    return exit_status;
}
