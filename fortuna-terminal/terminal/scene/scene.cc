#include "scene.hh"

#include "../painters/textpainter.hh"
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
        sz.w = Text::Columns_40Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
        sz.h = Text::Lines_40Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
    } else {
        sz.w = Text::Columns_80Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
        sz.h = Text::Lines_80Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
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

