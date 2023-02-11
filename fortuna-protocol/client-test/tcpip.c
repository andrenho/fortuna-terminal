#include "../fortuna-protocol.h"

#include <stdio.h>
#include <string.h>

int send(uint8_t const* data, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        printf("%02X ", data[i]);
    printf("\n");
    return 0;
}

int recv(uint8_t* buf, size_t len)
{
    memset(buf, FP_RESPONSE_OK, 3);
    return 0;
}

int main()
{
    FP_Command a[] = {
        {
            .command = FP_TEXT_PRINT_CHAR,
            .chr = 'A',
        }, {
            .command = FP_TEXT_PRINT_TEXT,
        }
    };
    strcpy((char *) a[1].text, "Hello");
    fp_send(a, 2, send, recv);

    return 0;
}