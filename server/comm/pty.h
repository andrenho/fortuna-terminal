#ifndef PTY_H_
#define PTY_H_

#include <stdint.h>
#include <stddef.h>

#include "../options/options.h"

int pty_init(PTYOptions* pty_options, size_t lines, size_t columns);
int pty_recv(uint8_t* byte);
int pty_send(const uint8_t* data, size_t sz);
int pty_finalize();

#endif //PTY_H_
