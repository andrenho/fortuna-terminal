#ifndef FDCOMM_HH_
#define FDCOMM_HH_

#include "comm.hh"

class FDComm : public CommunicationModule {
public:
    ~FDComm() override;

    std::vector<uint8_t> read_blocking(size_t n) override;
    void                 write(std::vector<uint8_t> const &data) override;

    virtual void         on_rw_zero();
    virtual void         on_read_zero() { on_rw_zero(); }
    virtual void         on_read_error(std::string const& error);
    virtual void         on_write_zero() { on_rw_zero(); }
    virtual void         on_write_error(std::string const& error);

    virtual void         client_disconnected();

protected:
    FDComm() = default;

    int fd_ = -1;
    int write_fd_ = -1;
};

#endif //FDCOMM_HH_
