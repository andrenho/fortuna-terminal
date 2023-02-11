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
    return 0;
}

int ansi_terminal_event(FP_Command* command, Buffer* output_buffer)
{
    return 0;
}

int ansi_finalize()
{
    tmt_close(vt_);
    return 0;
}
