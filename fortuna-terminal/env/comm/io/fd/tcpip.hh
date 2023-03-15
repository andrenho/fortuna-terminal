#ifndef TCPIP_HH_
#define TCPIP_HH_

#include "fdcomm.hh"
#include "application/options.hh"

#ifndef _WIN32
#define SOCKET int
#else
#include <winsock2.h>
#endif

class TCPIP : public FDComm {
public:
    explicit TCPIP(TcpIpOptions const& options);

    std::vector<uint8_t> read_blocking(size_t n) override;

    void write(std::vector<uint8_t> const &data) override;

private:
    SOCKET sock_fd = 0;
};

#endif //TCPIP_HH_
