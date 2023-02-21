#include "ansiprotocol.hh"

#include <cstring>
#include <csignal>
#include <iostream>
#include <thread>

#include "exceptions/fortunaexception.hh"
#include "common/color.hh"

using namespace std::chrono_literals;

AnsiProtocol::AnsiProtocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent>& scene_queue,
                           unsigned int scene_n, Size const& initial_size)
        : comm_(std::move(comm)),
          scene_queue_(scene_queue),
          scene_n_(scene_n),
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
                input_queue_->push(byte.value());
        }
    });

    input_thread_ = std::make_unique<std::thread>([this]() {
        while (threads_active_) {
            std::vector<uint8_t> received_bytes;
            input_queue_->pop_all_into(received_bytes);
            if (!received_bytes.empty())
                tmt_write(vt_.get(), (const char *) received_bytes.data(), received_bytes.size());
        }
    });

    output_thread_ = std::make_unique<std::thread>([this]() {
        while (threads_active_) {
            std::vector<uint8_t> bytes_to_output;
            output_queue_->pop_all_into(bytes_to_output);
            comm_->write(bytes_to_output);
        }
    });
}

/*
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
                        case SpecialKey::ESC:        comm_->write("\e"); break;
                        case SpecialKey::ENTER:      comm_->write("\r"); break;
                        case SpecialKey::TAB:        comm_->write("\t"); break;
                        case SpecialKey::BACKSPACE:  comm_->write("\b"); break;
                        case SpecialKey::F1:         comm_->write("\e[11~"); break;
                        case SpecialKey::F2:         comm_->write("\e[12~"); break;
                        case SpecialKey::F3:         comm_->write("\e[13~"); break;
                        case SpecialKey::F4:         comm_->write("\e[14~"); break;
                        case SpecialKey::F5:         comm_->write("\e[15~"); break;
                        case SpecialKey::F6:         comm_->write("\e[17~"); break;
                        case SpecialKey::F7:         comm_->write("\e[18~"); break;
                        case SpecialKey::F8:         comm_->write("\e[19~"); break;
                        case SpecialKey::F9:         comm_->write("\e[20~"); break;
                        case SpecialKey::F10:        comm_->write("\e[21~"); break;
                        case SpecialKey::F11:        comm_->write("\e[23~"); break;
                        case SpecialKey::F12:        comm_->write("\e[24~"); break;
                        case SpecialKey::INSERT:     comm_->write("\e[2~"); break;
                        case SpecialKey::HOME:       comm_->write("\e[1~"); break;
                        case SpecialKey::END:        comm_->write("\e[4~"); break;
                        case SpecialKey::PAGEUP:     comm_->write("\e[5~"); break;
                        case SpecialKey::PAGEDOWN:   comm_->write("\e[6~"); break;
                        case SpecialKey::UP:         comm_->write("\e[A"); break;
                        case SpecialKey::DOWN:       comm_->write("\e[B"); break;
                        case SpecialKey::LEFT:       comm_->write("\e[D"); break;
                        case SpecialKey::RIGHT:      comm_->write("\e[C"); break;
                        case SpecialKey::DELETE:     comm_->write("\e[3~"); break;
                        case SpecialKey::PRINTSCREEN:
                        case SpecialKey::PAUSEBREAK:
                        case SpecialKey::CAPSLOCK:
                        case SpecialKey::WIN:
                            break;
                    }
                }
                break;

            default:
                break;
        }
    }
}
 */

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
    input_queue_->push({});  // release the lock
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

void AnsiProtocol::event_text_input(std::string const &text)
{
    std::vector<uint8_t> v(text.begin(), text.end());
    output_queue_->push_all(v);
}

void AnsiProtocol::event_key(uint8_t key, bool is_down, KeyMod mod)
{
    if (is_down) {
        if (mod.control) {
            if (key >= 'a' && key <= 'z')
                output_queue_->push(key - 96);
        }
    }
}

void AnsiProtocol::event_key(SpecialKey key, bool is_down, KeyMod mod)
{
    auto push_str = [this](std::string const& str) {
        std::vector<uint8_t> v(str.begin(), str.end());
        output_queue_->push_all(v);
    };

    if (is_down) {
        switch (key) {
            case SpecialKey::ESC:        push_str("\e"); break;
            case SpecialKey::ENTER:      push_str("\r"); break;
            case SpecialKey::TAB:        push_str("\t"); break;
            case SpecialKey::BACKSPACE:  push_str("\b"); break;
            case SpecialKey::F1:         push_str("\e[11~"); break;
            case SpecialKey::F2:         push_str("\e[12~"); break;
            case SpecialKey::F3:         push_str("\e[13~"); break;
            case SpecialKey::F4:         push_str("\e[14~"); break;
            case SpecialKey::F5:         push_str("\e[15~"); break;
            case SpecialKey::F6:         push_str("\e[17~"); break;
            case SpecialKey::F7:         push_str("\e[18~"); break;
            case SpecialKey::F8:         push_str("\e[19~"); break;
            case SpecialKey::F9:         push_str("\e[20~"); break;
            case SpecialKey::F10:        push_str("\e[21~"); break;
            case SpecialKey::F11:        push_str("\e[23~"); break;
            case SpecialKey::F12:        push_str("\e[24~"); break;
            case SpecialKey::INSERT:     push_str("\e[2~"); break;
            case SpecialKey::HOME:       push_str("\e[1~"); break;
            case SpecialKey::END:        push_str("\e[4~"); break;
            case SpecialKey::PAGEUP:     push_str("\e[5~"); break;
            case SpecialKey::PAGEDOWN:   push_str("\e[6~"); break;
            case SpecialKey::UP:         push_str("\e[A"); break;
            case SpecialKey::DOWN:       push_str("\e[B"); break;
            case SpecialKey::LEFT:       push_str("\e[D"); break;
            case SpecialKey::RIGHT:      push_str("\e[C"); break;
            case SpecialKey::DELETE:     push_str("\e[3~"); break;
            case SpecialKey::PRINTSCREEN:
            case SpecialKey::PAUSEBREAK:
            case SpecialKey::CAPSLOCK:
            case SpecialKey::WIN:
                break;
        }
    }
}

