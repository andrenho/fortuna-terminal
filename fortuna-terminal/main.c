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
#include "gpio/gpio.h"

#define BUFFER_SZ (32 * 1024)

static Scene scene;
Buffer input_buffer, output_buffer;

static void initialize()
{
    // create buffers
    buffer_init(&input_buffer, BUFFER_SZ);
    buffer_init(&output_buffer, BUFFER_SZ);

    // intialization
    E_UI(protocol_init(&output_buffer, &scene), "Error initializing protocol");
    E_UI(comm_init(scene.text.lines, scene.text.columns), "Error initializing communication module");
    gpio_init();

    // start communication thread(s)
    comm_run_input(&input_buffer);
    comm_run_output(&output_buffer);
}

static void single_loop()
{
    Uint64 start_frame = SDL_GetTicks64();

    protocol_process_input(&input_buffer, &scene);

    ui_do_events();
    ui_draw(&scene);

    Uint64 end_frame = SDL_GetTicks64();
    if (end_frame < (start_frame + 16))
        nanosleep(&(struct timespec) { .tv_sec = 0, .tv_nsec = (long) (end_frame - (start_frame + 16)) * 1000000 }, NULL);

    gpio_notify_vsync();
}

static void finalize()
{
    gpio_finalize();
    comm_finalize();
    protocol_finalize();
    buffer_finalize(&input_buffer);
    buffer_finalize(&output_buffer);
}

int main(int argc, char* argv[])
{
    E_STDERR_ABORT(options_parse_cmdline(argc, argv, &options), "Command line option error");
    scene_init(&scene);
    E_STDERR_ABORT(ui_init(), "Error initializing the UI");

    initialize();
    gpio_reset();

    while (ui_running()) {
        single_loop();
    }

    finalize();
    ui_destroy();
    return EXIT_SUCCESS;
}

