#ifndef CLIENT_H_
#define CLIENT_H_

#include <avr/pgmspace.h>

typedef enum { C_UART } CommType;

void c_init(CommType comm_type);

void c_putchar(char c);
void c_print_P(PGM_P pstr);
void c_printf_P(PGM_P pstr, ...);

char c_getchar_nonblock(void);
char c_getchar_block(void);

#endif
