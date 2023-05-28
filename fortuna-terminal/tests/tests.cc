#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <span>
#include <vector>

#include "../env/protocol/varint.hh"
#include "../scene/scene.hh"
#include "../env/protocol/fortuna.hh"
#include "../env/protocol/fortuna_commands.hh"
#include "../application/control.hh"
#include "../application/debug.hh"

#define FAIL() {exit(EXIT_FAILURE);}
#define ASSERT(a) {if(!(a))FAIL()}

static void validate_varint(std::vector<int> const &values, size_t expected_n_bytes)
{
    std::vector<uint8_t> str = to_varint(values);
    auto [n_bytes, result] = from_varint(str, values.size());
    if (values.size() != result.size() || !std::equal(values.begin(), values.end(), result.begin()))
        FAIL();
    if (n_bytes != expected_n_bytes)
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

    // incomplete requests
    try {
        from_varint(std::span<const uint8_t> { { 0xf0 } }, 1);
        FAIL();
    } catch (VarintInputTooShortException&) {}
    try {
        from_varint(std::span<const uint8_t> { { 0x12 } }, 2);
        FAIL();
    } catch (VarintInputTooShortException&) {}
    try {
        from_varint(std::span<const uint8_t> { { 0xff } }, 5);
        FAIL();
    } catch (VarintInputTooShortException&) {}
    try {
        from_varint(std::span<const uint8_t> { { 0xff, 0x5 } }, 5);
        FAIL();
    } catch (VarintInputTooShortException&) {}
    try {
        from_varint(std::span<const uint8_t> { { 0xff, 0x5, 0x5 } }, 6);
        FAIL();
    } catch (VarintInputTooShortException&) {}

    // multiple requests in the same call
    validate_varint({20, -2000}, 3);
    compare_from_varint({ 0x38, 0xff, 0x5, 0x12, 0x1a }, { 0x38, 0x12, 0x12, 0x12, 0x12, 0x12, 0x1a });
    try {
        from_varint(std::span<const uint8_t> { { 0x38, 0xff, 0x5 } }, 6);
        FAIL();
    } catch (VarintInputTooShortException&) {}
}

static void test_fortuna_protocol()
{
#if 0
    // test full command
    {
        Scene scene; FortunaProtocol fp(scene);
        fp.process_inputs(to_varint({ I_RESET_TERMINAL }));
        ASSERT(control_queue.pop_nonblock().value().command == ControlCommand::ResetProtocol);
    }

    // test commands with parameters
    {
        Scene scene; FortunaProtocol fp(scene);
        fp.process_inputs(to_varint({ I_CHANGE_PALETTE, 1, 255, 255, 128 }));
        ASSERT(scene.palette[1].r == 255);
        ASSERT(scene.palette[1].g == 255);
        ASSERT(scene.palette[1].b == 128);
    }

    // test multiple commands
    {
        Scene scene; FortunaProtocol fp(scene);
        fp.process_inputs(to_varint({ I_RESET_TERMINAL, I_CHANGE_PALETTE, 1, 255, 255, 128 }));
        ASSERT(control_queue.pop_nonblock().value().command == ControlCommand::ResetProtocol);
        ASSERT(scene.palette[1].r == 255);
        ASSERT(scene.palette[1].g == 255);
        ASSERT(scene.palette[1].b == 128);
    }

    // test broken commands
    {
        Scene scene; FortunaProtocol fp(scene);
        uint8_t initial_r = scene.palette[1].r;

        fp.process_inputs(to_varint({ I_RESET_TERMINAL, I_CHANGE_PALETTE, 1, 255, 255 }));
        ASSERT(control_queue.pop_nonblock().value().command == ControlCommand::ResetProtocol);
        ASSERT(scene.palette[1].r == initial_r);

        fp.process_inputs(to_varint({ 128 }));
        ASSERT(scene.palette[1].r == 255);
        ASSERT(scene.palette[1].g == 255);
        ASSERT(scene.palette[1].b == 128);
    }

    {
        Scene scene; FortunaProtocol fp(scene);
        uint8_t initial_r = scene.palette[1].r;

        fp.process_inputs(to_varint({ I_CHANGE_PALETTE, 1 }));
        ASSERT(scene.palette[1].r == initial_r);
        fp.process_inputs(to_varint({ 255 }));
        ASSERT(scene.palette[1].r == initial_r);
        fp.process_inputs(to_varint({ 255, 128 }));
        ASSERT(scene.palette[1].r == 255);
    }

    // test very long commands
    {
        std::vector<int> values { I_UPLOAD_IMAGE, 4, 16 };
        std::vector<uint8_t> expected;
        for (size_t i = 0; i < 256; ++i) {
            values.push_back(i);
            expected.push_back(i);
        }

        Scene scene; FortunaProtocol fp(scene);
        fp.process_inputs(to_varint(values));
        ASSERT(scene.image(4).transparent_color == 16);
        ASSERT(std::equal(expected.begin(), expected.end(), scene.image(4).pixels));
    }

    // test end of frame
    {
        std::vector<uint8_t> request { I_RESET_TERMINAL, 0x54, 0xEE, 0xC2, 0x28 };
        auto change_palette = to_varint({ I_CHANGE_PALETTE, 1, 255, 255, 128 });
        request.insert(request.end(), change_palette.begin(), change_palette.end());

        Scene scene; FortunaProtocol fp(scene);

        fp.process_inputs(request);
        ASSERT(control_queue.pop_nonblock().value().command == ControlCommand::ResetProtocol);
        ASSERT(scene.palette[1].r == 255);
    }

    // test end of frame with incomplete request
    {
        std::vector<uint8_t> request { I_CHANGE_PALETTE, 1, 0x54, 0xEE, 0xC2, 0x28 };
        auto change_palette = to_varint({ I_CHANGE_PALETTE, 1, 255, 255, 128 });
        request.insert(request.end(), change_palette.begin(), change_palette.end());

        Scene scene; FortunaProtocol fp(scene);

        fp.process_inputs(request);
        ASSERT(scene.palette[1].r == 255);
    }
#endif

    // test message responses
    {
        std::vector<int> upload_image;
        upload_image.push_back(I_UPLOAD_IMAGE);
        upload_image.push_back(0);
        upload_image.push_back(-1);
        for (size_t i = 0; i < 256; ++i)
            upload_image.push_back(i / 2 + 1);

        Scene scene; FortunaProtocol fp(scene);
        fp.process_inputs(to_varint(upload_image));

        auto [n, response] = from_varint(fp.get_lastest_fortuna_outputs(), 3);
        ASSERT(n == 3);
        ASSERT(response[0] == E_IMAGE_CHECKSUM);
        ASSERT(response[1] == 0x3f);
        ASSERT(response[2] == 0x74);
    }

    // TODO - test events
}

int main()
{
    Debug::initialize(DebugVerbosity::V_NORMAL);

    test_varint();
    test_fortuna_protocol();

    return EXIT_SUCCESS;
}