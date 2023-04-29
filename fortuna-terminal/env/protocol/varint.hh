#ifndef VARINT_HH_
#define VARINT_HH_

#include <string>
#include <string_view>
#include <vector>

std::string to_varint(std::vector<int> const& pars);
std::pair<size_t, std::vector<int>> from_varint(std::string_view str, size_t count);

#endif //VARINT_HH_
