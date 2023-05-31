#ifndef ECHONUMERIC_HH_
#define ECHONUMERIC_HH_

#include "../comm.hh"
#include "common/iterativethread.hh"

class EchoNumeric : public CommunicationModule {
public:
    EchoNumeric();

    [[nodiscard]] std::string description() const override { return "EchoNumeric"; }
    [[nodiscard]] virtual std::vector<uint8_t> exchange(std::vector<uint8_t> data_to_send) override;

private:
    IterativeThread thread_;
    SyncQueue<int>  numbers_;
};

#endif //ECHONUMERIC_HH_