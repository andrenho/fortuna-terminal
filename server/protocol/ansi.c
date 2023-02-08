#include "ansi.h"

#include <string.h>

#include "../error/error.h"

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene)
{
    for (size_t i = 0; i < bufsz; ++i)
       text_add_char(&scene->text, buffer[i]);
    return (ssize_t) bufsz;
}

ssize_t ansi_terminal_event(FP_Command* command, uint8_t* buffer, size_t max_bufsz)
{
    switch (command->command) {

        case FP_EVENT_KEYSTROKE:
            if (command->var.length > max_bufsz)
                return ERR_BUF_OVERFLOW;
            memcpy(buffer, command->var.data, command->var.length);
            return command->var.length;

        case FP_EVENT_KEY_PRESS:
            if (command->key.key < 32 || command->key.key >= 127) {
                buffer[0] = command->key.key;
                return 1;
            }
            break;

        default:
            break;
    }

    return 0;
}
