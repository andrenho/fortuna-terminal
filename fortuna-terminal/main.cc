#include "terminal/terminal.hh"
#include "terminal/sceneevent.hh"

int main()
{
    SyncQueue<SceneEvent> scene_queue;
    SyncQueue<FP_Message> event_queue;

    Terminal terminal;
    unsigned int scene_n = terminal.add_scene();

    FP_Message msg { FP_TEXT_PRINT_CHAR, {} };
    msg.chr = 'A';

    scene_queue.emplace({scene_n, msg});

    bool quit = false;
    while (!quit) {
        terminal.do_events(event_queue, &quit);
        terminal.update_scene(scene_queue);
        terminal.draw();
    }
}
