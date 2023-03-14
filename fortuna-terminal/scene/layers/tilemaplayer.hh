#ifndef TILEMAPLAYER_HH_
#define TILEMAPLAYER_HH_

#include "imagelayer.hh"

struct TilemapLayer : public ImageLayer {
    ssize_t pos_x = 0;
    ssize_t pos_y = 0;
    ssize_t map = -1;

    [[nodiscard]] std::vector<ImageToDraw> images_to_draw_next_frame(class Scene const& scene) const override;

    void reset() override;
};

#endif //TILEMAPLAYER_HH_