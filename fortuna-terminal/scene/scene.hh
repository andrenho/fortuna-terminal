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

    size_t sprite_image_texture_idx;
    size_t tile_image_texture_idx;

    void reset();
    void set_mode(Mode mode);

    [[nodiscard]] Mode mode() const { return mode_; }
    [[nodiscard]] Size terminal_size() const;

private:
    Mode mode_;

    static size_t image_texture_idx_counter;
};

#endif //SCENE_HH_
