#ifndef TILEMAPLAYER_HH_
#define TILEMAPLAYER_HH_

#include "imagelayer.hh"

struct TilemapLayer : public ImageLayer {
    ssize_t pos_x = 0;
    ssize_t pos_y = 0;
    ssize_t map = 0;
};

#endif //TILEMAPLAYER_HH_