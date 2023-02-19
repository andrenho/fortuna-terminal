#ifndef PTY_HH_
#define PTY_HH_

#include "../options.hh"
#include "../common/geometry.hh"
#include "fdcomm.hh"

class PTY : public FDComm {
public:
    explicit PTY(PTYOptions const& pty_options, Size terminal_size);
    ~PTY() override;

    void action_on_rw_zero() override;

private:
    void client_disconnected();
};

#endif //PTY_HH_
