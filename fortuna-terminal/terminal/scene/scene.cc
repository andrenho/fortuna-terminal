#include "scene.hh"

#include "../painters/textpainter.hh"
#include "exceptions/fortunaexception.hh"

Scene::Scene()
{
    text.set_graphical_mode(graphical_mode_);
}

Size Scene::terminal_size() const
{
    Size sz {};
    switch (graphical_mode_) {
        case TEXT_80_COLUMNS:
            sz.w = Text::Columns_80Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
            sz.h = Text::Lines_80Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
            break;
        case TEXT_AND_GRAPHICS:
            sz.w = Text::Columns_40Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
            sz.h = Text::Lines_40Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
            break;
        default:
            throw FortunaException("Trying to set graphical mode " + std::to_string(graphical_mode_));
    }
    return sz;
}

void Scene::set_graphical_mode(GraphicalMode graphical_mode)
{
    graphical_mode_ = graphical_mode;
    text.set_graphical_mode(graphical_mode_);
}

