#ifndef PROTOCOL_DEBUG_H_
#define PROTOCOL_DEBUG_H_

#include <stdint.h>

void debug_char(uint8_t c);
void debug_special_0(const char* str);
void debug_special_1(const char* str, int p1);
void debug_special_2(const char* str, int p1, int p2);

#endif //PROTOCOL_DEBUG_H_
