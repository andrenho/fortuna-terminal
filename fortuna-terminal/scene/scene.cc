#include "scene.hh"
#include "global.hh"

void Scene::process_input_queue()
{
    std::optional<InputEvent> o_event;
    while ((o_event = input_queue.dequeue_noblock())) {
        auto const& event = o_event.value();
        switch (event.type) {

            case InputEventType::NoEvent:
                break;

            case InputEventType::TextPrintChar:
                text_layer.add_char(event.p1);
                break;

            case InputEventType::TextMoveUp:
                text_layer.move_cursor_relative(-event.p1, 0);
                break;
            case InputEventType::TextMoveDown:
                text_layer.move_cursor_relative(event.p1, 0);
                break;
            case InputEventType::TextMoveForward:
                text_layer.move_cursor_relative(0, event.p1);
                break;
            case InputEventType::TextMoveBackward:
                text_layer.move_cursor_relative(0, -event.p1);
                break;
            case InputEventType::TextMoveTo:
                text_layer.move_cursor_to(event.p1, event.p2);
                break;
            case InputEventType::TextSetColor:
                text_layer.set_color(event.p1);
                break;
            case InputEventType::TextClearScreen:
                text_layer.clear_screen();
                break;
            case InputEventType::TextResetFormatting:
                text_layer.reset_formatting();
                break;
            case InputEventType::TextClearToBeginningOfLine:
                text_layer.clear_to_beginning_of_line();
                break;
            case InputEventType::TextClearToEndOfLine:
                text_layer.clear_to_end_of_line();
                break;
            case InputEventType::TextClearLine:
                text_layer.clear_line();
                break;
            case InputEventType::SetScrollRegion:
                text_layer.set_scroll_region(event.p1, event.p2);
                break;
            case InputEventType::DeleteCharUnderCursor:
                text_layer.delete_char_under_cursor();
                break;
        }
    }

    text_layer.update_blink();
}
