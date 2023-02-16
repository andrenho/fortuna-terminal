#include "terminal/terminal.hh"
#include "../fortuna-protocol/fortuna-protocol.h"

int main() {
    SyncQueue<FP_Message> event_queue;

    Terminal terminal;
    unsigned int scene_n = terminal.add_scene();

    FP_Message msg { FP_TEXT_PRINT_CHAR, {} };
    msg.chr = 'A';

    terminal.update_scene(scene_n, std::move(msg));

    bool quit = false;
    while (!quit) {
        terminal.do_events(event_queue, &quit);
        terminal.draw();
    }
}
