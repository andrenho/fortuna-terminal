#include "ansi.h"

#include <stdio.h>
#include <string.h>

#include "../error/error.h"

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene)
{
    for (size_t i = 0; i < bufsz; ++i) {
        uint8_t c = buffer[i];
        if (c >= 32 && c < 127)
            text_add_char(&scene->text, c);
    }
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
                switch (command->key.special_key) {
                    case SK_ESC:
                        buffer[0] = '\e';
                        return 1;
                    case SK_ENTER:
                        buffer[0] = 10;
                        return 1;
                    case SK_TAB:
                        buffer[0] = '\t';
                        return 1;
                    case SK_BACKSPACE:
                        buffer[0] = '\b';
                        return 1;
                    default:
                        fprintf(stderr, "Currently unsupported key pressed: %d\n", command->key.special_key);
                }
            }
            break;

        default:
            break;
    }

    return 0;
}
