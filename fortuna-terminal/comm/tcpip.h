#ifndef TCPIP_H_
#define TCPIP_H_

#include <stdint.h>
#include <stddef.h>

#include "../options/options.h"

FT_Result tcpip_init();
FT_Result tcpip_recv(uint8_t* byte, bool* data_received);
FT_Result tcpip_send(const uint8_t* data, size_t sz);
void      tcpip_finalize();

#endif //TCPIP_H_
