#ifndef SDL_PAINTER_HH_
#define SDL_PAINTER_HH_

#include "../../scene/textlayer.hh"

class SDL_Painter {
public:
    void initialize(struct SDL_Window* window);
    void finalize();

    void draw_deep_background() const;
    void draw_background(TextLayer const& text_layer) const;
    void draw_text(TextLayer const& text_layer) const;

    void present() const;

private:
    void load_font();
    void draw_text_cell(TextLayer const& chr, uint16_t line, uint16_t column) const;

    struct SDL_Renderer* renderer_ = nullptr;
    struct SDL_Texture* font_ = nullptr;

    static const unsigned int TEXT_CHAR_W = 6;
    static const unsigned int TEXT_CHAR_H = 9;
};

#endif //SDL_PAINTER_HH_
