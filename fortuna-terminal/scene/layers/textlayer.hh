#ifndef TEXTLAYER_HH_
#define TEXTLAYER_HH_

#include <cstdint>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "layer.hh"
#include "common/enums/mode.hh"
#include "common/types/time.hh"
#include "common/color.hh"

struct Cursor {
    struct Attrib {
        uint8_t color;
        bool    visible;
    };

    size_t     x = 0;
    size_t     y = 0;
    Attrib     attrib = { COLOR_ORANGE, true };
    bool       blink_state = true;
    TimePoint  last_blink = Time::now();
};

struct Char {
    struct Attrib {
        uint8_t color;
        bool    reverse;
        bool    visible;
    };

    uint8_t c      = 0;
    Attrib  attrib = { COLOR_WHITE, false, false };
};

struct Cell {
    Char   chr;
    size_t line;
    size_t column;

    Cell(Char const &chr_, size_t line_, size_t column_) : chr(chr_), line(line_), column(column_) {}
};

class TextLayer : public Layer {
public:
    TextLayer();

    void          update_char(size_t row, size_t column, Char c);
    void          update_cell(std::vector<Cell> const& cells);
    void          write_text(size_t row, size_t column, std::string const& text, Char::Attrib const& attrib);

    void          move_cursor_to(size_t line, size_t column);

    void          set_mode(Mode mode);

    void          update_blink();
    void          reset() override;

    [[nodiscard]] Char const&   get_char(size_t row, size_t column) const;
    [[nodiscard]] size_t        columns() const { return columns_; }
    [[nodiscard]] size_t        lines() const   { return lines_; }
    [[nodiscard]] Mode          mode() const    { return mode_; }
    [[nodiscard]] Cursor const& cursor() const  { return cursor_; }

    static constexpr size_t Columns_80Columns = 80;
    static constexpr size_t Lines_80Columns   = 30;
    static constexpr size_t Columns_40Columns = 42;
    static constexpr size_t Lines_40Columns   = 28;

private:
    std::unique_ptr<Char[]> matrix_ {};
    Cursor                  cursor_ {};

    size_t                  columns_ = 0;
    size_t                  lines_ = 0;

    Mode                    mode_ = Mode::Text;

    void reset_blink();
};

#endif
