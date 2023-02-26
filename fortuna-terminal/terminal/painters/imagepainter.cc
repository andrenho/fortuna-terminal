#include "imagepainter.hh"

void ImagePainter::draw(ImageLayer &layer, TextureManager &texture_manager)
{
    initialize_sprites(layer, texture_manager);

    for (auto const& image : images_to_paint(layer, texture_manager)) {
        static SDL_Point center { 0, 0 };
        SDL_RenderCopyEx(renderer_, image.tx, &image.src, &image.dest, 0, &center, image.flip);
    }
}

void ImagePainter::initialize_sprites(ImageLayer &layer, TextureManager &texture_manager)
{
    std::optional<Image> oimg;
    while ((oimg = layer.pending_images()->pop_nonblock()).has_value()) {
        Image const& img = oimg.value();
        texture_manager.add_image(texture_index(), img, layer.palette);
    }
}
