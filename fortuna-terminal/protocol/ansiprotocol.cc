#include "ansiprotocol.hh"

#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>
#include <iomanip>

#include "exceptions/fortunaexception.hh"

using namespace std::chrono_literals;

AnsiProtocol::AnsiProtocol(std::unique_ptr<CommunicationModule> comm)
        : comm_(std::move(comm)),
          cache_(AnsiProtocol::initialize_cache({ Text::Columns_80Columns, Text::Lines_80Columns })),
          vt_(decltype(vt_)(
                  tmt_open(Text::Lines_80Columns, Text::Columns_80Columns, AnsiProtocol::tmt_callback, this, nullptr),
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
            if (byte.has_value()) {
                input_queue_->push(byte.value());
                debug_byte(true, byte.value());
            }
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
            std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(false, byte); });
        }
    });
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
    output_queue_->push({});  // release the lock
    output_thread_->join();
}


void AnsiProtocol::tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p)
{
    auto this_ = reinterpret_cast<AnsiProtocol*>(p);

    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);

    switch (m) {

        case TMT_MSG_MOVED: {
                this_->scene_.text.move_cursor_to(c->r, c->c);
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
                                this_->scene_.text.set(r, x, { (uint8_t) ch.c, translate_attrib(ch.a) });
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
    (void) mod;

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

void AnsiProtocol::debug_byte(bool is_input, uint8_t byte)
{
    if (debug_comm_) {
        if (is_input)
            std::cout << "\e[1;33m";
        else
            std::cout << "\e[1;34m";
        if (byte >= 32 && byte < 127)
            std::cout << byte;
        else if (byte == '\e')
            std::cout << "[ESC]";
        else
            std::cout << "[" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int) byte << "]";
        if (byte == 10)
            std::cout << "\n";
        std::cout << "\e[0m" << std::flush;
    }
}

