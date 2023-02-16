#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "error/error.h"

FT_Result debug_init();
FT_Result debug_recv(uint8_t* byte, bool* data_received);
FT_Result debug_send(const uint8_t* data, size_t sz);
void      debug_finalize();

#endif