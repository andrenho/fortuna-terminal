#include "ansi.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../error/error.h"

#define ESCAPE_SEQ_SZ 24

static bool ansi_parse_escape_sequence(char* buf, size_t idx)
{
    fprintf(stderr, "Invalid escape sequence: ^%.*s\n", (int) (idx - 1), &buf[1]);
    return false;
}

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene)
{
    size_t bytes_consumed = 0;

    bool    escape_sequence = false;
    char    escape_seq_buf[ESCAPE_SEQ_SZ] = {0};
    size_t  escape_seq_idx = 0;

    for (size_t i = 0; i < bufsz; ++i) {
        uint8_t c = buffer[i];
        if (!escape_sequence) {
            if (c == '\e')                              // escape sequence starts
                escape_sequence = true;
            else                                        // just a regular character
                text_add_char(&scene->text, c);
        }
        if (escape_sequence) {
            escape_seq_buf[escape_seq_idx++] = (char) c;
            if (!isalpha(c) && c != '@') {           // escape sequence continues
                if (escape_seq_idx >= ESCAPE_SEQ_SZ) {  // invalid escape sequence, rollback
                    for (size_t j = 0; j < escape_seq_idx; ++j)
                        text_add_char(&scene->text, escape_seq_buf[j]);
                    escape_sequence = false;
                    escape_seq_idx = 0;
                }
            } else {                                    // escape sequence ends
                if (!ansi_parse_escape_sequence(escape_seq_buf, escape_seq_idx)) {
                    for (size_t j = 0; j < escape_seq_idx; ++j)
                        text_add_char(&scene->text, escape_seq_buf[j]);
                }
                escape_sequence = false;
                escape_seq_idx = 0;
            }
        }
        ++bytes_consumed;
    }

    // check for an incomplete escape sequence
    if (escape_sequence) {
        bytes_consumed -= escape_seq_idx;
    }

    return (ssize_t) bytes_consumed;
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

