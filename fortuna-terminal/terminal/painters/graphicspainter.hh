#ifndef GRAPHICSPAINTER_HH_
#define GRAPHICSPAINTER_HH_

#include "lib/SDL2-windows/include/SDL2/SDL.h"

#include <memory>
#include "terminal/texturemap.hh"
#include "scene/scene.hh"

class GraphicsPainter {
public:
    explicit GraphicsPainter(SDL_Renderer* renderer)
        : texture_manager_(std::make_unique<TextureMap>(renderer)), renderer_(renderer) {}

    void initialize_pending_images(Scene const& scene);

    void draw_background(Scene const& scene) const;
    void draw(Scene const& scene, LayerIdentifier layer_id) const;

    std::unique_ptr<TextureMap> texture_manager_;

private:
    SDL_Renderer* renderer_;
};

#endif //GRAPHICSPAINTER_HH_
