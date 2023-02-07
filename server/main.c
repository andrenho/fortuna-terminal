#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "options/options.h"
#include "error/error.h"
#include "protocol/protocol.h"
#include "comm/comm.h"
#include "ui/ui.h"
#include "scene/scene.h"

int main(int argc, char* argv[])
{
    // parse options
    Options options;
    error_check(options_parse_cmdline(argc, argv, &options));

    // intialization
    protocol_init(&options);
    error_check(comm_init(&options));
    error_check(ui_init(&options));

    // create scene
    Scene scene;
    scene_init(&scene);

    // start communication thread(s)
    comm_run();

    // main thread loop
    while (ui_running()) {

        Uint64 start_frame = SDL_GetTicks64();

        // scene_process_input_queue();

        ui_do_events();
        ui_draw(&scene);

        Uint64 end_frame = SDL_GetTicks64();
        if (end_frame < (start_frame + 16))
            usleep((end_frame - (start_frame + 16)) * 1000);

        comm_notify_vsync();
    }

    // clean up
    comm_stop();
    ui_destroy();

    return EXIT_SUCCESS;
}

