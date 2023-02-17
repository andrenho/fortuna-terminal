#ifndef TEXT_HH_
#define TEXT_HH_

#include <cstdint>
#include <algorithm>
#include <chrono>
#include <memory>

#include "layer.hh"
#include "fortuna-protocol.h"
#include "terminal/scene/scene_mode.hh"

using TimePoint = std::chrono::high_resolution_clock::time_point;
using Time = std::chrono::high_resolution_clock;

struct Cursor {
    size_t    x = 0;
    size_t    y = 0;
    uint8_t   color = COLOR_ORANGE;
    bool      visible = true;
    bool      blink_state = true;
    TimePoint last_blink = Time::now();
};

struct Char {
    uint8_t    c      = 0;
    CharAttrib attrib = { COLOR_WHITE, false };
};

class Text : public Layer {
public:
    Text() : Layer() {}

    void          set_scene_mode(SceneMode scene_mode);

    Char const&   get(size_t line, size_t column) const;
    Char&         get(size_t line, size_t column);
    void          set(size_t line, size_t column, uint8_t c);
    void          set(size_t line, size_t column, Char c);

    void          reset_blink();
    void          reset_attributes();

    void          update_blink();

    size_t        columns() const { return columns_; }
    size_t        lines() const   { return lines_; }
    Cursor const& cursor() const  { return cursor_; }

    uint8_t       bg_color = COLOR_BLACK;

    static constexpr size_t Columns_80Columns = 80;
    static constexpr size_t Lines_80Columns   = 30;
    static constexpr size_t Columns_40Columns = 40;
    static constexpr size_t Lines_40Columns   = 25;

private:
    std::unique_ptr<Char[]> matrix_;
    Cursor cursor_;

    size_t columns_ = 0;
    size_t lines_ = 0;

    CharAttrib current_attrib_ = { COLOR_WHITE, false };
};

#endif
