#include "scene.hh"

#include "common/exceptions/fortunaexception.hh"
#include "common/pixelsize.hh"

size_t Scene::unique_id_counter = 0;

Scene::Scene(Mode mode)
    : mode_(mode),
      unique_id_(unique_id_counter++)
{
    layers_.emplace(LAYER_TILEMAP_BG, std::make_unique<TilemapLayer>());
    layers_.emplace(LAYER_TILEMAP_OBSTACLES, std::make_unique<TilemapLayer>());
    layers_.emplace(LAYER_SPRITES, std::make_unique<SpriteLayer>());
    layers_.emplace(LAYER_TILEMAP_FG, std::make_unique<TilemapLayer>());
    layers_.emplace(LAYER_TILEMAP_UI, std::make_unique<TilemapLayer>());
    layers_.emplace(LAYER_TILEMAP_TEXT, std::make_unique<TextLayer>(mode));

    palette_init(palette);
}

void Scene::reset()
{
    std::for_each(layers_.begin(), layers_.end(), [](auto& i) { i.second->reset(); });
}

void Scene::set_mode(Mode mode)
{
    mode_ = mode;
    text().set_mode(mode);
}

consteval std::pair<int, int> Scene::size_in_pixels() const
{
    if (mode_ == Mode::Graphics) {
        return {
            TextLayer::Columns_40Columns * PixelSize::TextChar_W,
            TextLayer::Lines_40Columns * PixelSize::TextChar_H
        };
    } else {
        return {
            TextLayer::Columns_80Columns * PixelSize::TextChar_W + 2 * PixelSize::TextBorder,
            TextLayer::Lines_80Columns * PixelSize::TextChar_H + 2 * PixelSize::TextBorder
        };
    }
}
