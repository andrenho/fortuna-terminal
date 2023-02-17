#include <iostream>
#include <memory>

#include "terminal/terminal.hh"
#include "terminal/scene/sceneevent.hh"
#include "exceptions/fortunaexception.hh"

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

int main(int argc, char* argv[])
{
    (void) argc; (void) argv;

    auto terminal = initialize_terminal({ true });

restart:
    try {
        SyncQueue<SceneEvent> scene_queue;
        SyncQueue<FP_Message> event_queue;

        throw FortunaException("This is an error");

        bool quit = false;
        while (!quit) {
            terminal->do_events(event_queue, &quit);
            terminal->update_scene(scene_queue);
            terminal->draw();
        }

    } catch (std::exception& e) {
        bool quit = false;
        terminal->show_error(e, &quit);
        if (!quit)
            goto restart;
    }

    return 0;
}
