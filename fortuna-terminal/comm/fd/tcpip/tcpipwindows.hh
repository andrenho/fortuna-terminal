#ifndef TCPIPWINDOWS_HH_
#define TCPIPWINDOWS_HH_

#include "tcpip.hh"

class TCPIP_Windows : public TCPIP {
public:
    TCPIP_Windows(TcpIpOptions const& options, size_t readbuf_sz) : TCPIP(options, readbuf_sz) {}

protected:
    void        initialize_tcpip() override;
    std::string find_listen_ipv4_addr() override;
    void        configure_socket(SOCKET fd) override;
    void        close_socket(SOCKET fd) override;
    bool        error_is_try_again() const override;
    int         write_flags() const override;
};

#endif //TCPIPWINDOWS_HH_
