#ifndef SDL_HH_
#define SDL_HH_

#include <vector>

#include <SDL2/SDL.h>

// this class wraps SDL to provide RAII
class SDL {
public:
    ~SDL();

    SDL_Window*   emplace_window(const char *title, int x, int y, int w, int h, Uint32 flags);
    SDL_Renderer* emplace_renderer(SDL_Window * window, int index, Uint32 flags);
    SDL_Texture*  emplace_texture(SDL_Renderer * renderer, Uint32 format, int access, int w, int h);

    static void init(Uint32 flags);
    static SDL& get();

private:
    SDL() = default;

    bool initialized = false;

    std::vector<SDL_Window*>   windows_;
    std::vector<SDL_Renderer*> renderers_;
    std::vector<SDL_Texture*>  textures_;

    void check_was_init() const;
};

#endif //SDL_HH_
