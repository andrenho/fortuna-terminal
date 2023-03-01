#ifndef TEXTPAINTER_HH_
#define TEXTPAINTER_HH_

#include <functional>
#include <memory>

#include <SDL2/SDL.h>
#include "scene/layers/textlayer.hh"
#include "scene/scene.hh"

#include "common/noncopyable.hh"

class TextPainter : NonCopyable {
public:
    explicit TextPainter(SDL_Renderer* renderer);

    void draw(Scene const& scene) const;

    static constexpr size_t TextChar_W = 6;
    static constexpr size_t TextChar_H = 9;
    static constexpr size_t TextBorder = 12;

private:
    SDL_Renderer* renderer_;
    std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>>    font_;

    void draw_cell(TextLayer const &text, size_t line, size_t column, Palette const palette, uint8_t bg_color) const;
};

#endif //TEXTPAINTER_HH_
