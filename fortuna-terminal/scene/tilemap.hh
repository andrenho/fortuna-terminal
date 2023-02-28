#ifndef TILEMAP_HH_
#define TILEMAP_HH_

#include <cstddef>
#include <vector>

struct Tilemap {
    std::vector<size_t> tilemap {};

    size_t w = 0;
    size_t h = 0;

    static constexpr size_t MAX_TILEMAPS = 64;
    static constexpr size_t MAX_W = 512, MAX_H = 512;
};

#endif //TILEMAP_HH_
