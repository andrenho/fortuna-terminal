#ifndef ECHO_H_
#define ECHO_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "error/error.h"

FT_Result echo_init();
FT_Result echo_recv(uint8_t* byte, bool* data_received);
FT_Result echo_send(const uint8_t* data, size_t sz);
void      echo_finalize();

#endif //ECHO_H_
