#include "comm.h"
#include "error/error.h"
#include "echo.h"

typedef struct {
    int (*run)();
    int (*stop)();
} CommFunctions;
static CommFunctions comm_f = { NULL, NULL };

int comm_init(Options* options)
{
    switch (options->comm_mode) {
        case CM_ECHO:
            comm_f = (CommFunctions) { echo_run, echo_stop };
            return echo_init();
        default:
            return ERR_NOT_IMPLEMENTED;
    }
}

int comm_run()
{
    return comm_f.run();
}

int comm_notify_vsync()
{
    // TODO
    return 0;
}

int comm_stop()
{
    return comm_f.stop();
}
