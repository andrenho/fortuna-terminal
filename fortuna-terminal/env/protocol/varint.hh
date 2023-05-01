#ifndef VARINT_HH_
#define VARINT_HH_

#include <cstdint>
#include <span>
#include <vector>

// Convert to varint bytes.
//   - pars: list of integers to be converted
//   - returns: the array of bytes
std::vector<uint8_t> to_varint(std::vector<int> const& pars);

// Convert from varint bytes to a list of integers.
//   - array: the array of bytes to be converted
//   - number_of_ints_expected: the number of ints to extract from array
//   - returns
//      - the number of bytes used in the conversion, or 0 if there were not enough bytes
//      - the array of ints
std::pair<size_t, std::vector<int>> from_varint(std::span<const uint8_t> const& array, size_t number_of_ints_expected);

#endif //VARINT_HH_
