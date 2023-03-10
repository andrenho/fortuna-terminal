#ifndef FDCOMM_HH_
#define FDCOMM_HH_

#include "env/comm/io/comm_io.hh"

class FDComm : public CommIO {
public:
    ~FDComm() override;

    std::vector<uint8_t> read_blocking(size_t n) override;
    void                 write(std::vector<uint8_t> const &data) override;

    // these can be overwritten at will
    virtual void         on_rw_zero();
    virtual void         on_read_zero() { on_rw_zero(); }
    virtual void         on_read_error(std::string const& error);
    virtual void         on_write_zero() { on_rw_zero(); }
    virtual void         on_write_error(std::string const& error);

    virtual void         client_disconnected();

protected:
    FDComm() = default;

    int fd_ = INVALID_FD;
    int write_fd_ = INVALID_FD;   // writes will point to fd_ if unset

    static constexpr int INVALID_FD = -1;
};

#endif //FDCOMM_HH_
