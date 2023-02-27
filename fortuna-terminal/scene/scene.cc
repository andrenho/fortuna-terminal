#include "scene.hh"

#include "terminal/painters/textpainter.hh"
#include "exceptions/fortunaexception.hh"

size_t Scene::image_texture_idx_counter = 0;

Scene::Scene(Mode mode)
    : text(mode),
      sprite_image_texture_idx(image_texture_idx_counter++),
      tile_image_texture_idx(image_texture_idx_counter++),
      mode_(mode)
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

