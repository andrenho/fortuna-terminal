#ifndef TCPIP_HH_
#define TCPIP_HH_

#include "comm_module.hh"

#if _WIN32
#include <winsock.h>
#endif

class TCPIP : public CommunicationModule {
public:
    void initialize() override;

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

    void finalize() override;

private:
#ifndef _WIN32
#define SOCKET int
#endif
    SOCKET sock_fd = 0;
    SOCKET client_fd = 0;

    static const int BACKLOG = 1;

    bool client_connected = false;
};

#endif //TCPIP_HH_
