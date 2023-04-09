#ifndef TCPIPUNIX_HH_
#define TCPIPUNIX_HH_

#include "tcpip.hh"

class TCPIP_Unix : public TCPIP {
public:
    TCPIP_Unix(TcpIpOptions const& options, size_t readbuf_sz) : TCPIP(options, readbuf_sz) {}

protected:
    std::string find_listen_ipv4_addr() override;
    void        configure_socket(SOCKET fd) override;
    void        close_socket(SOCKET fd) override;
    bool        error_is_try_again() const override;
    int         write_flags() const override;
};

#endif //TCPIPUNIX_HH_
