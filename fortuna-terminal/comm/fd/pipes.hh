#ifndef PIPES_HH_
#define PIPES_HH_

#include "commfd.hh"

class Pipes : public CommFileDescriptor {
public:
    explicit Pipes(size_t readbuf_sz);

    void vsync() override;

    [[nodiscard]] std::string description() const override { return "Pipes (stdin/stdout)"; }
};

#endif //PIPES_HH_
