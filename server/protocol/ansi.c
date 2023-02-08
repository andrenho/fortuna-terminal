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

ssize_t seq(uint8_t* buffer, const char* sequence)
{
    if (sequence[1] == '\0') {
        buffer[0] = sequence[0];
        return 1;
    } else {
        strcpy((char *) buffer, sequence);
        return (ssize_t) strlen(sequence);
    }
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
                    case SK_ESC:        return seq(buffer, "\e");
                    case SK_ENTER:      return seq(buffer, "\r");
                    case SK_TAB:        return seq(buffer, "\t");
                    case SK_BACKSPACE:  return seq(buffer, "\b");
                    case SK_F1:         return seq(buffer, "\e[[A");
                    case SK_F2:
                        break;
                    case SK_F3:
                        break;
                    case SK_F4:
                        break;
                    case SK_F5:
                        break;
                    case SK_F6:
                        break;
                    case SK_F7:
                        break;
                    case SK_F8:
                        break;
                    case SK_F9:
                        break;
                    case SK_F10:
                        break;
                    case SK_F11:
                        break;
                    case SK_F12:
                        break;
                    case SK_CAPSLOCK:
                        break;
                    case SK_WIN:
                        break;
                    case SK_INSERT:
                        break;
                    case SK_HOME:
                        break;
                    case SK_END:
                        break;
                    case SK_PAGEUP:
                        break;
                    case SK_PAGEDOWN:
                        break;
                    case SK_UP:
                        break;
                    case SK_DOWN:
                        break;
                    case SK_LEFT:
                        break;
                    case SK_RIGHT:
                        break;
                    case SK_DELETE:
                        break;
                    case SK_PRINTSCREEN:
                        break;
                    case SK_PAUSEBREAK:
                        break;
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
