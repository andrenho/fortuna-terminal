#include "imagepainter.hh"
#include "exceptions/sdlexception.hh"

void ImagePainter::draw(Scene const& scene, ImageLayer &layer, TextureManager &texture_manager)
{
    initialize_sprites(scene, layer, texture_manager);

    for (auto const& image : images_to_paint(scene, layer, texture_manager)) {
        static SDL_Point center { 0, 0 };
        if (SDL_RenderCopyEx(renderer_, image.tx, &image.src, &image.dest, 0, &center, image.flip) < 0)
            throw SDLException("Error renderering");
    }
}

void ImagePainter::initialize_sprites(Scene const& scene, ImageLayer &layer, TextureManager &texture_manager)
{
    std::optional<Image> oimg;
    while ((oimg = layer.pending_images()->pop_nonblock()).has_value()) {
        Image const& img = oimg.value();
        texture_manager.add_image(scene.texture_image_index(), img, layer.palette);
    }
}
