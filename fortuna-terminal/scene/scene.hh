#ifndef SCENE_HH_
#define SCENE_HH_

#include "common/types/noncopyable.hh"
#include "common/enums/mode.hh"
#include "common/types/noncopyable.hh"
#include "scene/layers/textlayer.hh"
#include "scene/layers/spritelayer.hh"
#include "scene/layers/tilemaplayer.hh"
#include "tilemap.hh"
#include "scene/palette/palette.hh"
#include "scene/layers/images.hh"

#include <optional>
#include <unordered_map>

enum LayerIdentifier : uint8_t {
    LAYER_DEEP_BG           =  0,
    LAYER_TILEMAP_BG        =  4,
    LAYER_TILEMAP_OBSTACLES =  8,
    LAYER_SPRITES           = 12,
    LAYER_TILEMAP_FG        = 16,
    LAYER_TILEMAP_UI        = 20,
    LAYER_TILEMAP_TEXT      = 24,
};

struct Scene : NonCopyable {
    explicit Scene(Mode mode);

    Tilemap tilemaps[Tilemap::MAX_TILEMAPS] {};
    Images  images;
    Palette palette {};

    uint8_t bg_color = COLOR_BLACK;

    void reset();
    void set_mode(Mode mode);

    [[nodiscard]] TextLayer&          text() const;
    [[nodiscard]] SpriteLayer&        sprites() const;
    [[nodiscard]] TilemapLayer*       tilemap_layer(LayerIdentifier layer_id) const;
    [[nodiscard]] Layer*              layer(LayerIdentifier id) const;

    [[nodiscard]] ImageLayer const*   image_layer_unsafe(LayerIdentifier layer_id) const;

    [[nodiscard]] Mode                mode() const { return mode_; }
    [[nodiscard]] size_t              unique_id() const { return unique_id_; }

    [[nodiscard]] std::pair<int, int> size_in_pixels() const { return size_in_pixels_; }

    static size_t constexpr MAX_IMAGES = 1024;

private:
    Mode                mode_;
    std::pair<int, int> size_in_pixels_;
    const size_t        unique_id_;

    std::unordered_map<LayerIdentifier, std::unique_ptr<Layer>> layers_ {};

    static size_t unique_id_counter;
};

#endif //SCENE_HH_
