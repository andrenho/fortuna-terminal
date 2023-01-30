#include "terminal.hh"

#include <iostream>

#include "sdl_terminal.hh"

std::unique_ptr<Terminal> Terminal::make_terminal(const Options &options, CommunicationModule& comm)
{
    switch (options.terminal_type) {
        case TerminalType::SDL:
            return std::make_unique<SDL_Terminal>(comm, options.window_mode);
        default:
            std::cerr << "Unsupported terminal type.\n";
            exit(EXIT_FAILURE);
    }

}
