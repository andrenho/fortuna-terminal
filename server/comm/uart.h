#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

#include "../options/options.h"

int uart_init(SerialOptions* serial_options);
int uart_recv(uint8_t* byte);
int uart_send(const uint8_t* data, size_t sz);
int uart_finalize();

#endif //UART_H_
