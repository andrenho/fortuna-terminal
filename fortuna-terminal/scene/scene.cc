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
        }
    }

    text_layer.update_blink();
}
