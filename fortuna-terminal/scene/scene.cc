#include "scene.hh"

void Scene::process_input_queue(InputQueue& input_queue)
{
    std::optional<InputEvent> o_event;
    while ((o_event = input_queue.pop())) {
        auto const& event = o_event.value();
        switch (event.type) {

            case InputEventType::TextPrintChar:
                text_layer.add_char(event.p1);
                break;

            case InputEventType::TextMoveUp:
                text_layer.move_cursor_relative(0, -event.p1);
                break;
            case InputEventType::TextMoveDown:
                text_layer.move_cursor_relative(0, event.p1);
                break;
            case InputEventType::TextMoveForward:
                text_layer.move_cursor_relative(event.p1, 0);
                break;
            case InputEventType::TextMoveBackward:
                text_layer.move_cursor_relative(-event.p1, 0);
                break;
            case InputEventType::TextMoveTo:
                text_layer.move_cursor_to(event.p2, event.p1);
                break;
            case InputEventType::TextSetColor:
                text_layer.set_color(event.p2);
                break;
            case InputEventType::TextClearScreen:
                text_layer.clear_screen();
                break;
            case InputEventType::TextResetFormatting:
                text_layer.reset_formatting();
                break;
        }
    }

    text_layer.update_blink();
}
