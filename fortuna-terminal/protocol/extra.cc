#include "extra.hh"

#include <iostream>
#include <sstream>

#include "control.hh"

void Extra::send_bytes(std::string const &bytes)
{
    for (char c : bytes) {
        escape_sequence_ += c;
        if (std::isalpha(c)) {
            escape_sequence_complete();
            escape_sequence_.clear();
        }
    }
}

void Extra::escape_sequence_complete()
{
    std::vector<size_t> p;
    char command = parse_escape_sequence(p);
    switch (command) {
        case 'r':
            control.emplace(ControlCommand::ResetProtocol);
            break;
        case 'x':
            std::cout << "Computer reset." << std::endl;
            gpio_.reset();
            break;
        case 'g':
            if (!p.empty()) {
                if (p.at(0) == 0)
                    control.emplace(ControlCommand::SetMode, Mode::Text);
                else if (p.at(0) == 1)
                    control.emplace(ControlCommand::SetMode, Mode::Graphics);
            }
            break;
    }

    if (mode_ == Mode::Graphics) {
        switch (command) {
            case 'm':
                if (!p.empty())
                    control.emplace(ControlCommand::SetMouseActive, p.at(0));
                break;
            case 'n':
                if (!p.empty())
                    control.emplace(ControlCommand::SetMouseMoveReport, p.at(0));
                break;
            case 'j':
                if (!p.empty())
                    control.emplace(ControlCommand::SetJoystickEmulation, p.at(0));
                break;
            case 'B':
                if (!p.empty())
                    scene_.text.bg_color = p.at(0) % PALETTE_SZ;
                break;
            case 's':
                if (p.size() < (Image::IMAGE_SZ + 2)) {
                    std::cerr << "warning: trying to create a sprite but has less than 258 bytes" << std::endl;
                } else {
                    Image image {
                        .key = (uint16_t) (p.at(0) % SpriteLayer::MAX_SPRITE_IMAGES),
                        .transparent_color = (uint8_t) p.at(1),
                        .image = {0}
                    };
                    std::copy(p.begin() + 2, p.end(), image.image);
                    scene_.sprites.create_image(std::move(image));
                }
                break;
            case 'S':
                if (p.size() >= 3) {
                    SpriteState& ss = scene_.sprites.sprite_state[p.at(0) & SpriteLayer::MAX_SPRITES];
                    ss.pos_x = p.at(1);
                    ss.pos_y = p.at(2);
                    if (p.size() >= 4)
                        ss.visible = static_cast<bool>(p.at(3));
                    if (p.size() >= 5)
                        ss.mirrored_h = static_cast<bool>(p.at(4));
                    if (p.size() >= 6)
                        ss.mirrored_h = static_cast<bool>(p.at(5));
                    if (p.size() >= 7)
                        ss.z_order = (uint8_t) p.at(6);
                    if (p.size() >= 8)
                        ss.image = (uint16_t) p.at(7);
                }
                break;
            default:
                break;
        }
    }
}

char Extra::parse_escape_sequence(std::vector<size_t> &parameters) const
{
    std::stringstream ss(escape_sequence_.substr(2, escape_sequence_.size() - 3));
    std::string item;
    while (std::getline(ss, item, ';')) {
        size_t value = std::stoull(item);
        parameters.push_back(value);
    }

    return escape_sequence_.back();
}