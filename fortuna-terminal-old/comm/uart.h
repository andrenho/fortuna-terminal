#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

#include "../options/options.h"

FT_Result uart_init();
FT_Result uart_recv(uint8_t* byte, bool* data_received);
FT_Result uart_send(const uint8_t* data, size_t sz);
void      uart_finalize();

#endif //UART_H_
