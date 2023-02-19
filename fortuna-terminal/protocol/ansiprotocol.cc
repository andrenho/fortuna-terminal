#include "ansiprotocol.hh"
#include "exceptions/fortunaexception.hh"
#include "common/color.hh"

#include <cstring>
#include <csignal>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

AnsiProtocol::AnsiProtocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, unsigned int scene_n,
                           Size initial_size)
        : Protocol(std::move(comm), scene_queue, scene_n),
          cache_(AnsiProtocol::initialize_cache(initial_size)),
          vt_(decltype(vt_)(
                  tmt_open(initial_size.h, initial_size.w, AnsiProtocol::tmt_callback, this, nullptr),
                  [](TMT* vt) { tmt_close(vt); }
          ))
{
    if (!vt_)
        throw FortunaException("Could not allocate terminal");
}

void AnsiProtocol::run()
{
    read_thread_ = std::make_unique<std::thread>([this]() {
        while (threads_active_) {
            auto byte = comm_->read_blocking();
            if (byte.has_value())
                input_queue_.push(byte.value());
        }
    });

    input_thread_ = std::make_unique<std::thread>([this]() {
        while (threads_active_) {
            std::vector<uint8_t> received_bytes;
            input_queue_.pop_all_into(received_bytes);
            if (!received_bytes.empty())
                tmt_write(vt_.get(), (const char *) received_bytes.data(), received_bytes.size());
        }
    });
}

void AnsiProtocol::do_events(SyncQueue<FP_Message> &event_queue)
{
    while (true) {
        auto omsg = event_queue.pop_nonblock();
        if (!omsg.has_value())
            break;
        auto message = *omsg;

        switch (message.command) {

            case FP_EVENT_KEYSTROKE:
                comm_->write(message.keystroke);
                break;

            case FP_EVENT_KEY_PRESS:
                if (message.key.mod.control) {
                    if (message.key.key >= 'a' && message.key.key <= 'z')
                        comm_->write(message.key.key - 96);

                } else if (message.key.key < 32 || message.key.key >= 127) {
                    switch (message.key.special_key) {
                        case SK_ESC:        comm_->write("\e"); break;
                        case SK_ENTER:      comm_->write("\r"); break;
                        case SK_TAB:        comm_->write("\t"); break;
                        case SK_BACKSPACE:  comm_->write("\b"); break;
                        case SK_F1:         comm_->write("\e[11~"); break;
                        case SK_F2:         comm_->write("\e[12~"); break;
                        case SK_F3:         comm_->write("\e[13~"); break;
                        case SK_F4:         comm_->write("\e[14~"); break;
                        case SK_F5:         comm_->write("\e[15~"); break;
                        case SK_F6:         comm_->write("\e[17~"); break;
                        case SK_F7:         comm_->write("\e[18~"); break;
                        case SK_F8:         comm_->write("\e[19~"); break;
                        case SK_F9:         comm_->write("\e[20~"); break;
                        case SK_F10:        comm_->write("\e[21~"); break;
                        case SK_F11:        comm_->write("\e[23~"); break;
                        case SK_F12:        comm_->write("\e[24~"); break;
                        case SK_INSERT:     comm_->write("\e[2~"); break;
                        case SK_HOME:       comm_->write("\e[1~"); break;
                        case SK_END:        comm_->write("\e[4~"); break;
                        case SK_PAGEUP:     comm_->write("\e[5~"); break;
                        case SK_PAGEDOWN:   comm_->write("\e[6~"); break;
                        case SK_UP:         comm_->write("\e[A"); break;
                        case SK_DOWN:       comm_->write("\e[B"); break;
                        case SK_LEFT:       comm_->write("\e[D"); break;
                        case SK_RIGHT:      comm_->write("\e[C"); break;
                        case SK_DELETE:     comm_->write("\e[3~"); break;
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
    }
}

void AnsiProtocol::tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p)
{
    auto this_ = reinterpret_cast<AnsiProtocol*>(p);

    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);

    switch (m) {

        case TMT_MSG_MOVED: {
                FP_Message msg = { FP_TEXT_SET_POS, {} };
                msg.set_pos = { (uint8_t) c->r, (uint8_t) c->c };
                this_->scene_queue_.emplace({ this_->scene_n_, msg });
                tmt_clean(vt);
            }
            break;

        case TMT_MSG_UPDATE: {
            for (size_t r = 0; r < s->nline; r++) {
                    if (s->lines[r]->dirty) {
                        for (size_t x = 0; x < s->ncol; x++) {
                            TMTCHAR ch = s->lines[r]->chars[x];
                            TMTCHAR cached_ch = this_->cache_.at(r).at(x);
                            if (memcmp(&ch, &cached_ch, sizeof(TMTCHAR)) != 0) {
                                FP_Message msg = { FP_TEXT_SET_CHAR, {} };
                                msg.set_char = { (uint8_t) ch.c, (uint8_t) r, (uint8_t) x, translate_attrib(ch.a) };
                                this_->scene_queue_.emplace({ this_->scene_n_, msg });
                                this_->cache_.at(r).at(x) = ch;
                            }
                        }
                    }
                }
            }
            tmt_clean(vt);
            break;

        case TMT_MSG_ANSWER:
            std::cout << "terminal answered " << (const char *) a << "\n";  // ???
            break;

        case TMT_MSG_BELL:
            // TODO
            break;

        case TMT_MSG_CURSOR:
            break;
    }
}

CharAttrib AnsiProtocol::translate_attrib(TMTATTRS a)
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

void AnsiProtocol::finalize_threads()
{
    threads_active_ = false;
    if (comm_->release_locks()) {
        read_thread_->join();
    } else {
        pthread_kill(read_thread_->native_handle(), 9);
        read_thread_->detach();
    }
    input_queue_.push({});  // release the lock
    input_thread_->join();
}

std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> AnsiProtocol::initialize_cache(Size initial_size)
{
    std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> k;
    for (size_t y = 0; y < initial_size.h; ++y) {
        k[y] = {};
        for (size_t x = 0; x < initial_size.w; ++x) {
            k[y][x] = TMTCHAR { ' ', { false, false, false, false, false, false, tmt_color_t::TMT_COLOR_DEFAULT, tmt_color_t::TMT_COLOR_DEFAULT } };
        }
    }
    return k;
}

