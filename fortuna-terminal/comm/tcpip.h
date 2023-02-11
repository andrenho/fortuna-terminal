#ifndef TCPIP_H_
#define TCPIP_H_

#include <stdint.h>
#include <stddef.h>

#include "../options/options.h"

int tcpip_init();
int tcpip_recv(uint8_t* byte);
int tcpip_send(const uint8_t* data, size_t sz);
int tcpip_finalize();

#endif //TCPIP_H_
