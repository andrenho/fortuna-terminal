#ifndef FDCOMM_HH_
#define FDCOMM_HH_

#include "comm.hh"

class FDComm : public CommunicationModule {
public:
    virtual ~FDComm() override;

    std::vector<uint8_t> read_blocking(size_t n) override;
    void                 write(std::vector<uint8_t> const &data) override;

    void                 release_locks() override;

    virtual void         action_on_rw_zero() = 0;

protected:
    FDComm() = default;

    int fd_ = 0;
    size_t last_wait_ = 0;
};

#endif //FDCOMM_HH_
