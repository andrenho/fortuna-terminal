#ifndef TCPIP_HH_
#define TCPIP_HH_

#include "comm/fd/commfd.hh"
#include "application/options.hh"

#ifndef _WIN32
#define SOCKET int
#else
#include <winsock2.h>
#endif

class TCPIP : public CommFileDescriptor {
public:
    void initialize();

    [[nodiscard]] std::string description() const override;

    void vsync() override;

protected:
    TCPIP(TcpIpOptions const& options, size_t readbuf_sz) : CommFileDescriptor(readbuf_sz), port_(options.port) {}

    virtual void        initialize_tcpip() {}
    virtual std::string find_listen_ipv4_addr() = 0;
    virtual void        configure_socket(SOCKET fd) = 0;
    virtual void        close_socket(SOCKET fd) = 0;
    virtual bool        error_is_try_again() const = 0;
    virtual int         write_flags() const = 0;

    std::vector<uint8_t> read() override;
    void write(std::vector<uint8_t> const& data_to_send) override;

private:
    SOCKET sock_fd = 0;
    int port_;
    std::string listen_address_;
};

#endif //TCPIP_HH_
