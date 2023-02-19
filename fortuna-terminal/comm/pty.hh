#ifndef PTY_HH_
#define PTY_HH_

#include "comm.hh"
#include "../options.hh"
#include "../common/geometry.hh"

class PTY : public CommunicationModule {
public:
    explicit PTY(PTYOptions const& pty_options, Size terminal_size);
    ~PTY();

    std::vector<uint8_t> read_blocking(size_t n) override;
    std::vector<uint8_t> read_for(std::chrono::duration<double> duration) override;
    void write(std::vector<uint8_t> const &data) override;

private:
    int fd_ = -1;

    void client_disconnected();
};

#endif //PTY_HH_
