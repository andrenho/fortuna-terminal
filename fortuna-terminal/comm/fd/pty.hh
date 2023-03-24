#ifndef PTY_HH_
#define PTY_HH_

#include "application/options.hh"
#include "commfd.hh"

class PTY : public CommFileDescriptor {
public:
    PTY(PTYOptions const& pty_options, size_t readbuf_sz);

    [[nodiscard]] std::string description() const override;

protected:
    void on_read_error() override;
    void client_disconnected() override;

private:
    std::string shell_;
    std::string name_;
};

#endif //PTY_HH_
