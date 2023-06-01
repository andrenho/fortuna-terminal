#include "ansi.hh"

#include <cstring>

#include "common/exceptions/fortunaexception.hh"
#include "application/debug.hh"

static constexpr const wchar_t acs_charset[] = {
        0xa,   // right arrow
        0xa,   // left arrow
        0x8,   // up arrow
        0x9,   // down arrow
        0x7,   // block
        0x4,   // diamond
        0xb2,  // checkerboard
        0xf8,  // degree
        0xf1,  // plus/minus
        0xb1,  // board
        0xd9,  // lower right corner
        0xbf,  // upper right corner
        0xda,  // upper left corner
        0xd9,  // lower left corner
        0xc5,  // cross
        0xee,  // scan line 1
        0x2d,  // scan line 3
        0xc4,  // horizontal line
        0xee,  // scan line 7
        0x5f,  // scan line 9
        0xc3,  // left tee
        0xb4,  // right tee
        0xc1,  // top tee
        0xc2,  // bottom tee
        0xb3,  // vertical line
        0x11,  // less than or equal (TODO)
        0x10,  // greater than or equal (TODO)
        0xf4,  // pi
        '!',   // not equal (TODO)
        'f',   // pound starling
        0xfe,  // bullet
};

ANSI::ANSI(Scene &scene)
    : scene_(scene)
{
    initialize_cache();
    vt_ = decltype(vt_)(
            tmt_open(TextLayer::Lines_80Columns, TextLayer::Columns_80Columns, ANSI::tmt_callback, this, acs_charset),
            [](TMT* vt) { tmt_close(vt); }
    );
    if (!vt_)
        throw FortunaException("Could not allocate terminal");
}

void ANSI::reset_protocol()
{
    std::string clrscr = "\033[2J\033[H";
    tmt_write(vt_.get(), clrscr.data(), clrscr.size());
    current_mode_ = scene_.mode();
}

void ANSI::process_input(std::span<const uint8_t> const &bytes)
{
    if (bytes.empty())
        return;

    tmt_write(vt_.get(), (const char *) bytes.data(), bytes.size());
    if (scene_.mode() != current_mode_) {
        current_mode_ = scene_.mode();
        resize_text_terminal(current_mode_);
    }
}

void ANSI::resize_text_terminal(Mode mode)
{
    size_t nline = mode == Mode::Text ? TextLayer::Lines_80Columns: TextLayer::Lines_40Columns;
    size_t ncols = mode == Mode::Text ? TextLayer::Columns_80Columns : TextLayer::Columns_40Columns;
    tmt_resize(vt_.get(), nline, ncols);
    debug().info("Text terminal resized to %d x %d.", nline, ncols);
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

void ANSI::initialize_cache()
{
    cache_ = Cache(TextLayer::Columns_80Columns * std::max(TextLayer::Lines_40Columns, TextLayer::Lines_80Columns),
                   { ' ', { false, false, false, false, false, false, tmt_color_t::TMT_COLOR_DEFAULT, tmt_color_t::TMT_COLOR_DEFAULT } });
}

void ANSI::event_text_input(std::string const &text)
{
    output_.insert(output_.end(), text.begin(), text.end());
}

void ANSI::event_key(uint8_t key, bool is_down, KeyMod mod)
{
    if (is_down && mod.control && key >= 'a' && key <= 'z')
        output_.push_back((char) (key - 96));
}

void ANSI::event_key(SpecialKey key, bool is_down, KeyMod mod)
{
    if (is_down) {
        auto ostr = translate_special_key(key, mod);
        if (ostr.has_value()) {
            output_.insert(output_.end(), ostr->begin(), ostr->end());
        }
    }
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

std::vector<uint8_t> ANSI::output()
{
    std::vector<uint8_t> out = output_;
    output_.clear();
    return out;
}

