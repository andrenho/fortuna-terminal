#include "scene.hh"

#include "../painters/textpainter.hh"

Size Scene::terminal_size() const
{
    Size sz {};
    switch (mode) {
        case SceneMode::Text80Columns:
            sz.w = 80 * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
            sz.h = 30 * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
            break;
        case SceneMode::TextAndGraphics:
            sz.w = 40 * TextPainter::TextChar_W + 2 * TextPainter::TextBorder;
            sz.h = 25 * TextPainter::TextChar_H + 2 * TextPainter::TextBorder;
            break;
    }
    return sz;
}
