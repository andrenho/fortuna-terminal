#ifndef PTY_H_
#define PTY_H_

#include <stdint.h>
#include <stddef.h>

#include "../options/options.h"

FT_Result pty_init(size_t lines, size_t columns);
FT_Result pty_recv(uint8_t* byte, bool* data_received);
FT_Result pty_send(const uint8_t* data, size_t sz);
void      pty_finalize();

#endif //PTY_H_
