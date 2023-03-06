#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>

#define BUFFER_SZ 256

extern uint8_t  out_buffer[BUFFER_SZ];
extern uint16_t out_buffer_sz;
extern uint8_t  in_buffer[BUFFER_SZ];
extern uint16_t in_buffer_sz;

#endif
