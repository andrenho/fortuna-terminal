#include "../fortuna-protocol.h"

#include <string.h>
#include <stdio.h>

#define WILDCARD 0xff

#define mu_assert(test) do { if (!(test)) return #test; } while (0)
#define mu_assert_buffer(...) do { \
    uint8_t b[] = { __VA_ARGS__ }; \
    if (buffer_len != sizeof(b)) { expected(); return #__VA_ARGS__; } \
    for (size_t i = 0; i < buffer_len; ++i) if (b[i] != WILDCARD && buffer[i] != b[i]) { expected(); return #__VA_ARGS__; } \
    } while (0)
#define mu_run_test(test) do { char *message = test(); \
                                if (message) return message; } while (0)

uint8_t buffer[1024];
size_t buffer_len = 0;

static void expected()
{
    for (size_t i = 0; i < buffer_len; ++i)
        printf("%02X ", buffer[i]);
    printf("\n");
}

int send(uint8_t const* data, size_t len)
{
    memcpy(buffer, data, len);
    buffer_len = len;

    return 0;
}

int recv(uint8_t* buf, size_t len)
{
    (void) len;
    memset(buf, FP_RESPONSE_OK, 3);
    return 0;
}

static char* single_message()
{
    FP_Command a = { .command = FP_TEXT_PRINT_CHAR, .chr = 'A' };
    int r = fp_send(&a, 1, send, recv);

    mu_assert(r == 0);
    mu_assert_buffer(FP_FRAME_START, 2, FP_TEXT_PRINT_CHAR, 'A', WILDCARD, FP_FRAME_END);

    return 0;
}

static char *hello()
{
    FP_Command a = { .command = FP_TEXT_PRINT_TEXT };
    strcpy((char *) a.text, "Hello");

    int r = fp_send(&a, 1, send, recv);
    mu_assert(r == 0);
    mu_assert_buffer(FP_FRAME_START, 7, FP_TEXT_PRINT_TEXT, 'H', 'e', 'l', 'l', 'o', 0, WILDCARD, FP_FRAME_END);

    return 0;
}

static char* three_messages()
{
    FP_Command a[] = {
            { .command = FP_TEXT_PRINT_CHAR, .chr = 'A' },
            { .command = FP_TEXT_PRINT_CHAR, .chr = 'B' },
            { .command = FP_TEXT_PRINT_CHAR, .chr = 'C' },
    };
    int r = fp_send(a, 3, send, recv);

    mu_assert(r == 0);
    mu_assert_buffer(FP_FRAME_START, 6,
                     FP_TEXT_PRINT_CHAR, 'A',
                     FP_TEXT_PRINT_CHAR, 'B',
                     FP_TEXT_PRINT_CHAR, 'C',
                     WILDCARD, FP_FRAME_END);

    return 0;
}

static char* short_and_long()
{
    FP_Command a[] = {
            { .command = FP_TEXT_PRINT_CHAR, .chr = 'A' },
            { .command = FP_TEXT_PRINT_TEXT },
    };
    strcpy((char *) a[1].text, "0123456789012345678901234567890123456789012");
    int r = fp_send(a, 2, send, recv);

    mu_assert(r == 0);
    mu_assert_buffer(
            FP_FRAME_START, 2,
            FP_TEXT_PRINT_CHAR, 'A',
            WILDCARD, FP_FRAME_END,
            FP_FRAME_START, 41,
            FP_TEXT_PRINT_TEXT,
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '0', '1', '2', 0,
            WILDCARD, FP_FRAME_END
    );

    return 0;
}

static char* long_and_short()
{
    FP_Command a[] = {
            { .command = FP_TEXT_PRINT_TEXT },
            { .command = FP_TEXT_PRINT_CHAR, .chr = 'A' },
    };
    strcpy((char *) a[0].text, "0123456789012345678901234567890123456789012");
    int r = fp_send(a, 2, send, recv);

    mu_assert(r == 0);
    mu_assert_buffer(
            FP_FRAME_START, 39,
            FP_TEXT_PRINT_TEXT,
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                '0', '1', '2', 0,
            WILDCARD, FP_FRAME_END,
            FP_FRAME_START, 2,
            FP_TEXT_PRINT_CHAR, 'A',
            WILDCARD, FP_FRAME_END
    );

    return 0;
}

static char * all_tests()
{
    mu_run_test(single_message);
    mu_run_test(hello);
    mu_run_test(three_messages);
    // mu_run_test(short_and_long);
    // mu_run_test(long_and_short);
    return 0;
}

int main()
{
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }

    return result != 0;
}