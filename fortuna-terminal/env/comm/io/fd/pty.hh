#ifndef PTY_HH_
#define PTY_HH_

#include "application/options.hh"
#include "fdcomm.hh"

class PTY : public FDComm {
public:
    explicit PTY(PTYOptions const& pty_options);

    std::vector<uint8_t> read_blocking(size_t n) override;

    void write(std::vector<uint8_t> const &data) override;

    void client_disconnected() override;

};

#endif //PTY_HH_
