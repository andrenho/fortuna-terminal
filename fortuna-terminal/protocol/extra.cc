#include "extra.hh"

#include <sstream>

void Extra::send_bytes(std::vector<uint8_t> const &bytes)
{
    for (char c : bytes) {
        if (escape_active_) {
            escape_sequence_.push_back(c);
            if (escape_sequence_.size() == 2 && c != '*') {
                escape_active_ = false;
                escape_sequence_.clear();
            } else if (escape_sequence_.size() > 2 && std::isalpha(c)) {
                escape_sequence_complete();
                escape_active_ = false;
                escape_sequence_.clear();
            } else {
                escape_sequence_.push_back(c);
            }
        } else {
            if (c == '\e') {
                escape_active_ = true;
                escape_sequence_.push_back(c);
            }
        }
    }
}

void Extra::escape_sequence_complete()
{
    std::vector<size_t> parameters;
    char command = parse_escape_sequence(parameters);
    switch (command) {
        default:
            break;
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
