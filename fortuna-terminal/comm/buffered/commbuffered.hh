#ifndef COMMBUFFERED_HH_
#define COMMBUFFERED_HH_

#include <optional>
#include <queue>

#include "../comm.hh"
#include "common/iterativethread.hh"

class CommBuffered : public CommunicationModule {
public:
    explicit CommBuffered(size_t readbuf_sz): readbuf_sz_(readbuf_sz) {}

    [[nodiscard]] std::string exchange(std::string_view data_to_send) override;

protected:
    int                fd_ = INVALID_FILE;
    std::optional<int> write_fd_ {};

    virtual void        write(std::string_view data_to_send);
    virtual std::string read();

    virtual void client_disconnected();
    [[noreturn]] virtual void on_read_error();

private:
    static constexpr int INVALID_FILE = -1;
    size_t readbuf_sz_;
};

#endif //COMMBUFFERED_HH_
