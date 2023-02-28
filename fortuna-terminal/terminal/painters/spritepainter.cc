#include "spritepainter.hh"
#include "exceptions/sdlexception.hh"
#include "scene/layers/spritelayer.hh"

#include <cstring>
#include <iterator>

std::vector<ImagePainter::ImageToPaint> SpritePainter::images_to_paint(size_t texture_idx, ImageLayer const &layer, TextureManager const& texture_manager) const
{
    std::vector<ImageToPaint> images;

    for (size_t i = 0; i < SpriteLayer::MAX_SPRITES; ++i) {
        SpriteState const& ss = reinterpret_cast<SpriteLayer const*>(&layer)->sprite_state[i];
        if (ss.visible) {
            TextureInfo ti = texture_manager.texture_info(texture_idx, ss.image);

            ImageToPaint image;
            image.tx = ti.tx;
            if (image.tx == nullptr)
                break;
            image.src = ti.src;
            image.z = ss.z_order * SpriteLayer::MAX_SPRITES + i;

            if (ss.mirrored_h && !ss.mirrored_v)
                image.flip = SDL_FLIP_HORIZONTAL;
            else if (!ss.mirrored_h && ss.mirrored_v)
                image.flip = SDL_FLIP_VERTICAL;
            else if (ss.mirrored_h && ss.mirrored_v)
                image.flip = static_cast<SDL_RendererFlip>(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL);

            image.dest = { (int) ss.pos_x, (int) ss.pos_y, Image::IMAGE_W, Image::IMAGE_H };

            images.emplace_back(std::move(image));
        }
    }

    std::sort(std::begin(images), std::end(images),
              [](auto const& a, auto const& b) { return a.z < b.z; });

    return images;
}