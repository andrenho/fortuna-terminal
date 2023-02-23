#include "ansi.hh"

#include <cstring>
#include <iostream>

#include "exceptions/fortunaexception.hh"

ANSI::ANSI(Scene &scene)
    : scene_(scene),
      cache_(initialize_cache({Text::Columns_80Columns, Text::Lines_80Columns })),
        vt_(decltype(vt_)(
                tmt_open(Text::Lines_80Columns, Text::Columns_80Columns, ANSI::tmt_callback, this, nullptr),
                [](TMT* vt) { tmt_close(vt); }
        ))
{
    if (!vt_)
        throw FortunaException("Could not allocate terminal");
}

void ANSI::reset()
{
    std::string clrscr = "\033[2J\033[H";
    tmt_write(vt_.get(), clrscr.data(), clrscr.size());
}

void ANSI::send_bytes(std::string const &bytes)
{
    tmt_write(vt_.get(), bytes.data(), bytes.size());
}

ANSI::Cache ANSI::initialize_cache(Size initial_size)
{
    Cache k;
    for (size_t i = 0; i < (initial_size.w * initial_size.h); ++i)
        k.push_back(TMTCHAR { ' ', { false, false, false, false, false, false, tmt_color_t::TMT_COLOR_DEFAULT, tmt_color_t::TMT_COLOR_DEFAULT } });
    return k;
}

void ANSI::tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p)
{
    auto this_ = reinterpret_cast<ANSI*>(p);

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
            static std::vector<Cell> cells;
            for (size_t r = 0; r < s->nline; r++) {
                if (s->lines[r]->dirty) {
                    for (size_t x = 0; x < s->ncol; x++) {
                        TMTCHAR ch = s->lines[r]->chars[x];
                        TMTCHAR cached_ch = this_->cache_.at(r * s->ncol + x);
                        if (!tmtchar_equals(ch, cached_ch)) {
                            cells.emplace_back(Char { (uint8_t) ch.c, translate_attrib(ch.a) }, r, x);
                            this_->cache_[r * s->ncol + x] = ch;
                        }
                    }
                }
            }
            this_->scene_.text.set(cells);
            tmt_clean(vt);
        }
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

CharAttrib ANSI::translate_attrib(TMTATTRS a)
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

std::optional<std::string> ANSI::translate_special_key(SpecialKey special_key, KeyMod mod)
{
    (void) mod;

    switch (special_key) {
        case SpecialKey::ESC:        return "\e";
        case SpecialKey::ENTER:      return "\r";
        case SpecialKey::TAB:        return "\t";
        case SpecialKey::BACKSPACE:  return "\b";
        case SpecialKey::F1:         return "\e[11~";
        case SpecialKey::F2:         return "\e[12~";
        case SpecialKey::F3:         return "\e[13~";
        case SpecialKey::F4:         return "\e[14~";
        case SpecialKey::F5:         return "\e[15~";
        case SpecialKey::F6:         return "\e[17~";
        case SpecialKey::F7:         return "\e[18~";
        case SpecialKey::F8:         return "\e[19~";
        case SpecialKey::F9:         return "\e[20~";
        case SpecialKey::F10:        return "\e[21~";
        case SpecialKey::F11:        return "\e[23~";
        case SpecialKey::F12:        return "\e[24~";
        case SpecialKey::INSERT:     return "\e[2~";
        case SpecialKey::HOME:       return "\e[1~";
        case SpecialKey::END:        return "\e[4~";
        case SpecialKey::PAGEUP:     return "\e[5~";
        case SpecialKey::PAGEDOWN:   return "\e[6~";
        case SpecialKey::UP:         return "\e[A";
        case SpecialKey::DOWN:       return "\e[B";
        case SpecialKey::LEFT:       return "\e[D";
        case SpecialKey::RIGHT:      return "\e[C";
        case SpecialKey::DELETE:     return "\e[3~";
        case SpecialKey::PRINTSCREEN:
        case SpecialKey::PAUSEBREAK:
        case SpecialKey::CAPSLOCK:
        case SpecialKey::WIN:
            break;
    }
    return {};
}

bool ANSI::tmtchar_equals(TMTCHAR const& c1, TMTCHAR const& c2)
{
    return (uint8_t) c1.c == (uint8_t) c2.c &&
        c1.a.reverse == c2.a.reverse &&
        c1.a.bold == c2.a.bold &&
        c1.a.blink == c2.a.blink &&
        c1.a.dim == c2.a.dim &&
        c1.a.invisible == c2.a.invisible &&
        c1.a.underline == c2.a.underline &&
        c1.a.fg == c2.a.fg &&
        c1.a.bg == c2.a.bg;
}

