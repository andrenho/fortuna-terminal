#include "scene.hh"

#include "../painters/textpainter.hh"
#include "exceptions/fortunaexception.hh"

Scene::Scene()
{
    text.set_80_columns(!graphical_mode_);
}

Size Scene::terminal_size() const
{
    Size sz {};
    if (graphical_mode_) {
        sz.w = Text::Columns_40Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
        sz.h = Text::Lines_40Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
    } else {
        sz.w = Text::Columns_80Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
        sz.h = Text::Lines_80Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
    }
    return sz;
}

void Scene::set_graphical_mode(bool value)
{
    graphical_mode_ = value;
    text.set_80_columns(!value);
}

void Scene::reset()
{
    text.reset();
}

