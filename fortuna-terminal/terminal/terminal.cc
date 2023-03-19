#include "terminal.hh"

#include "common/exceptions/fortunaexception.hh"

#include <cstring>

Terminal::Terminal(TerminalOptions terminal_options)
    : window_mode_(terminal_options.window_mode)
{
    SDL::init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

    if (!terminal_options.window_mode) {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        win_w_ = mode.w;
        win_h_ = mode.h;
    }

    window_ = SDL::get().emplace_window("Fortuna Terminal " VERSION,
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        win_w_, win_h_,
                                        SDL_WINDOW_OPENGL);

    renderer_ = SDL::get().emplace_renderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    print_renderer_info();

    SDL_ShowCursor(SDL_DISABLE);

    text_painter_ = std::make_unique<TextPainter>(renderer_);
    graphics_painter_ = std::make_unique<GraphicsPainter>(renderer_);
}

void Terminal::draw(Scene const& scene) const
{
    graphics_painter_->initialize_pending_images(scene);

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);

    graphics_painter_->draw_background(scene);
    if (scene.mode() == Mode::Graphics) {
        graphics_painter_->draw(scene, LAYER_TILEMAP_BG);
        graphics_painter_->draw(scene, LAYER_TILEMAP_OBSTACLES);
        graphics_painter_->draw(scene, LAYER_SPRITES);
        graphics_painter_->draw(scene, LAYER_TILEMAP_FG);
        graphics_painter_->draw(scene, LAYER_TILEMAP_UI);
    }
    text_painter_->draw(scene);

    SDL_RenderPresent(renderer_);
}

void Terminal::resize_window(Scene const& scene)
{
    auto [w, h] = scene.size_in_pixels();

    if (window_mode_) {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);

        // find adequate zoom
        unsigned int zoom_w = mode.w / w;
        unsigned int zoom_h = mode.h / h;
        unsigned int zoom = std::min(zoom_w, zoom_h);

        win_w_ = w * zoom;
        win_h_ = h * zoom;

        SDL_SetWindowSize(window_, win_w_, win_h_);
        SDL_SetWindowPosition(window_, (mode.w - win_w_) / 2, (mode.h - win_h_) / 2);
    }

    SDL_RenderSetLogicalSize(renderer_, w, h);
}

void Terminal::beep()
{
    // TODO
}

void Terminal::print_renderer_info()
{
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer_, &info);
    printf("Renderer:\n");
    printf("  Name: %s\n", info.name);
    printf("  Flags: ");
    if (info.flags & SDL_RENDERER_SOFTWARE)
        printf("software ");
    if (info.flags & SDL_RENDERER_ACCELERATED)
        printf("accelerated ");
    if (info.flags & SDL_RENDERER_PRESENTVSYNC)
        printf("vsync ");
    if (info.flags & SDL_RENDERER_TARGETTEXTURE)
        printf("target_texture ");
    printf("\n");
    printf("  Max texture width: %d\n", info.max_texture_width);
    printf("  Max texture height: %d\n", info.max_texture_height);
    printf("  Texture formats:\n");
    for (size_t i = 0; i < info.num_texture_formats; ++i)
        printf("    %s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
}