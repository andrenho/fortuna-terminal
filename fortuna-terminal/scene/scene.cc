#include "scene.hh"

void Scene::update(InputQueue& input_queue)
{
    std::optional<InputEvent> o_event;
    while ((o_event = input_queue.pop())) {
        auto const& event = o_event.value();
        switch (event.type) {

            case InputEventType::PrintChar:
                text_layer.add_char(event.print_char);
                break;
        }
    }

    text_layer.update_blink();
}
