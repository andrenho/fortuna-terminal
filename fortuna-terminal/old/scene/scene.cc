#include "scene.hh"

#include "old/terminal/painters/textpainter.hh"
#include "old/exceptions/fortunaexception.hh"

size_t Scene::image_texture_idx_counter = 0;

Scene::Scene(Mode mode)
    : mode_(mode),
      texture_image_index_(image_texture_idx_counter++)
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

std::pair<int, int> Scene::size_in_pixels() const
{
    if (mode_ == Mode::Graphics) {
        return {
            TextLayer::Columns_40Columns * TextPainter::TextChar_W,
            TextLayer::Lines_40Columns * TextPainter::TextChar_H
        };
    } else {
        return {
            TextLayer::Columns_80Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder,
            TextLayer::Lines_80Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder
        };
    }
}
