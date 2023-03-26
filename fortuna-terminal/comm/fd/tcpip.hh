#ifndef TCPIP_HH_
#define TCPIP_HH_

#include "commfd.hh"
#include "application/options.hh"

#ifndef _WIN32
#define SOCKET int
#else
#include <winsock2.h>
#endif

class TCPIP : public CommFileDescriptor {
public:
    TCPIP(TcpIpOptions const& options, size_t readbuf_sz);

    [[nodiscard]] std::string description() const override;

    void vsync() override;

protected:
    std::string read() override;
    void write(std::string_view data_to_send) override;

private:
    SOCKET sock_fd = 0;
    int port_;
    std::string listen_address_;

    std::string find_listen_ipv4_addr();
};

#endif //TCPIP_HH_
