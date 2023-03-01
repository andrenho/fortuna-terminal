#ifndef SCENE_HH_
#define SCENE_HH_

#include "scene/layers/textlayer.hh"
#include "common/noncopyable.hh"
#include "protocol/mode.hh"
#include "../common/noncopyable.hh"
#include "scene/layers/spritelayer.hh"
#include "scene/layers/tilemaplayer.hh"
#include "tilemap.hh"
#include "palette/palette.hh"
#include "scene/layers/images.hh"

#include <optional>
#include <unordered_map>

enum LayerIdentifier {
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

    Tilemap tilemap[Tilemap::MAX_TILEMAPS] {};
    Images  images;
    Palette palette;

    uint8_t bg_color = COLOR_BLACK;

    TextLayer&         text() { return *reinterpret_cast<TextLayer*>(layers_.at(LAYER_TILEMAP_TEXT).get()); }
    TextLayer const&   text() const { return *reinterpret_cast<TextLayer const*>(layers_.at(LAYER_TILEMAP_TEXT).get()); }
    SpriteLayer&       sprites() { return *reinterpret_cast<SpriteLayer*>(layers_.at(LAYER_SPRITES).get()); }
    SpriteLayer const& sprites() const { return *reinterpret_cast<SpriteLayer const*>(layers_.at(LAYER_SPRITES).get()); }

    ImageLayer const*  image_layer_unsafe(LayerIdentifier layer_id) const { return reinterpret_cast<ImageLayer const *>(layers_.at(layer_id).get()); }
    ImageLayer*        image_layer_unsafe(LayerIdentifier layer_id) { return const_cast<ImageLayer *>(const_cast<Scene const *>(this)->image_layer_unsafe(layer_id)); }

    TilemapLayer const* tilemap_layer(LayerIdentifier layer_id) const { return dynamic_cast<TilemapLayer const *>(layers_.at(layer_id).get()); }
    TilemapLayer*       tilemap_layer(LayerIdentifier layer_id) { return const_cast<TilemapLayer *>(const_cast<Scene const *>(this)->tilemap_layer(layer_id)); }

    Layer*              layer(LayerIdentifier id) { return layers_.at(id).get(); }

    void reset();
    void set_mode(Mode mode);

    [[nodiscard]] Mode mode() const { return mode_; }

    [[nodiscard]] size_t texture_image_index() const { return texture_image_index_; }

    std::pair<int, int> size_in_pixels() const;

    static size_t constexpr MAX_IMAGES = 1024;

private:
    Mode mode_;
    size_t texture_image_index_;

    std::unordered_map<LayerIdentifier, std::unique_ptr<Layer>> layers_ {};

    static size_t image_texture_idx_counter;
};

#endif //SCENE_HH_
