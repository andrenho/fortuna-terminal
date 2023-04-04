#ifndef FTCLIENT_TCPIP_H_H_
#define FTCLIENT_TCPIP_H_H_

#include <ftclient.h>

int ftclient_tcpip_init(FTClient* ft_client, const char* address, int port, char* error, size_t err_sz);

#endif //FTCLIENT_TCPIP_H_H_
