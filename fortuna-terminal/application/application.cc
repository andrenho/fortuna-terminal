#include <iostream>
#include "application.hh"

#define ALL_PROTOCOLS(...) { std::for_each(std::begin(protocols), std::end(protocols), [&](Protocol& p) { __VA_ARGS__; }); }

Application::Application(int argc, char **argv)
    : options(argc, argv),
      terminal(options.terminal_options),
      comm(CommunicationModule::create_unique(options))
{
    protocols.emplace_back(options.mode, *comm, gpio);
    current_protocol = &protocols.at(0);

    if (options.debug_comm)
        ALL_PROTOCOLS(p.set_debug_comm(true));

    terminal.setup_scene(current_protocol->scene());

    ALL_PROTOCOLS(p.run())
}

void Application::run()
{
    try {
        gpio.reset();

    } catch (std::exception& e) {
        bool quit = false;
        on_error(e, &quit);
        run();
    }
}

void Application::on_error(std::exception &e, bool *quit)
{
    std::cerr << "\e[1;31m" << e.what() << "\e0m\n";
    ALL_PROTOCOLS(p.show_error(e))
    terminal.draw(current_protocol->scene());
    terminal.wait_for_enter(quit);
}
