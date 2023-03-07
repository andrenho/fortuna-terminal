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
    uint16_t image = 0;
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

    [[nodiscard]] std::vector<ImageToDraw> images_to_draw([[maybe_unused]] class Scene const& scene) const override;

    void subscribe_to_collisions(size_t sprite_a, size_t sprite_b);
    void unsubscribe_to_collisions(size_t sprite_a, size_t sprite_b);
    void unsubscribe_to_all_collisions();

    std::vector<Collision> check_for_new_collisions();

    void reset() override;

private:
    std::set<std::pair<size_t, size_t>> collision_subscriptions_ {};
    std::set<std::pair<size_t, size_t>> current_collisions_ {};

    bool sprites_colliding(unsigned long long int const sprite_a, unsigned long long int const sprite_b);
};

#endif //SPRITELAYER_HH_
