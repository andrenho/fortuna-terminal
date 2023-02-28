#include "scene.hh"

#include "terminal/painters/textpainter.hh"
#include "exceptions/fortunaexception.hh"

size_t Scene::image_texture_idx_counter = 0;

Scene::Scene(Mode mode)
    : text(mode),
      mode_(mode),
      texture_image_index_(image_texture_idx_counter++)
{
}

Size Scene::terminal_size() const
{
    Size sz {};
    if (mode_ == Mode::Graphics) {
        sz.w = TextLayer::Columns_40Columns * TextPainter::TextChar_W;
        sz.h = TextLayer::Lines_40Columns * TextPainter::TextChar_H;
    } else {
        sz.w = TextLayer::Columns_80Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
        sz.h = TextLayer::Lines_80Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
    }
    return sz;
}

void Scene::reset()
{
    text.reset();
}

void Scene::set_mode(Mode mode)
{
    mode_ = mode;
    text.set_mode(mode);
}

std::optional<TilemapLayer *> Scene::tilemap_layer(size_t index)
{
    switch (index) {
        case 4: return &tilemap_background;
        case 8: return &tilemap_obstacles;
        case 16: return &tilemap_foreground;
        case 20: return &tilemap_ui;
        default: return {};
    }
}

