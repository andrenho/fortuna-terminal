#include <cstddef>
#include <cstdlib>
#include <span>
#include <vector>

#include "../env/protocol/varint.hh"

#define FAIL() {exit(EXIT_FAILURE);}

static void validate_varint(std::vector<int> const &values, size_t expected_count)
{
    std::vector<uint8_t> str = to_varint(values);
    auto [count, result] = from_varint(str, values.size());
    if (values.size() != result.size() || !std::equal(values.begin(), values.end(), result.begin()))
        FAIL();
    if (count != expected_count)
        FAIL();
}

static void compare_from_varint(std::vector<uint8_t> const& bytes, size_t expected_count, std::vector<int> const& expected)
{
    auto [count, result] = from_varint(std::span<const uint8_t> { bytes }, 5);
}

int main()
{
    // varint
    validate_varint({0}, 1);
    validate_varint({200}, 2);
    validate_varint({10000}, 2);
    validate_varint({-2}, 2);
    validate_varint({-200}, 2);
    validate_varint({-10000}, 2);

    // RLE
    compare_from_varint({ 0xff, 0x05, 0xab }, 5, { 0xab, 0xab, 0xab, 0xab, 0xab });

    // TODO: incomplete request

    return EXIT_SUCCESS;
}