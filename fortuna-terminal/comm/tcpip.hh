#ifndef TCPIP_HH_
#define TCPIP_HH_

#include "fdcomm.hh"
#include "../options.hh"

#ifndef _WIN32
#define SOCKET int
#else
#include <winsock2.h>
#endif

class TCPIP : public FDComm {
public:
    explicit TCPIP(TcpIpOptions const& options);

    void action_on_rw_zero() override;

private:
    SOCKET sock_fd = 0;
};

#endif //TCPIP_HH_
