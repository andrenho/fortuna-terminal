#include "scene.hh"

#include "common/exceptions/fortunaexception.hh"
#include "common/enums/pixelsize.hh"

size_t Scene::unique_id_counter = 0;

Scene::Scene()
    : unique_id_(unique_id_counter++)
{
    layers_[LAYER_TILEMAP_BG] = std::make_unique<TilemapLayer>();
    layers_[LAYER_TILEMAP_OBSTACLES] = std::make_unique<TilemapLayer>();
    layers_[LAYER_SPRITES] = std::make_unique<SpriteLayer>();
    layers_[LAYER_TILEMAP_FG] = std::make_unique<TilemapLayer>();
    layers_[LAYER_TILEMAP_UI] = std::make_unique<TilemapLayer>();
    layers_[LAYER_TILEMAP_TEXT] = std::make_unique<TextLayer>();

    palette_init(palette);
    set_mode(Mode::Text);
}

void Scene::reset()
{
    for (size_t i = 0; i < MAX_LAYERS; ++i)
        if (layers_[i])
            layers_[i]->reset();
    set_mode(Mode::Text);
    images_.clear();
    pending_images_->clear();
}

void Scene::set_mode(Mode mode)
{
    mode_ = mode;

    if (mode_ == Mode::Graphics) {
        size_in_pixels_ = {
                TextLayer::Columns_40Columns * PixelSize::TextChar_W,
                TextLayer::Lines_40Columns * PixelSize::TextChar_H
        };
    } else {
        size_in_pixels_ = {
                TextLayer::Columns_80Columns * PixelSize::TextChar_W + 2 * PixelSize::TextBorder,
                TextLayer::Lines_80Columns * PixelSize::TextChar_H + 2 * PixelSize::TextBorder
        };
    }

    text().set_mode(mode);
}

TextLayer &Scene::text() const
{
    return *reinterpret_cast<TextLayer*>(layers_[LAYER_TILEMAP_TEXT].get());
}

SpriteLayer &Scene::sprites() const
{
    return *reinterpret_cast<SpriteLayer*>(layers_[LAYER_SPRITES].get());
}

ImageLayer const *Scene::image_layer_unsafe(LayerIdentifier layer_id) const
{
    return reinterpret_cast<ImageLayer const *>(layers_[layer_id].get());
}

TilemapLayer* Scene::tilemap_layer(LayerIdentifier layer_id) const
{
    return dynamic_cast<TilemapLayer*>(layers_[layer_id].get());
}

Layer* Scene::layer(LayerIdentifier id) const
{
    auto& layer = layers_[id];
    if (layer)
        return layer.get();
    else
        return nullptr;
}

void Scene::add_image(size_t index, Image &&image)
{
    images_[index] = image;
    pending_images_->push(index);
}

template class SyncQueue<size_t>;
