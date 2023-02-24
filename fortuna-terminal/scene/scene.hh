#ifndef SCENE_HH_
#define SCENE_HH_

#include "scene/layers/textlayer.hh"
#include "common/noncopyable.hh"
#include "common/geometry.hh"
#include "protocol/mode.hh"
#include "../common/noncopyable.hh"
#include "scene/layers/spritelayer.hh"

struct Scene : NonCopyable {
    explicit Scene(Mode mode);

    TextLayer   text;
    SpriteLayer sprites;

    [[nodiscard]] Size terminal_size() const;

    void reset();

    void set_mode(Mode mode);

    [[nodiscard]] Mode mode() const { return mode_; }

private:
    Mode mode_;
};

#endif //SCENE_HH_
