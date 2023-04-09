#include "fortuna.hh"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "application/control.hh"

// protocol described at https://github.com/andrenho/fortuna-terminal/wiki/ANSI-protocol-extension-for-Fortuna-Terminal

using namespace std::string_literals;

void FortunaProtocol::send_fortuna_bytes(std::string const &bytes)
{
    for (char c : bytes) {
        current_escape_sequence_ += c;
        if (std::isalpha(c)) {
            execute_escape_sequence();
            current_escape_sequence_.clear();
        }
    }
}

void FortunaProtocol::execute_escape_sequence()
{
    static std::vector<ssize_t> p;
    p.resize(0);
    char command = parse_escape_sequence(p);
    switch (command) {
        case 'r':
            control_queue.emplace(ControlCommand::ResetProtocol);
            break;
        case 'x':
            control_queue.emplace(ControlCommand::ResetComputer);
            break;
        case 'g':
            if (!p.empty()) {
                if (p.at(0) == 0)
                    control_queue.emplace(ControlCommand::SetMode, Mode::Text);
                else if (p.at(0) == 1)
                    control_queue.emplace(ControlCommand::SetMode, Mode::Graphics);
            }
            break;
        case 'V':
            if (!p.empty())
                control_queue.emplace(ControlCommand::EnableVSYNC, p.at(0));
            break;
        case 'v': {
                std::string version = VERSION;
                std::replace(version.begin(), version.end(), '.', ';');
                fortuna_output_queue_ << "\e#" + version + "v";
            }
            break;
        case 'm':
            if (!p.empty())
                control_queue.emplace(ControlCommand::SetMouseActive, p.at(0));
            break;
        case 'n':
            if (!p.empty())
                control_queue.emplace(ControlCommand::SetMouseMoveReport, p.at(0));
            break;
        case 'j':
            if (!p.empty())
                control_queue.emplace(ControlCommand::SetJoystickEmulation, p.at(0));
            break;
        case 'B':
            if (!p.empty())
                scene_.bg_color = (uint8_t)(p.at(0) % PALETTE_SZ);
            break;
        case 'i':
            if (p.size() < (Image::IMAGE_SZ + 2)) {
                fprintf(stderr, "warning: trying to create a sprites but has less than 258 bytes\n");
            } else {
                size_t index = (uint16_t) (p.at(0) % Scene::MAX_IMAGES);
                Image image = {
                        .transparent_color = (uint8_t) p.at(1),
                        .pixels = {0}
                };
                std::copy(p.begin() + 2, p.end(), image.pixels);
                scene_.add_image(index, std::move(image));
            }
            break;
        case 't':
            if (p.size() >= 3) {
                size_t tilemap_idx = p.at(0) % Tilemap::MAX_TILEMAPS;
                size_t w = p.at(1);
                size_t h = p.at(2);
                if (p.size() >= 3 + (w * h)) {
                    Tilemap& tilemap = scene_.tilemaps[tilemap_idx];
                    tilemap.w = w;
                    tilemap.h = h;
                    tilemap.image_indexes.clear();
                    std::copy(p.begin() + 3, p.begin() + 3 + (w * h), std::back_inserter(tilemap.image_indexes));
                }
            }
            break;
        case 'S':
            if (p.size() >= 3) {
                Sprite& ss = scene_.sprites().sprites[p.at(0) % SpriteLayer::MAX_SPRITES];
                ss.pos_x = (int) p.at(1);
                ss.pos_y = (int) p.at(2);
                if (p.size() >= 4)
                    ss.visible = static_cast<bool>(p.at(3));
                if (p.size() >= 5)
                    ss.mirrored_h = static_cast<bool>(p.at(4));
                if (p.size() >= 6)
                    ss.mirrored_h = static_cast<bool>(p.at(5));
                if (p.size() >= 7)
                    ss.z_order = (uint8_t) p.at(6);
                if (p.size() >= 8)
                    ss.image_idx = (uint16_t) p.at(7);
            }
            break;
        case 'M':
            if (p.size() >= 4) {
                try {
                    TilemapLayer* tilemap_layer = scene_.tilemap_layer((LayerIdentifier) p.at(0));
                    if (tilemap_layer == nullptr)
                        break;
                    tilemap_layer->map = (ssize_t) (p.at(1) % Tilemap::MAX_TILEMAPS);
                    tilemap_layer->pos_x = p.at(2);
                    tilemap_layer->pos_y = p.at(3);
                } catch (std::out_of_range&) {}
            }
            break;
        case 'P':
            if (p.size () >= 3) {
                Palette palette;
                std::copy(std::begin(scene_.palette), std::end(scene_.palette), palette);
                for (size_t i = 0; i < PALETTE_SZ; ++i) {
                    if (p.size() >= ((i + 1) * 3)) {
                        palette[i].r = (uint8_t) p.at(i * 3);
                        palette[i].g = (uint8_t) p.at(i * 3 + 1);
                        palette[i].b = (uint8_t) p.at(i * 3 + 2);
                    }
                }
                std::copy(std::begin(palette), std::end(palette), scene_.palette);
            }
            break;
        case 'L':
            if (p.size() >= 2) {
                try {
                    scene_.layer(static_cast<LayerIdentifier>(p.at(0)))->enabled = p.at(1);
                } catch (std::out_of_range&) {}
            }
            break;
        case 'C':
            if (p.size() >= 2)
                scene_.sprites().subscribe_to_collisions(p.at(0), p.at(1));
            break;
        case 'U':
            if (p.size() >= 2)
                scene_.sprites().unsubscribe_to_collisions(p.at(0), p.at(1));
            break;
        case 'X':
            scene_.sprites().unsubscribe_to_all_collisions();
            break;
        default:
            break;
    }
}

char FortunaProtocol::parse_escape_sequence(std::vector<ssize_t> &parameters) const
{
    std::stringstream ss(current_escape_sequence_.substr(2, current_escape_sequence_.size() - 3));
    std::string item;
    while (std::getline(ss, item, ';')) {
        if (item.starts_with("$")) {
            int pos = item.find_first_of(',', 0);
            ssize_t count = std::stoll(item.substr(1, pos - 1));
            ssize_t value = std::stoll(item.substr(pos + 1));
            for (ssize_t i = 0; i < count ; ++i)
                parameters.push_back(value);
        } else {
            ssize_t value = std::stoll(item);
            parameters.push_back(value);
        }
    }

    return current_escape_sequence_.back();
}

std::string FortunaProtocol::output_collisions()
{
    std::stringstream ss;
    if (scene_.mode() == Mode::Graphics) {
        for (auto const& collision: scene_.sprites().check_for_new_collisions()) {
            ss << "\e#"s + (collision.type == Collision::Colliding ? "1" : "0") + ";" +
                std::to_string(collision.sprite_a) + ";" + std::to_string(collision.sprite_b);
        }
    }
    return ss.str();
}

void FortunaProtocol::reset_fortuna_protocol()
{
    current_escape_sequence_.clear();
}

std::string FortunaProtocol::get_lastest_fortuna_outputs()
{
    std::string events = fortuna_output_queue_.str();
    fortuna_output_queue_.str("");
    return events;
}

