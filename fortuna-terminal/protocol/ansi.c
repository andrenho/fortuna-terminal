// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

#include "ansi.h"
#include "tmt.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static TMT *vt_;
static Scene* scene_;

static CharAttrib translate_color(TMTATTRS a)
{
    CharAttrib attr;

    if (!a.bold) {
        switch (a.fg) {
            case TMT_COLOR_BLACK: attr.color = COLOR_BLACK; break;
            case TMT_COLOR_RED: attr.color = COLOR_RED; break;
            case TMT_COLOR_GREEN: attr.color = COLOR_GREEN; break;
            case TMT_COLOR_YELLOW: attr.color = COLOR_ORANGE; break;
            case TMT_COLOR_BLUE: attr.color = COLOR_DARK_BLUE; break;
            case TMT_COLOR_MAGENTA: attr.color = COLOR_PURPLE; break;
            case TMT_COLOR_CYAN: attr.color = COLOR_TURQUOISE; break;
            default:
                attr.color = COLOR_WHITE;
                break;
        }
    } else {
        switch (a.fg) {
            case TMT_COLOR_BLACK: attr.color = COLOR_GRAY; break;
            case TMT_COLOR_RED: attr.color = COLOR_ORANGE; break;
            case TMT_COLOR_GREEN: attr.color = COLOR_LIME; break;
            case TMT_COLOR_YELLOW: attr.color = COLOR_YELLOW; break;
            case TMT_COLOR_BLUE: attr.color = COLOR_LIGHT_BLUE; break;
            case TMT_COLOR_MAGENTA: attr.color = COLOR_BLUE; break;
            case TMT_COLOR_CYAN: attr.color = COLOR_CYAN; break;
            default:
                attr.color = COLOR_LIGHT_GRAY;
                break;
        }
    }

    attr.reverse = a.reverse;
    return attr;
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
                        text_set_char(&scene_->text, r, x, ch.c, translate_color(ch.a));
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

FT_Result ansi_init(Scene* scene)
{
    scene_ = scene;

    vt_ = tmt_open(scene->text.lines, scene->text.columns, callback, NULL, NULL);
    if (!vt_)
        return perror("could not allocate terminal"), EXIT_FAILURE;
    return FT_OK;
}

FT_Result ansi_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene, Buffer* output_buffer, size_t* bytes_processed)
{
    (void) scene; (void) output_buffer;
    tmt_write(vt_, (const char *) buffer, bufsz);
    *bytes_processed = bufsz;
    return FT_OK;
}

FT_Result ansi_terminal_event(FP_Message* message, Buffer* output_buffer)
{
    switch (message->command) {

        case FP_EVENT_KEYSTROKE:
            buffer_add_bytes(output_buffer, message->keystroke.data, message->keystroke.length);
            return 0;

        case FP_EVENT_KEY_PRESS:
            if (message->key.mod.control) {
                if (message->key.key >= 'a' && message->key.key <= 'z')
                    buffer_add_byte(output_buffer, message->key.key - 96);

            } else if (message->key.key < 32 || message->key.key >= 127) {
                switch (message->key.special_key) {
                    case SK_ESC:        return buffer_add_str_nonull(output_buffer, "\e");
                    case SK_ENTER:      return buffer_add_str_nonull(output_buffer, "\r");
                    case SK_TAB:        return buffer_add_str_nonull(output_buffer, "\t");
                    case SK_BACKSPACE:  return buffer_add_str_nonull(output_buffer, "\b");
                    case SK_F1:         return buffer_add_str_nonull(output_buffer, "\e[11~");
                    case SK_F2:         return buffer_add_str_nonull(output_buffer, "\e[12~");
                    case SK_F3:         return buffer_add_str_nonull(output_buffer, "\e[13~");
                    case SK_F4:         return buffer_add_str_nonull(output_buffer, "\e[14~");
                    case SK_F5:         return buffer_add_str_nonull(output_buffer, "\e[15~");
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

    return FT_OK;
}

void ansi_finalize()
{
    tmt_close(vt_);
}
