#ifndef SPRITELAYER_HH_
#define SPRITELAYER_HH_

#include "imagelayer.hh"

#include <set>
#include <vector>
#include <utility>

struct Sprite {
    int      pos_x = 0, pos_y = 0;
    bool     visible = false;
    bool     mirrored_h = false;
    bool     mirrored_v = false;
    uint8_t  z_order = 0;
    uint16_t image_idx = 0;
};

struct Collision {
    enum Type { Colliding, Separating };

    size_t sprite_a;
    size_t sprite_b;
    Type   type;
};

class SpriteLayer : public ImageLayer {
public:
    static constexpr size_t MAX_SPRITES = 128;
    Sprite sprites[MAX_SPRITES] {};

    [[nodiscard]] std::vector<ImageToDraw> images_to_draw_next_frame([[maybe_unused]] class Scene const& scene) const override;
    [[nodiscard]] std::vector<Collision> check_for_new_collisions() const;

    void subscribe_to_collisions(size_t sprite_a, size_t sprite_b);
    void unsubscribe_to_collisions(size_t sprite_a, size_t sprite_b);
    void unsubscribe_to_all_collisions();

    void reset() override;

private:
    std::set<std::pair<size_t, size_t>> collision_subscriptions_ {};
    mutable std::set<std::pair<size_t, size_t>> current_collisions_ {};

    [[nodiscard]] bool sprites_colliding(size_t sprite_a, size_t sprite_b) const;
};

#endif //SPRITELAYER_HH_
