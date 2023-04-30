#ifndef VARINT_HH_
#define VARINT_HH_

#include <cstdint>
#include <span>
#include <vector>

std::vector<uint8_t> to_varint(std::vector<int> const& pars);

std::pair<size_t, std::vector<int>> from_varint(std::span<const uint8_t> const& array, size_t count);

#endif //VARINT_HH_
