#ifndef TEXTPAINTER_HH_
#define TEXTPAINTER_HH_

#include <SDL2/SDL.h>
#include "../scene/layers/text.hh"

#include "../../common/noncopyable.hh"

class TextPainter : NonCopyable {
public:
    explicit TextPainter(SDL_Renderer* renderer) : renderer_(renderer) {}

    void draw(Text const& text) const;

private:
    SDL_Renderer* renderer_;
};

#endif //TEXTPAINTER_HH_
