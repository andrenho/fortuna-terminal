#include "terminal.hh"

#include <iostream>

#include "global.hh"
#include "sdl/sdl_terminal.hh"

std::unique_ptr<Terminal> Terminal::make_terminal()
{
    switch (options.terminal_type) {
        case TerminalType::SDL:
            return std::make_unique<SDL_Terminal>(options.window_mode);
        default:
            std::cerr << "Unsupported terminal type.\n";
            exit(EXIT_FAILURE);
    }

}
