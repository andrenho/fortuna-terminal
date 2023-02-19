#ifndef PTY_HH_
#define PTY_HH_

#include "../options.hh"
#include "../common/geometry.hh"
#include "fdcomm.hh"

class PTY : public FDComm {
public:
    explicit PTY(PTYOptions const& pty_options, Size terminal_size);

    std::vector<uint8_t> read_blocking(size_t n) override;

    void client_disconnected() override;

};

#endif //PTY_HH_
