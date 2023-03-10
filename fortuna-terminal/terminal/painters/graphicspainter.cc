#include "graphicspainter.hh"
#include "common/exceptions/sdlexception.hh"

void GraphicsPainter::initialize_pending_images(Scene const& scene)
{
    std::optional<size_t> image_idx;
    while ((image_idx = scene.pending_images().pop_nonblock()).has_value()) {
        texture_atlas_.emplace_from_image(scene.unique_id(), *image_idx, scene.image(*image_idx), scene.palette);
    }
}

void GraphicsPainter::draw_background(Scene const &scene) const
{
    Color bg = scene.palette[scene.bg_color];
    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, SDL_ALPHA_OPAQUE);
    auto [w, h] = scene.size_in_pixels();
    SDL_Rect r = { 0, 0, w, h };
    SDL_RenderFillRect(renderer_, &r);
}

void GraphicsPainter::draw(Scene const &scene, LayerIdentifier layer_id) const
{
    ImageLayer const* layer = scene.image_layer_unsafe(layer_id);
    if (!layer->enabled)
        return;

    auto [sw, sh] = scene.size_in_pixels();

    for (auto const& image: layer->images_to_draw_next_frame(scene)) {

        static SDL_Point center { 0, 0 };
        TextureInfo ti = texture_atlas_.get_texture(scene.unique_id(), image.image_idx);
        if (ti.tx == nullptr)
            break;
        SDL_Rect dest { image.pos_x, image.pos_y, Image::IMAGE_W, Image::IMAGE_H };

        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (image.mirrored_h && !image.mirrored_v)
            flip = SDL_FLIP_HORIZONTAL;
        else if (!image.mirrored_h && image.mirrored_v)
            flip = SDL_FLIP_VERTICAL;
        else if (image.mirrored_h && image.mirrored_v)
            flip = static_cast<SDL_RendererFlip>(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL);

        // check if within screen bounds
        if (ti.src.x > sw || ti.src.y > sh || (ti.src.x + ti.src.w) < 0 || (ti.src.y + ti.src.h) < 0)
            break;

        if (SDL_RenderCopyEx(renderer_, ti.tx, &ti.src, &dest, 0, &center, flip) < 0)
            throw SDLException("Error rendering");
    }
}

