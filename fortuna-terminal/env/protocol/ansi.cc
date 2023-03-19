#include "ansi.hh"

#include <cstring>

#include "common/exceptions/fortunaexception.hh"

ANSI::ANSI(Mode initial_mode, Scene &scene)
    : scene_(scene),
      current_mode_(initial_mode),
      cache_(initialize_cache(TextLayer::Columns_80Columns, std::max(TextLayer::Lines_40Columns, TextLayer::Lines_80Columns))),
        vt_(decltype(vt_)(
                tmt_open(
                        initial_mode == Mode::Text ? TextLayer::Lines_80Columns : TextLayer::Lines_40Columns,
                        initial_mode == Mode::Text ? TextLayer::Columns_80Columns : TextLayer::Columns_40Columns,
                        ANSI::tmt_callback, this, nullptr),
                [](TMT* vt) { tmt_close(vt); }
        ))
{
    if (!vt_)
        throw FortunaException("Could not allocate terminal");
}

void ANSI::reset_ansi_protocol()
{
    std::string clrscr = "\033[2J\033[H";
    tmt_write(vt_.get(), clrscr.data(), clrscr.size());
    current_mode_ = scene_.mode();
}

void ANSI::send_ansi_bytes(std::string const &bytes)
{
    tmt_write(vt_.get(), bytes.data(), bytes.size());
    if (scene_.mode() != current_mode_) {
        current_mode_ = scene_.mode();
        resize_text_terminal(current_mode_);
    }
}

void ANSI::resize_text_terminal(Mode mode)
{
    tmt_resize(vt_.get(),
               mode == Mode::Text ? TextLayer::Lines_80Columns : TextLayer::Lines_40Columns,
               mode == Mode::Text ? TextLayer::Columns_80Columns : TextLayer::Columns_40Columns);
}

ANSI::Cache ANSI::initialize_cache(size_t w, size_t h)
{
    Cache k(w * h);
    std::fill(k.begin(), k.end(), TMTCHAR { ' ', { false, false, false, false, false, false, tmt_color_t::TMT_COLOR_DEFAULT, tmt_color_t::TMT_COLOR_DEFAULT } });
    return k;
}

void ANSI::tmt_callback(tmt_msg_t m, TMT *vt, [[maybe_unused]] void const *a, void *p)
{
    auto this_ = reinterpret_cast<ANSI*>(p);

    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);

    switch (m) {

        case TMT_MSG_MOVED:
            this_->scene_.text().move_cursor_to(c->r, c->c);
            tmt_clean(vt);
            break;

        case TMT_MSG_UPDATE: {
            static std::vector<Cell> cells;
            for (size_t r = 0; r < s->nline; r++) {
                if (s->lines[r]->dirty) {
                    for (size_t x = 0; x < s->ncol; x++) {
                        TMTCHAR ch = s->lines[r]->chars[x];
                        TMTCHAR cached_ch = this_->cache_.at(r * s->ncol + x);
                        if (tmtchar_not_equals(ch, cached_ch)) {
                            cells.emplace_back(Char { (uint8_t) ch.c, translate_attrib(ch.a) }, r, x);
                            this_->cache_[r * s->ncol + x] = ch;
                        }
                    }
                }
            }
            this_->scene_.text().update_cell(cells);
            tmt_clean(vt);
        }
            break;

        case TMT_MSG_ANSWER:
            break;

        case TMT_MSG_BELL:
            // TODO
            break;

        case TMT_MSG_CURSOR:
            break;
    }
}

Char::Attrib ANSI::translate_attrib(TMTATTRS a)
{
    Char::Attrib attr {};

    if (a.bold) {   // brighter
        switch (a.fg) {
            case TMT_COLOR_BLACK:   attr.color = COLOR_DARK_SLATE; break;
            case TMT_COLOR_RED:     attr.color = COLOR_RED; break;
            case TMT_COLOR_GREEN:   attr.color = COLOR_LIGHT_GREEN; break;
            case TMT_COLOR_YELLOW:  attr.color = COLOR_YELLOW; break;
            case TMT_COLOR_BLUE:    attr.color = COLOR_LIGHT_BLUE; break;
            case TMT_COLOR_MAGENTA: attr.color = COLOR_PINK; break;
            case TMT_COLOR_CYAN:    attr.color = COLOR_LIGHT_CYAN; break;
            default:
                attr.color = COLOR_WHITE;
                break;
        }
    } else if (a.dim) {   // darker
        switch (a.fg) {
            case TMT_COLOR_BLACK:   attr.color = COLOR_BLACK; break;
            case TMT_COLOR_RED:     attr.color = COLOR_DARK_RED; break;
            case TMT_COLOR_GREEN:   attr.color = COLOR_DARK_GREEN; break;
            case TMT_COLOR_YELLOW:  attr.color = COLOR_BROWN; break;
            case TMT_COLOR_BLUE:    attr.color = COLOR_DARK_BLUE; break;
            case TMT_COLOR_MAGENTA: attr.color = COLOR_DARK_PURPLE; break;
            case TMT_COLOR_CYAN:    attr.color = COLOR_DARK_CYAN; break;
            default:
                attr.color = COLOR_LIGHT_GRAY;
                break;
        }
    } else {   // normal
        switch (a.fg) {
            case TMT_COLOR_BLACK:   attr.color = COLOR_LIGHT_BLACK; break;
            case TMT_COLOR_RED:     attr.color = COLOR_RED; break;
            case TMT_COLOR_GREEN:   attr.color = COLOR_GREEN; break;
            case TMT_COLOR_YELLOW:  attr.color = COLOR_ORANGE; break;
            case TMT_COLOR_BLUE:    attr.color = COLOR_BLUE; break;
            case TMT_COLOR_MAGENTA: attr.color = COLOR_PURPLE; break;
            case TMT_COLOR_CYAN:    attr.color = COLOR_CYAN; break;
            default:
                attr.color = COLOR_DARK_WHITE;
                break;
        }
    }

    attr.reverse = a.reverse;
    return attr;
}

bool ANSI::tmtchar_not_equals(TMTCHAR const& c1, TMTCHAR const& c2)
{
    return (uint8_t) c1.c != (uint8_t) c2.c ||
        c1.a.reverse != c2.a.reverse ||
        c1.a.bold != c2.a.bold ||
        c1.a.blink != c2.a.blink ||
        c1.a.dim != c2.a.dim ||
        c1.a.invisible != c2.a.invisible ||
        c1.a.underline != c2.a.underline ||
        c1.a.fg != c2.a.fg ||
        c1.a.bg != c2.a.bg;
}

