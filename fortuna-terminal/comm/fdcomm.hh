#ifndef FDCOMM_HH_
#define FDCOMM_HH_

#include "comm.hh"

class FDComm : public CommunicationModule {
public:
    std::vector<uint8_t> read_blocking(size_t n) override;
    std::vector<uint8_t> read_for(std::chrono::duration<double> duration) override;
    void write(std::vector<uint8_t> const &data) override;

    virtual void action_on_rw_zero() {}

protected:
    FDComm() = default;

    int fd_ = 0;
};

#endif //FDCOMM_HH_
