#include <iostream>
#include <memory>

#include "terminal/terminal.hh"
#include "exceptions/fortunaexception.hh"
#include "protocol/protocol.hh"

#define ALL_PROTOCOLS(...) { std::for_each(std::begin(protocols), std::end(protocols), [&](Protocol& p) { __VA_ARGS__; }); }

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

int main(int argc, char* argv[])
{
    std::unique_ptr<const Options> options;
    std::unique_ptr<Terminal> terminal;
    std::unique_ptr<CommunicationModule> comm;

    std::vector<Protocol> protocols;
    size_t current_protocol = 0;

    Protocol* protocol;

    try {
        options = std::make_unique<const Options>(argc, argv);
        terminal = std::make_unique<Terminal>(options->terminal_options);
        comm = CommunicationModule::create_unique(options.get());
        protocols.emplace_back(std::move(comm));

        protocol = &protocols.at(current_protocol);

        if (options->debug_comm) {
            ALL_PROTOCOLS(p.set_debug_comm(true));
        }

        terminal->resize_window(protocol->scene());

    } catch (std::exception& e) {
        on_error(terminal.get(), protocols, current_protocol, e, nullptr);
    }

    ALL_PROTOCOLS(p.run())

restart:
    try {
        bool quit = false;
        while (!quit) {
            ALL_PROTOCOLS(p.scene().text.update_blink())
            terminal->do_events(*protocol, &quit);
            terminal->draw(protocol->scene());
        }

    } catch (std::exception& e) {
        bool quit = false;
        on_error(terminal.get(), protocols, current_protocol, e, &quit);
        if (!quit)
            goto restart;
    }

    ALL_PROTOCOLS(p.finalize_threads())

    return EXIT_SUCCESS;
}
