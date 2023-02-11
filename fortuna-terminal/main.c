#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "options/options.h"
#include "error/error.h"
#include "protocol/protocol.h"
#include "comm/comm.h"
#include "ui/ui.h"
#include "scene/scene.h"
#include "buffer/buffer.h"

#define BUFFER_SZ (32 * 1024)

int main(int argc, char* argv[])
{
    // parse options
    error_check(options_parse_cmdline(argc, argv, &options));

    // create buffers
    Buffer input_buffer, output_buffer;
    buffer_init(&input_buffer, BUFFER_SZ);
    buffer_init(&output_buffer, BUFFER_SZ);

    // create scene
    Scene scene;
    scene_init(&scene);

    // intialization
    protocol_init(&output_buffer, &scene);
    error_check(comm_init(scene.text.lines, scene.text.columns));
    error_check(ui_init());

    // start communication thread(s)
    comm_run_input(&input_buffer);
    comm_run_output(&output_buffer);

    // main thread loop
    while (ui_running()) {

        Uint64 start_frame = SDL_GetTicks64();

        protocol_process_input(&input_buffer, &scene);

        ui_do_events();
        ui_draw(&scene);

        Uint64 end_frame = SDL_GetTicks64();
        if (end_frame < (start_frame + 16))
            nanosleep(&(struct timespec) { .tv_sec = 0, .tv_nsec = (long) (end_frame - (start_frame + 16)) * 1000000 }, NULL);

        comm_notify_vsync();
    }

    // clean up
    comm_finalize();
    protocol_finalize();
    buffer_finalize(&input_buffer);
    buffer_finalize(&output_buffer);
    ui_destroy();

    return EXIT_SUCCESS;
}

