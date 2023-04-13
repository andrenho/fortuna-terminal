#include "fortuna.hh"

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <stdexcept>

#include "application/control.hh"
#include "application/debug.hh"
#include "common/exceptions/fortunaexception.hh"

// protocol described at https://github.com/andrenho/fortuna-terminal/wiki/ANSI-protocol-extension-for-Fortuna-Terminal

using namespace std::string_literals;

static ssize_t to_ssize(std::string const& str)
{
    try {
        return std::stoll(str);
    } catch (std::invalid_argument& e) {
        throw FortunaException("Could not convert the string \"" + str + "\" to number: " + e.what());
    } catch (std::out_of_range& e) {
        throw FortunaException("Integer conversion out of range for the string string \"" + str + "\" to number: " + e.what());
    }
}

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
    std::vector<ssize_t> p;
    p.reserve(258);

    char command = parse_escape_sequence(p);
    switch (command) {
        case 'r':
            control_queue.emplace(ControlCommand::ResetProtocol);
            debug().info("fortuna: reset protocol");
            break;
        case 'x':
            control_queue.emplace(ControlCommand::ResetComputer);
            debug().info("fortuna: reset computer");
            break;
        case 'g':
            if (!p.empty()) {
                if (p.at(0) == 0)
                    control_queue.emplace(ControlCommand::SetMode, Mode::Text);
                else if (p.at(0) == 1)
                    control_queue.emplace(ControlCommand::SetMode, Mode::Graphics);
                debug().info("fortuna: graphics mode %s", p.at(0) ? "enabled" : "disabled");
            }
            break;
        case 'V':
            if (!p.empty()) {
                control_queue.emplace(ControlCommand::EnableVSYNC, p.at(0));
                debug().info("fortuna: VSYNC %s", p.at(0) ? "enabled" : "disabled");
            }
            break;
        case 'v': {
                std::string version = VERSION;
                std::replace(version.begin(), version.end(), '.', ';');
                fortuna_output_queue_ << "\e#" + version + "v";
                debug().info("fortuna: version requested (the version is " VERSION ")");
            }
            break;
        case 'm':
            if (!p.empty()) {
                control_queue.emplace(ControlCommand::SetMouseActive, p.at(0));
                debug().info("fortuna: mouse %s", p.at(0) ? "enabled" : "disabled");
            }
            break;
        case 'n':
            if (!p.empty()) {
                control_queue.emplace(ControlCommand::SetMouseMoveReport, p.at(0));
                debug().info("fortuna: mouse move %s", p.at(0) ? "enabled" : "disabled");
            }
            break;
        case 'j':
            if (!p.empty()) {
                control_queue.emplace(ControlCommand::SetJoystickEmulation, p.at(0));
                debug().info("fortuna: joystick emulation %s", p.at(0) ? "enabled" : "disabled");
            }
            break;
        case 'B':
            if (!p.empty()) {
                scene_.bg_color = (uint8_t)(p.at(0) % PALETTE_SZ);
                debug().info("fortuna: background color %d", scene_.bg_color);
            }
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
                debug().info("fortuna: image index %d created (transparent color is %d)", index, image.transparent_color);
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
                    debug().info("fortuna: tilemap %d set to the size %zu x %zu", tilemap_idx, w, h);
                } else {
                    fprintf(stderr, "warning: trying to create a tilemap but less than %zu bytes\n", w * h);
                }
            }
            break;
        case 'S':
            if (p.size() >= 3) {
                Sprite& ss = scene_.sprites().sprites[p.at(0) % SpriteLayer::MAX_SPRITES];
                ss.pos_x = (int) p.at(1);
                ss.pos_y = (int) p.at(2);
                debug().debug("fortuna: setting sprite %d at position %d, %d", p.at(0) % SpriteLayer::MAX_SPRITES, ss.pos_x, ss.pos_y);
                if (p.size() >= 4) {
                    ss.visible = static_cast<bool>(p.at(3));
                    debug().debug("  %svisible", ss.visible ? "" : "in");
                }
                if (p.size() >= 5) {
                    ss.mirrored_h = static_cast<bool>(p.at(4));
                    debug().debug("  H %smirrored", ss.mirrored_h ? "not " : "");
                }
                if (p.size() >= 6) {
                    ss.mirrored_v = static_cast<bool>(p.at(5));
                    debug().debug("  V %smirrored", ss.mirrored_v ? "not " : "");
                }
                if (p.size() >= 7) {
                    ss.z_order = (uint8_t) p.at(6);
                    debug().debug("  z-order %d", ss.z_order);
                }
                if (p.size() >= 8) {
                    ss.image_idx = (uint16_t) p.at(7);
                    debug().debug("  image index %d", ss.image_idx);
                }
            }
            break;
        case 'M':
            if (p.size() >= 4) {
                try {
                    TilemapLayer* tilemap_layer = scene_.tilemap_layer((LayerIdentifier) p.at(0));
                    if (tilemap_layer == nullptr) {
                        fprintf(stderr, "warning: layer is not a tilemap layer\n");
                        break;
                    }
                    tilemap_layer->map = (ssize_t) (p.at(1) % Tilemap::MAX_TILEMAPS);
                    tilemap_layer->pos_x = p.at(2);
                    tilemap_layer->pos_y = p.at(3);
                    debug().debug("fortuna: map layer %d: use tilemap %zi and place it at %zi, %zi", p.at(0), tilemap_layer->map, p.at(2), p.at(3));
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
                debug().info("fortuna: palette set");
            }
            break;
        case 'L':
            if (p.size() >= 2) {
                try {
                    scene_.layer(static_cast<LayerIdentifier>(p.at(0)))->enabled = p.at(1);
                    debug().info("fortuna: layer %zi %s", p.at(0), p.at(1) ? "enabled" : "disabled");
                } catch (std::out_of_range&) {}
            }
            break;
        case 'C':
            if (p.size() >= 2) {
                scene_.sprites().subscribe_to_collisions(p.at(0), p.at(1));
                debug().info("fortuna: subscribed to collisions between sprite %zi and sprite %zi", p.at(0), p.at(1));
            }
            break;
        case 'U':
            if (p.size() >= 2) {
                scene_.sprites().unsubscribe_to_collisions(p.at(0), p.at(1));
                debug().info("fortuna: unsubscribed to collisions between sprite %zi and sprite %zi", p.at(0), p.at(1));
            }
            break;
        case 'X':
            scene_.sprites().unsubscribe_to_all_collisions();
            debug().info("fortuna: unsubscribed to all collisions");
            break;
        default:
            fprintf(stderr, "fortuna: invalid command '%c'", command);
            break;
    }
}

char FortunaProtocol::parse_escape_sequence(std::vector<ssize_t> &parameters) const
{
    size_t i = 2;  // skip ESC and "*"

    while (true) {

        size_t next_semicolon = current_escape_sequence_.find(';', i);
        if (next_semicolon == std::string::npos)
            next_semicolon = current_escape_sequence_.length() - 1;

        if (i > next_semicolon)
            break;

        std::string_view token(&current_escape_sequence_.c_str()[i], next_semicolon - i);

        if (token[0] == '$') {  // compressed command
            size_t next_comma = token.find(',');
            if (next_comma != std::string_view::npos) {  // if comma not found, bail out
                ssize_t count = to_ssize(std::string(token.substr(1, next_comma - 1)));
                ssize_t value = to_ssize(std::string(token.substr(next_comma + 1)));
                for (ssize_t j = 0; j < count ; ++j)
                    parameters.push_back(value);
            }

        } else {
            ssize_t value = to_ssize(std::string(token));
            parameters.push_back(value);
        }

        i = next_semicolon + 1;
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
            debug().debug("%s: sprite %zu and sprite %zu.", collision.type == Collision::Colliding ? "Collision" : "Separation",
                          collision.sprite_a, collision.sprite_b);
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

