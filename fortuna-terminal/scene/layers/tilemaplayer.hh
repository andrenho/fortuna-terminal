#ifndef TILEMAPLAYER_HH_
#define TILEMAPLAYER_HH_

#include "imagelayer.hh"

#include <optional>
#include <unordered_map>

#define TILEMAP_MAX_W 512

struct TilePos {
    size_t x, y;

    bool operator==(TilePos const& o) const { return x == o.x && y == o.y; }
};

namespace std {
    template<> struct hash<TilePos> {
        size_t operator()(TilePos const& t) const { return t.y * TILEMAP_MAX_W + t.x; }
    };
}

class TilemapLayer : public ImageLayer {
public:
    [[nodiscard]] std::optional<size_t> tile_image(size_t x, size_t y) const {
        auto it = tilemap_.find({x, y}); return it == tilemap_.end() ? std::optional<size_t>() : it->second;
    };
    void set_tile_image(size_t x, size_t y, size_t image) { tilemap_[{x, y}] = image; }

    ssize_t pos_x = 0;
    ssize_t pos_y = 0;

    static constexpr size_t MAX_TILEMAP_IMAGES = 512;
    static constexpr size_t MAX_W = TILEMAP_MAX_W, MAX_H = 512;

private:
    std::unordered_map<TilePos, size_t> tilemap_ {};
};

#undef TILEMAP_MAX_W

#endif //TILEMAPLAYER_HH_