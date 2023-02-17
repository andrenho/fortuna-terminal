#include "text.hh"
#include "terminal/scene/scene.hh"

void Text::set_scene_mode(SceneMode scene_mode)
{
    switch (scene_mode) {
        case SceneMode::Text80Columns:
            columns_ = Columns_80Columns;
            lines_ = Lines_80Columns;
            break;
        case SceneMode::TextAndGraphics:
            columns_ = Columns_40Columns;
            lines_ = Lines_40Columns;
            break;
    }

    matrix_ = std::make_unique<Char[]>(columns_ * lines_);

    reset_attributes();

    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { (uint8_t) i, current_attrib_ };
}

Char const &Text::get(size_t column, size_t line) const
{
    return matrix_[line * columns_ + column];
}

Char &Text::get(size_t column, size_t line)
{
    return matrix_[line * columns_ + column];
}

void Text::set(size_t column, size_t line, uint8_t c)
{
    get(column, line) = { c, current_attrib_ };
}

void Text::set(size_t column, size_t line, Char c)
{
    get(column, line) = c;
}

void Text::reset_attributes()
{
    current_attrib_ = { COLOR_WHITE, false };
}
