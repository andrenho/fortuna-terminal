#include "scene.hh"

#include "../painters/textpainter.hh"

Scene::Scene()
{
    text.set_scene_mode(mode);
}

Size Scene::terminal_size() const
{
    Size sz {};
    switch (mode) {
        case SceneMode::Text80Columns:
            sz.w = Text::Columns_80Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
            sz.h = Text::Lines_80Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
            break;
        case SceneMode::TextAndGraphics:
            sz.w = Text::Columns_40Columns * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
            sz.h = Text::Lines_40Columns * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
            break;
    }
    return sz;
}

