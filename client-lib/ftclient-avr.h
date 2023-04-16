#ifndef FTCLIENT_AVR_H_
#define FTCLIENT_AVR_H_

#include <avr/pgmspace.h>

#include "ftclient.h"

int ft_print_P(FTClient* ft, PGM_P str);
int ft_printf_P(FTClient* ft, PGM_P fmt, ...);

#endif
