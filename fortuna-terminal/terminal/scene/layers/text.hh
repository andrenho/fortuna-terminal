#ifndef TEXT_HH_
#define TEXT_HH_

#include <cstdint>
#include <algorithm>

#include "layer.hh"
#include "fortuna-protocol.h"
#include "terminal/scene/scene_mode.hh"

struct Cursor {
    size_t   x = 0;
    size_t   y = 0;
    uint8_t  color = 0;
    bool     visible = true;
    bool     blink_state = true;
    uint64_t last_blink = 0;
};

struct Char {
    uint8_t    c      = 0;
    CharAttrib attrib = { COLOR_WHITE, false };
};

class Text : public Layer {
public:
    void        set_scene_mode(SceneMode scene_mode);

    Char const& get(size_t column, size_t line) const;
    Char&       get(size_t column, size_t line);
    void        set(size_t column, size_t line, uint8_t c);
    void        set(size_t column, size_t line, Char c);

    void        reset_attributes();

    static constexpr size_t Columns_80Columns = 80;
    static constexpr size_t Lines_80Columns   = 40;
    static constexpr size_t Columns_40Columns = 40;
    static constexpr size_t Lines_40Columns   = 25;

private:
    static constexpr size_t MaxColumns = std::max(Columns_80Columns, Columns_40Columns);
    static constexpr size_t MaxLines   = std::max(Lines_80Columns, Lines_40Columns);

    Char matrix_[MaxColumns * MaxLines] = {};
    Cursor cursor_;

    size_t columns_;
    size_t lines_;

    CharAttrib current_attrib_ = { COLOR_WHITE, false };
};

#endif
