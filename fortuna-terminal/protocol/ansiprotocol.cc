#include "ansiprotocol.hh"
#include "exceptions/fortunaexception.hh"

#include <thread>

AnsiProtocol::AnsiProtocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, int scene_n,
                           Size initial_size)
        : Protocol(std::move(comm), scene_queue, scene_n),
          vt_(decltype(vt_)(
                  tmt_open(initial_size.w, initial_size.h, AnsiProtocol::tmt_callback, this, nullptr),
                  [](TMT* vt) { tmt_close(vt); }
          ))
{
    if (!vt_)
        throw FortunaException("Could not allocate terminal");
}

void AnsiProtocol::run()
{
}

void AnsiProtocol::do_events(SyncQueue<FP_Message> &event_queue)
{

}

void AnsiProtocol::tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p)
{
    /*
    AnsiProtocol* ansi_protocol = reinterpret_cast<AnsiProtocol*>(p);

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
    */
}

