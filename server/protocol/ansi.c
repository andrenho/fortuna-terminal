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
                    case SK_F2:         return seq(buffer, "\e[[B");
                    case SK_F3:         return seq(buffer, "\e[[C");
                    case SK_F4:         return seq(buffer, "\e[[D");
                    case SK_F5:         return seq(buffer, "\e[[E");
                    case SK_F6:         return seq(buffer, "\e[17~");
                    case SK_F7:         return seq(buffer, "\e[18~");
                    case SK_F8:         return seq(buffer, "\e[19~");
                    case SK_F9:         return seq(buffer, "\e[20~");
                    case SK_F10:        return seq(buffer, "\e[21~");
                    case SK_F11:        return seq(buffer, "\e[23~");
                    case SK_F12:        return seq(buffer, "\e[24~");
                    case SK_INSERT:     return seq(buffer, "\e[2~");
                    case SK_HOME:       return seq(buffer, "\e[1~");
                    case SK_END:        return seq(buffer, "\e[4~");
                    case SK_PAGEUP:     return seq(buffer, "\e[5~");
                    case SK_PAGEDOWN:   return seq(buffer, "\e[6~");
                    case SK_UP:         return seq(buffer, "\e[A");
                    case SK_DOWN:       return seq(buffer, "\e[B");
                    case SK_LEFT:       return seq(buffer, "\e[D");
                    case SK_RIGHT:      return seq(buffer, "\e[C");
                    case SK_DELETE:     return seq(buffer, "\e[3~");
                    case SK_PRINTSCREEN:
                    case SK_PAUSEBREAK:
                    case SK_CAPSLOCK:
                    case SK_WIN:
                        break;
                }
            }
            break;

        default:
            break;
    }

    return 0;
}
