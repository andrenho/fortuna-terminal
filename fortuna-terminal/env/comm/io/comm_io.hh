#ifndef COMM_IO_HH_
#define COMM_IO_HH_

#include "env/comm/comm.hh"

#include <string>
#include <optional>
#include <vector>

class CommIO : public CommunicationModule {
public:
    [[nodiscard]] Channels channels() const override { return Channels::InputAndOutput; }

    virtual std::vector<uint8_t>   read_blocking(size_t n) = 0;
    virtual std::optional<uint8_t> read_blocking();

    virtual void                   write(std::vector<uint8_t> const& data) = 0;
    void                           write(std::string const& str);
    void                           write(uint8_t byte);
};

#endif //COMM_IO_HH_
