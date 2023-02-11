#include "ansi.h"
#include "protocol_debug.h"
#include "tmt.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static TMT *vt_;
static Scene* scene_;

static uint8_t translate_color(tmt_color_t fg)
{
    return COLOR_WHITE;  // TODO
}

static void callback(tmt_msg_t m, TMT *vt, const void *a, void *p)
{
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
                        text_set_char(&scene_->text, r, x, s->lines[r]->chars[x].c, translate_color(s->lines[r]->chars[x].a.fg));
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
            switch (command->key.special_key) {
                case SK_ENTER:
                    buffer_add_byte(output_buffer, '\r');
                    break;
                case SK_ESC: return buffer_add_str_nonull(output_buffer, TMT_KEY_ESCAPE);
                case SK_F1: return buffer_add_str_nonull(output_buffer, TMT_KEY_F1);
                case SK_F2: return buffer_add_str_nonull(output_buffer, TMT_KEY_F2);
                case SK_F3: return buffer_add_str_nonull(output_buffer, TMT_KEY_F3);
                case SK_F4: return buffer_add_str_nonull(output_buffer, TMT_KEY_F4);
                case SK_F5: return buffer_add_str_nonull(output_buffer, TMT_KEY_F5);
                case SK_F6: return buffer_add_str_nonull(output_buffer, TMT_KEY_F6);
                case SK_F7: return buffer_add_str_nonull(output_buffer, TMT_KEY_F7);
                case SK_F8: return buffer_add_str_nonull(output_buffer, TMT_KEY_F8);
                case SK_F9: return buffer_add_str_nonull(output_buffer, TMT_KEY_F9);
                case SK_F10: return buffer_add_str_nonull(output_buffer, TMT_KEY_F10);
                case SK_TAB: return buffer_add_str_nonull(output_buffer, TMT_KEY_BACK_TAB);
                case SK_INSERT: return buffer_add_str_nonull(output_buffer, TMT_KEY_INSERT);
                case SK_HOME: return buffer_add_str_nonull(output_buffer, TMT_KEY_HOME);
                case SK_END: return buffer_add_str_nonull(output_buffer, TMT_KEY_END);
                case SK_PAGEUP: return buffer_add_str_nonull(output_buffer, TMT_KEY_PAGE_UP);
                case SK_PAGEDOWN: return buffer_add_str_nonull(output_buffer, TMT_KEY_PAGE_DOWN);
                case SK_UP: return buffer_add_str_nonull(output_buffer, TMT_KEY_UP);
                case SK_DOWN: return buffer_add_str_nonull(output_buffer, TMT_KEY_DOWN);
                case SK_LEFT: return buffer_add_str_nonull(output_buffer, TMT_KEY_LEFT);
                case SK_RIGHT: return buffer_add_str_nonull(output_buffer, TMT_KEY_RIGHT);
                case SK_BACKSPACE: return buffer_add_str_nonull(output_buffer, TMT_KEY_BACKSPACE);
                default:
                    break;
            }

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
