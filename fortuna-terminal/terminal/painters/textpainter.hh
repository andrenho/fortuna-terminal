#ifndef TEXTPAINTER_HH_
#define TEXTPAINTER_HH_

#include <functional>
#include <memory>

#include <SDL2/SDL.h>
#include "../scene/layers/text.hh"

#include "../../common/noncopyable.hh"

class TextPainter : NonCopyable {
public:
    explicit TextPainter(SDL_Renderer* renderer);

    void draw(Text const& text) const;

    static constexpr size_t TextChar_W = 6;
    static constexpr size_t TextChar_H = 9;
    static constexpr size_t TextBorder = 12;

private:
    SDL_Renderer* renderer_;
    std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>> font_;
};

#endif //TEXTPAINTER_HH_
