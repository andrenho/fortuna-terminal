#include "ansi.h"
#include "protocol_debug.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define ESCAPE_SEQ_SZ 24
#define MAX_PARAMS 2
#define PARAM_SZ 24

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

int ansi_init()
{
    return 0;
}

static char ansi_parse_escape_sequence(const char* seq, char* control, int* p1, int* p2)
{
    size_t i = 1;
    char param[MAX_PARAMS][PARAM_SZ] = {0};
    size_t current_param = 0;
    size_t j = 0;

    if (seq[i] == '[' || seq[i] == '?')
        *control = seq[i++];
    if (seq[2] == '?') {
        *control = seq[2];
        ++i;
    }

    while (seq[i]) {
        if (isdigit(seq[i])) {
            if (j >= PARAM_SZ) return '\0';
            param[current_param][j++] = seq[i];
        } else if (seq[i] == ';') {
            ++current_param;
            j = 0;
        } else {
            *p1 = (int) strtoul(param[0], NULL, 10);
            *p2 = (int) strtoul(param[1], NULL, 10);
            return seq[i];
        }
        ++i;
    }

    return '\0';
}

static uint8_t text_ansi_color(int number)
{
    switch (number) {
        case 30: return COLOR_BLACK;
        case 31: return COLOR_RED;
        case 32: return COLOR_GREEN;
        case 33: return COLOR_ORANGE;
        case 34: return COLOR_DARK_BLUE;
        case 35: return COLOR_PURPLE;
        case 36: return COLOR_TURQUOISE;
        case 37: return COLOR_LIGHT_GRAY;
        case 90: return COLOR_GRAY;
        case 91: return COLOR_ORANGE;
        case 92: return COLOR_LIME;
        case 93: return COLOR_YELLOW;
        case 94: return COLOR_LIGHT_BLUE;
        case 95: return COLOR_BLUE;
        case 96: return COLOR_CYAN;
        case 97: return COLOR_WHITE;
        default: return COLOR_WHITE;
    }
}

static bool ansi_execute_escape_sequence(const char* seq, Text* text)
{
    int p1, p2;
    char control = '\0';
    char cmd = ansi_parse_escape_sequence(seq, &control, &p1, &p2);
    switch (cmd) {

        case 'A':
            debug_special_1("cursor_up", p1);
            text_move_cursor_relative(text, min(-p1, -1), 0);
            break;

        case 'B':
            debug_special_1("cursor_down", p1);
            text_move_cursor_relative(text, max(p1, 1), 0);
            break;

        case 'C':
            debug_special_1("cursor_right", p1);
            text_move_cursor_relative(text, 0, max(p1, 1));
            break;

        case 'D':
            debug_special_1("cursor-left (alt)", p1);
            text_move_cursor_relative(text, 0, min(-p1, -1));
            break;

        case 'H':
        case 'f':
            debug_special_2("cursor_home", p1, p2);
            text_move_cursor_to(text, max(p1, 1), max(p2, 1));
            break;

        case 'J':
            if (p1 == 0) {
                debug_special_0("clear_eos");
                text_clear_to_end_of_screen(text);
            } else {
                return false;
            }
            break;

        case 'K':
            if (p1 == 0) {
                debug_special_0("clear_eol");
                text_clear_to_end_of_line(text);
            } else if (p1 == 1) {
                debug_special_0("clear_bol");
                text_clear_to_beginning_of_line(text);
            } else if (p1 == 2) {
                debug_special_0("delete_line (alt)");
                text_clear_line(text);
            } else {
                return false;
            }
            break;

        case 'M':
            if (p1 == 0) {
                debug_special_0("delete_line");
                text_clear_line(text);
            } else {
                return false;
            }
            break;

        case 'P':
            if (p1 == 0) {
                debug_special_0("delete_character");
                text_delete_char_under_cursor(text);
            } else {
                return false;
            }
            break;

        case 'h':
            if (control == '?' && p1 == 2004) {
                debug_special_0("stop_paste");
                break;
            }
            if (p1 == 4) {
                debug_special_0("enter_insert_mode");
                text_set_insertion_mode(text, true);
            } else {
                return false;
            }
            break;

        case 'l':
            if (control == '?' && p1 == 2004) {
                debug_special_0("start_paste");
                break;
            }
            if (p1 == 4) {
                debug_special_0("exit_insert_mode");
                text_set_insertion_mode(text, false);
            } else {
                return false;
            }
            break;

        case 'm':
            if (p1 == 0 && p2 == 0) {
                debug_special_0("reset_formatting");
                text_reset_formatting(text);
            } else if (p2 != 0) {
                debug_special_2("set_color", p1, p2);
                text_set_color(text, text_ansi_color(p2));
            }
            break;

        case 'r':
            debug_special_2("change_scroll_region", p1, p2);
            text_set_scroll_region(text, p1, p2);
            break;

        default:
            fprintf(stderr, "Invalid escape sequence: ^%s\n", &seq[1]);
            return false;
    }
    return true;
}

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene)
{
    // TODO - break this function

    size_t bytes_consumed = 0;

    static bool    escape_sequence = false;
    static char    escape_seq_buf[ESCAPE_SEQ_SZ] = {0};
    static size_t  escape_seq_idx = 0;

    for (size_t i = 0; i < bufsz; ++i) {
        uint8_t c = buffer[i];
        if (!escape_sequence) {
            if (c == '\e')                              // escape sequence starts
                escape_sequence = true;
            else if (c == '\b')
                text_move_cursor_relative(&scene->text, 0, -1);
            else if (c == '\r')
                text_move_cursor_bol(&scene->text);
            else if (c == '\n')
                text_move_cursor_down_scroll(&scene->text);
            else                                        // just a regular character
                text_add_char(&scene->text, c);
            debug_char(c);
        }
        if (escape_sequence) {
            escape_seq_buf[escape_seq_idx++] = (char) c;
            if (!isalpha(c) && c != '@' && c != '~') {    // escape sequence continues
                if (escape_seq_idx >= ESCAPE_SEQ_SZ) {  // invalid escape sequence, rollback
                    for (size_t j = 0; j < escape_seq_idx; ++j)
                        text_add_char(&scene->text, escape_seq_buf[j]);
                    escape_sequence = false;
                    escape_seq_idx = 0;
                }
            } else {                                    // escape sequence ends
                escape_seq_buf[escape_seq_idx + 1] = '\0';
                if (!ansi_execute_escape_sequence(escape_seq_buf, &scene->text)) {
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

int ansi_terminal_event(FP_Command* command, Buffer* output_buffer)
{
    switch (command->command) {

        case FP_EVENT_KEYSTROKE:
            buffer_add_bytes(output_buffer, command->var.data, command->var.length);
            return 0;

        case FP_EVENT_KEY_PRESS:
            if (command->key.key < 32 || command->key.key >= 127) {
                switch (command->key.special_key) {
                    case SK_ESC:        return buffer_add_str_nonull(output_buffer, "\e");
                    case SK_ENTER:      return buffer_add_str_nonull(output_buffer, "\r");
                    case SK_TAB:        return buffer_add_str_nonull(output_buffer, "\t");
                    case SK_BACKSPACE:  return buffer_add_str_nonull(output_buffer, "\b");
                    case SK_F1:         return buffer_add_str_nonull(output_buffer, "\e[[A");
                    case SK_F2:         return buffer_add_str_nonull(output_buffer, "\e[[B");
                    case SK_F3:         return buffer_add_str_nonull(output_buffer, "\e[[C");
                    case SK_F4:         return buffer_add_str_nonull(output_buffer, "\e[[D");
                    case SK_F5:         return buffer_add_str_nonull(output_buffer, "\e[[E");
                    case SK_F6:         return buffer_add_str_nonull(output_buffer, "\e[17~");
                    case SK_F7:         return buffer_add_str_nonull(output_buffer, "\e[18~");
                    case SK_F8:         return buffer_add_str_nonull(output_buffer, "\e[19~");
                    case SK_F9:         return buffer_add_str_nonull(output_buffer, "\e[20~");
                    case SK_F10:        return buffer_add_str_nonull(output_buffer, "\e[21~");
                    case SK_F11:        return buffer_add_str_nonull(output_buffer, "\e[23~");
                    case SK_F12:        return buffer_add_str_nonull(output_buffer, "\e[24~");
                    case SK_INSERT:     return buffer_add_str_nonull(output_buffer, "\e[2~");
                    case SK_HOME:       return buffer_add_str_nonull(output_buffer, "\e[1~");
                    case SK_END:        return buffer_add_str_nonull(output_buffer, "\e[4~");
                    case SK_PAGEUP:     return buffer_add_str_nonull(output_buffer, "\e[5~");
                    case SK_PAGEDOWN:   return buffer_add_str_nonull(output_buffer, "\e[6~");
                    case SK_UP:         return buffer_add_str_nonull(output_buffer, "\e[A");
                    case SK_DOWN:       return buffer_add_str_nonull(output_buffer, "\e[B");
                    case SK_LEFT:       return buffer_add_str_nonull(output_buffer, "\e[D");
                    case SK_RIGHT:      return buffer_add_str_nonull(output_buffer, "\e[C");
                    case SK_DELETE:     return buffer_add_str_nonull(output_buffer, "\e[3~");
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

int ansi_finalize()
{
    return 0;
}
