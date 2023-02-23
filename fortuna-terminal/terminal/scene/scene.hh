#ifndef SCENE_HH_
#define SCENE_HH_

#include "layers/text.hh"
#include "../../common/noncopyable.hh"
#include "common/geometry.hh"
#include "protocol/mode.hh"

struct Scene {
    explicit Scene(Mode mode);

    Text text;

    [[nodiscard]] Size terminal_size() const;

    void reset();

    void set_mode(Mode mode);

    [[nodiscard]] Mode mode() const { return mode_; }

private:
    Mode mode_;
};

#endif //SCENE_HH_
