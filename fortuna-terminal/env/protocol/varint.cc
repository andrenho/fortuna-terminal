#include "varint.hh"

#include <stdexcept>

static constexpr int MAX = 16383;
static constexpr int MIN = -16127;

static constexpr uint8_t RLE = 0xff;

std::vector<uint8_t> to_varint(std::vector<int> const& pars)
{
    std::vector<uint8_t> r;

    for (int par: pars) {
        if (par >= 0 && par <= 127) {
            r.push_back(par);
        } else if (par >= MIN && par <= MAX) {
            uint8_t b1 = (1 << 7) | (abs(par) >> 8);
            if (par < 0)
                b1 |= (1 << 6);
            r.push_back(b1);
            r.push_back(abs(par) & 0xff);
        } else {
            throw std::runtime_error("Integer outside of bounds");
        }
    }

    return r;
}

std::pair<size_t, std::vector<int>> from_varint(std::span<const uint8_t> const& array, size_t count)
{
    size_t pos = 0;

    std::vector<int> r;

    if (!array.empty() && array[0] == RLE) {

        auto [n_bytes, values] = from_varint(array.subspan(1), 2);
        pos = n_bytes + 1;
        std::fill_n(std::back_inserter(r), values[0], values[1]);

    } else {

        for (size_t i = 0; i < count; ++i) {
            uint8_t b1 = array[pos++];
            if (b1 & (1 << 7)) {
                int value = ((b1 & 0x3f) << 8) | array[pos++];
                if (b1 & (1 << 6))
                    value *= -1;
                r.push_back(value);
            } else {
                r.push_back(b1);
            }
        }
    }

    return { pos, r };
}