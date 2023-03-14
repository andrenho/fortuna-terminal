#ifndef GRAPHICSPAINTER_HH_
#define GRAPHICSPAINTER_HH_

#include "lib/SDL2-windows/include/SDL2/SDL.h"

#include <memory>
#include <unordered_set>
#include "terminal/textureatlas.hh"
#include "scene/scene.hh"

class GraphicsPainter {
public:
    explicit GraphicsPainter(SDL_Renderer* renderer)
        : texture_atlas_(renderer), renderer_(renderer) {}

    void initialize_pending_images(Scene const& scene);

    void draw_background(Scene const& scene) const;
    void draw(Scene const& scene, LayerIdentifier layer_id) const;

    void reset(size_t scene_id) { texture_atlas_.reset(scene_id); }

private:
    TextureAtlas   texture_atlas_;
    SDL_Renderer*  renderer_;
};

#endif //GRAPHICSPAINTER_HH_
