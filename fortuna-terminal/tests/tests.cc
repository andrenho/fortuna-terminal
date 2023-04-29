#include <cstddef>
#include <cstdlib>
#include <span>
#include <vector>

#include "../env/protocol/varint.hh"

#define FAIL() {exit(EXIT_FAILURE);}

static void validate_varint(std::vector<int> const &values, size_t expected_count)
{
    std::vector<uint8_t> str = to_varint(values);
    auto [count, result] = from_varint(std::span<uint8_t>{ str }, values.size());
    if (values.size() != result.size() || !std::equal(values.begin(), values.end(), result.begin()))
        FAIL();
    if (count != expected_count)
        FAIL();
}

int main()
{
    validate_varint({0}, 1);
    validate_varint({200}, 2);
    validate_varint({10000}, 2);
    validate_varint({-2}, 2);
    validate_varint({-200}, 2);
    validate_varint({-10000}, 2);

    // TODO: rle
    // TODO: incomplete request

    return EXIT_SUCCESS;
}