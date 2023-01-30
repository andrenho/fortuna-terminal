#include "echo.hh"

#include <cstdio>

void Echo::run(Protocol* protocol)
{
    while (running_) {
        int c = std::getchar();
        protocol->input((uint8_t) c);
    }
}
