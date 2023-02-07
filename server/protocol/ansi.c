#include "ansi.h"

#include <string.h>

#include "../error/error.h"

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene)
{
    (void) buffer; (void) bufsz; (void) scene;
    // TODO
    return 0;
}

ssize_t ansi_terminal_event(FP_Command* command, uint8_t* buffer, size_t max_bufsz)
{
    switch (command->command) {

        case FP_EVENT_KEYSTROKE:
            if (command->var.length > max_bufsz)
                return ERR_BUF_OVERFLOW;
            memcpy(buffer, command->var.data, command->var.length);
            return command->var.length;

        default:
            break;
    }

    return 0;
}
