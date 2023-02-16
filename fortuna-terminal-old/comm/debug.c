#include "debug.h"

#include <stdio.h>

FT_Result debug_init()
{
    return FT_OK;
}

FT_Result debug_recv(uint8_t* byte, bool* data_received)
{
    int value;
    int r = scanf("%x", &value);
    if (r == 1) {
        *data_received = true;
        *byte = (uint8_t) value;
    }
    return FT_OK;
}

FT_Result debug_send(const uint8_t* data, size_t sz)
{
    for (size_t i = 0; i < sz; ++i)
        printf("%02X ", data[i]);
    printf("\n");
    return FT_OK;
}

void debug_finalize()
{
}