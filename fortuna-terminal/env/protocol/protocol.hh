#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include "ansi.hh"
#include "extra.hh"
#include "common/mode.hh"
#include "env/scene/scene.hh"
#include "common/duration.hh"

class Protocol {
public:
    explicit Protocol(Mode mode, Scene& scene);

    void execute_inputs();
    void show_error(std::exception const& e);

    void show_fps_counter(Duration duration);

private:
    ANSI  ansi_;
    Extra extra_;
};

#endif //PROTOCOL_HH_
