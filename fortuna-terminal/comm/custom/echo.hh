#ifndef ECHO_HH_
#define ECHO_HH_

#include <string_view>
#include "comm/comm.hh"

class Echo : public CommunicationModule {
public:
    [[nodiscard]] std::string description() const override { return "Echo"; }
    std::string exchange(std::string_view data_to_send) override;
};

#endif //ECHO_HH_