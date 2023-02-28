#ifndef SCENE_HH_
#define SCENE_HH_

#include "scene/layers/textlayer.hh"
#include "common/noncopyable.hh"
#include "common/geometry.hh"
#include "protocol/mode.hh"
#include "../common/noncopyable.hh"
#include "scene/layers/spritelayer.hh"
#include "scene/layers/tilemaplayer.hh"

struct Scene : NonCopyable {
    explicit Scene(Mode mode);

    TilemapLayer tilemap_background, tilemap_obstacles;
    SpriteLayer  sprites;
    TilemapLayer tilemap_foreground, tilemap_ui;
    TextLayer    text;

    void reset();
    void set_mode(Mode mode);

    [[nodiscard]] Mode mode() const { return mode_; }
    [[nodiscard]] Size terminal_size() const;

    size_t texture_image_index() const { return texture_image_index_; }

    static size_t constexpr MAX_IMAGES = 1024;

private:
    Mode mode_;
    size_t texture_image_index_;

    static size_t image_texture_idx_counter;
};

#endif //SCENE_HH_
