#include "terminal.hh"

#include <iostream>

#include "sdl/sdl_terminal.hh"

std::unique_ptr<Terminal> Terminal::make_terminal(const Options &options, Scene const& scene, OutputQueue& output_queue)
{
    switch (options.terminal_type) {
        case TerminalType::SDL:
            return std::make_unique<SDL_Terminal>(scene, output_queue, options.window_mode);
        default:
            std::cerr << "Unsupported terminal type.\n";
            exit(EXIT_FAILURE);
    }

}
