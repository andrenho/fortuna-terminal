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