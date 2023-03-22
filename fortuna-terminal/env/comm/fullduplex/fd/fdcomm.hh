#ifndef FDCOMM_HH_
#define FDCOMM_HH_

#include "env/comm/fullduplex/comm_fullduplex.hh"

class FDComm : public CommFullDuplex {
public:
    ~FDComm() override;

protected:
    explicit FDComm(size_t readbuf_sz) : readbuf_sz_(readbuf_sz) {}

private:
    std::vector<uint8_t> read_for(Duration duration) override;

protected:
    void                 write(std::vector<uint8_t> const &data) override;

    // these can be overwritten at will
    virtual void         on_rw_zero();
    virtual void         on_read_zero() { on_rw_zero(); }
    virtual void         on_read_error(std::string const& error);
    virtual void         on_write_zero() { on_rw_zero(); }
    virtual void         on_write_error(std::string const& error);

    virtual void         client_disconnected();

    size_t readbuf_sz_;

    int fd_ = INVALID_FD;
    int write_fd_ = INVALID_FD;   // writes will point to fd_ if unset

    static constexpr int INVALID_FD = -1;
};

#endif //FDCOMM_HH_
