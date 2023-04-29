#include "varint.hh"

std::string to_varint(std::vector<int> const& pars)
{
    std::string str;

    for (int par: pars) {
        while (par > 127) {
            str += (char) ((par & 0x7f) | 0x80);
            par >>= 7;
        }
        str += (char) (par & 0x7f);
    }

    return str;
}

static std::pair<size_t, int> from_varint_single(std::string_view str)
{
    int value = 0;
    size_t shift = 0;
    size_t pos = 0;
    size_t len = str.length();
    size_t byte_count = 0;

    while (true) {
        if (pos >= len)
            return { 0, {} };

        uint8_t byte = str.at(pos++);
        value |= ((int) byte & 0x7f) << shift;
        ++byte_count;
        if (!(byte & 0x80))
            break;
        shift += 7;
    }

    return { true, value };
}

std::pair<size_t, std::vector<int>> from_varint(std::string_view str, size_t count)
{
    std::vector<int> r;
    size_t byte_count = 0;

    for (size_t i = 0; i < count; ++i) {
        auto [bytes, value] = from_varint_single(str.begin() + byte_count);
        r.push_back(value);
        byte_count += bytes;
    }

    return { byte_count, r };
}