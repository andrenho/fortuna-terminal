// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

#include "ansi.h"
#include "protocol_debug.h"
#include "tmt.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static TMT *vt_;
static Scene* scene_;

static uint8_t translate_color(tmt_color_t fg, bool bold)
{
    if (!bold) {
        switch (fg) {
            case TMT_COLOR_BLACK: return COLOR_BLACK;
            case TMT_COLOR_RED: return COLOR_RED;
            case TMT_COLOR_GREEN: return COLOR_GREEN;
            case TMT_COLOR_YELLOW: return COLOR_ORANGE;
            case TMT_COLOR_BLUE: return COLOR_DARK_BLUE;
            case TMT_COLOR_MAGENTA: return COLOR_PURPLE;
            case TMT_COLOR_CYAN: return COLOR_TURQUOISE;
            default:
                return COLOR_WHITE;
        }
    } else {
        switch (fg) {
            case TMT_COLOR_BLACK: return COLOR_GRAY;
            case TMT_COLOR_RED: return COLOR_ORANGE;
            case TMT_COLOR_GREEN: return COLOR_LIME;
            case TMT_COLOR_YELLOW: return COLOR_YELLOW;
            case TMT_COLOR_BLUE: return COLOR_LIGHT_BLUE;
            case TMT_COLOR_MAGENTA: return COLOR_BLUE;
            case TMT_COLOR_CYAN: return COLOR_CYAN;
            default:
                return COLOR_LIGHT_GRAY;
        }
    }
    return COLOR_WHITE;  // TODO
}

static void callback(tmt_msg_t m, TMT *vt, const void *a, void *p)
{
    (void) p;

    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);

    switch (m) {

        case TMT_MSG_MOVED:
            text_move_cursor_to(&scene_->text, c->r, c->c);
            tmt_clean(vt);
            break;

        case TMT_MSG_UPDATE:
            for (size_t r = 0; r < s->nline; r++) {
                if (s->lines[r]->dirty) {
                    for (size_t x = 0; x < s->ncol; x++) {
                        TMTCHAR ch = s->lines[r]->chars[x];
                        text_set_char(&scene_->text, r, x, ch.c, translate_color(ch.a.fg, ch.a.bold));
                    }
                }
            }
            tmt_clean(vt);
            break;

        case TMT_MSG_ANSWER:
            printf("terminal answered %s\n", (const char *)a);  // ???
            break;

        case TMT_MSG_BELL:
            break;

        case TMT_MSG_CURSOR:
            break;
    }
}

int ansi_init(Scene* scene)
{
    scene_ = scene;

    vt_ = tmt_open(scene->text.lines, scene->text.columns, callback, NULL, NULL);
    if (!vt_)
        return perror("could not allocate terminal"), EXIT_FAILURE;
    return 0;
}

ssize_t ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene)
{
    (void) scene;
    tmt_write(vt_, (const char *) buffer, bufsz);
    return (ssize_t) bufsz;
}

int ansi_terminal_event(FP_Command* command, Buffer* output_buffer)
{
    switch (command->command) {

        case FP_EVENT_KEYSTROKE:
            buffer_add_bytes(output_buffer, command->var.data, command->var.length);
            return 0;

        case FP_EVENT_KEY_PRESS:
            if (command->key.mod.control) {
                if (command->key.key >= 'a' && command->key.key <= 'z')
                    buffer_add_byte(output_buffer, command->key.key - 96);

            } else if (command->key.key < 32 || command->key.key >= 127) {
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
                    case SK_F9:         return buffer_add_str_nonull(output_buffer, TMT_KEY_F9);
                    case SK_F10:        return buffer_add_str_nonull(output_buffer, TMT_KEY_F10);
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
    tmt_close(vt_);
    return 0;
}
