#include "scene.hh"

#include "terminal/painters/textpainter.hh"
#include "exceptions/fortunaexception.hh"

Scene::Scene(Mode mode)
    : text(mode),
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

