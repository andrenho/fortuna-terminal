#include "tilemappainter.hh"
#include "scene/layers/tilemaplayer.hh"

std::vector<ImagePainter::ImageToPaint> TilemapPainter::images_to_paint(Scene const& scene, ImageLayer const &layer,
                                                          TextureManager const &texture_manager) const
{
    auto const* tilemap_layer = reinterpret_cast<TilemapLayer const*>(&layer);
    return std::vector<ImageToPaint>();
}
