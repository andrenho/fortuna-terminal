#include "echo.h"
#include "error/error.h"

int echo_init()
{
    // TODO
    return 0;
}

int echo_recv(uint8_t* byte)
{
    (void) byte;
    // TODO
    return ERR_NO_DATA;
}

int echo_send(const uint8_t* data, size_t sz)
{
    (void) data; (void) sz;
    // TODO
    return 0;
}

int echo_finalize()
{
    // TODO
    return 0;
}