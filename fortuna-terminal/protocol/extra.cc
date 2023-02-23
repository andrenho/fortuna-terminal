#include "extra.hh"

#include <iostream>
#include <sstream>

#include "control.hh"

void Extra::send_bytes(std::vector<uint8_t> const &bytes)
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
    std::vector<size_t> parameters;
    char command = parse_escape_sequence(parameters);
    switch (command) {
        case 'r':
            control_commands.push(ControlCommand::ResetProtocol);
            break;
        case 'x':
            std::cout << "Computer reset." << std::endl;
            gpio_.reset();
            break;
        default:
            break;
    }
}

char Extra::parse_escape_sequence(std::vector<size_t> &parameters) const
{
    std::stringstream ss(escape_sequence_.substr(2, escape_sequence_.size() - 4));
    std::string item;
    while (std::getline(ss, item, ';')) {
        size_t value = std::stoull(item);
        parameters.push_back(value);
    }

    return escape_sequence_.back();
}

bool Extra::escape_sequence_active() const
{
    return !escape_sequence_.empty();
}
