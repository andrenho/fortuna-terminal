#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <span>
#include <vector>

#include "../env/protocol/varint.hh"

#define FAIL() {exit(EXIT_FAILURE);}
#define ASSERT(a) {if(!(a))FAIL()}

static void validate_varint(std::vector<int> const &values, size_t expected_count)
{
    std::vector<uint8_t> str = to_varint(values);
    auto [count, result] = from_varint(str, values.size());
    if (values.size() != result.size() || !std::equal(values.begin(), values.end(), result.begin()))
        FAIL();
    if (count != expected_count)
        FAIL();
}

static void compare_from_varint(std::vector<uint8_t> const& bytes, std::vector<int> const& expected)
{
    auto [count, result] = from_varint(std::span<const uint8_t> { bytes }, expected.size());
    ASSERT(count == bytes.size())
    ASSERT(result.size() == expected.size())
    ASSERT(std::equal(result.begin(), result.end(), expected.begin()))
}

static void test_varint()
{
    // varint
    validate_varint({0}, 1);
    validate_varint({200}, 2);
    validate_varint({10000}, 2);
    validate_varint({-2}, 2);
    validate_varint({-200}, 2);
    validate_varint({-10000}, 2);
    {
        try {
            validate_varint({ 20000 }, 2);
            FAIL()
        } catch (std::out_of_range&) {}
    }

    // RLE
    compare_from_varint({ 0xff, 0x05, 0x12 }, { 0x12, 0x12, 0x12, 0x12, 0x12 });
    {
        auto req = to_varint({ 300, 400 });  // 300 times the number 400
        req.insert(req.begin(), 0xff);
        std::vector<int> expected(300, 400);
        compare_from_varint(req, expected);
    }

    // incomplete request
    {
        auto [count, _] = from_varint(std::span<const uint8_t> { { 0xf0 } }, 1);
        ASSERT(count == 0)
    }
    {
        auto [count, _] = from_varint(std::span<const uint8_t> { { 0x12 } }, 2);
        ASSERT(count == 0)
    }
    {
        auto [count, _] = from_varint(std::span<const uint8_t> { { 0xff } }, 5);
        ASSERT(count == 0)
    }
    {
        auto [count, _] = from_varint(std::span<const uint8_t> { { 0xff, 0x5 } }, 5);
        ASSERT(count == 0)
    }
    {
        auto [count, _] = from_varint(std::span<const uint8_t> { { 0xff, 0x5, 0x5 } }, 6);
        ASSERT(count == 0)
    }

    // TODO: incomplete request

}

int main()
{
    test_varint();

    return EXIT_SUCCESS;
}