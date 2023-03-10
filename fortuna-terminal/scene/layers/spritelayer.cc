#include "spritelayer.hh"
#include "images.hh"

#include <algorithm>

std::vector<ImageToDraw> SpriteLayer::images_to_draw_next_frame([[ maybe_unused ]] class Scene const& scene) const
{
    std::vector<ImageToDraw> images_to_draw;
    images_to_draw.reserve(MAX_SPRITES);

    Sprite sprite_copy[MAX_SPRITES] {};
    std::copy_if(std::begin(sprites), std::end(sprites), sprite_copy, [](Sprite const& s) { return s.visible; });
    std::sort(std::begin(sprite_copy), std::end(sprite_copy),
              [](auto const& a, auto const& b) { return a.z_order < b.z_order; });

    for (auto const& s : sprite_copy) {
        if (s.visible)
            images_to_draw.push_back({s.image_idx, s.pos_x, s.pos_y, s.mirrored_h, s.mirrored_v });
    }

    return images_to_draw;
}

void SpriteLayer::subscribe_to_collisions(size_t sprite_a, size_t sprite_b)
{
    collision_subscriptions_.erase({ sprite_b, sprite_a });
    collision_subscriptions_.emplace(sprite_a, sprite_b);
}

void SpriteLayer::unsubscribe_to_collisions(size_t sprite_a, size_t sprite_b)
{
    collision_subscriptions_.erase({ sprite_a, sprite_b });
    collision_subscriptions_.erase({ sprite_b, sprite_a });
}

void SpriteLayer::unsubscribe_to_all_collisions()
{
    collision_subscriptions_.clear();
}

std::vector<Collision> SpriteLayer::check_for_new_collisions() const
{
    std::vector<Collision> new_collisions;
    std::set<std::pair<size_t, size_t>> collisions;

    // check for current collisions
    for (auto const& [sprite_a, sprite_b]: collision_subscriptions_) {
        if (sprites_colliding(sprite_a, sprite_b))
            collisions.emplace(sprite_a, sprite_b);
    }

    // check for new collisions
    for (auto const& c: collisions) {
        auto it = current_collisions_.find(c);
        if (it == current_collisions_.end())
            new_collisions.push_back({ c.first, c.second, Collision::Type::Colliding });
    }

    // check for separations
    for (auto const& c : current_collisions_) {
        auto it = collisions.find(c);
        if (it == collisions.end())
            new_collisions.push_back({ c.first, c.second, Collision::Type::Separating });
    }

    current_collisions_ = collisions;
    return new_collisions;
}

void SpriteLayer::reset()
{
    std::fill(std::begin(sprites), std::end(sprites), Sprite {});
    collision_subscriptions_.clear();
    current_collisions_.clear();
}

bool SpriteLayer::sprites_colliding(size_t sprite_a, size_t sprite_b) const
{
    Sprite const& a = sprites[sprite_a];
    Sprite const& b = sprites[sprite_b];

    if (!a.visible || !b.visible)
        return false;

    int left1 = a.pos_x;
    int right1 = a.pos_x + Image::IMAGE_W;
    int top1 = a.pos_y;
    int bottom1 = a.pos_y + Image::IMAGE_H;

    int left2 = b.pos_x;
    int right2 = b.pos_x + Image::IMAGE_W;
    int top2 = b.pos_y;
    int bottom2 = b.pos_y + Image::IMAGE_H;

    if (left1 > right2 || left2 > right1 || top1 > bottom2 || top2 > bottom1) {
        return false;
    }

    return true;
}
