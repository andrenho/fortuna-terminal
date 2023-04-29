#include <cstddef>
#include <cstdlib>
#include <vector>

#include "../env/protocol/varint.hh"

#define FAIL() {exit(EXIT_FAILURE);}

static void validate_varint(std::vector<int> const &values, size_t expected_count)
{
    std::string str = to_varint(values);
    auto [count, result] = from_varint(str, values.size());
    if (values.size() != result.size() || !std::equal(values.begin(), values.end(), result.begin()))
        FAIL();
    if (count != expected_count)
        FAIL();
}

int main()
{
    validate_varint({0}, 1);

    return EXIT_SUCCESS;
}