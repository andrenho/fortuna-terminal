#ifndef GRAPHICSPAINTER_HH_
#define GRAPHICSPAINTER_HH_

#include "lib/SDL2-windows/include/SDL2/SDL.h"

#include <memory>
#include "old/terminal/texturemanager.hh"
#include "old/scene/scene.hh"

class GraphicsPainter {
public:
    explicit GraphicsPainter(SDL_Renderer* renderer)
        : texture_manager_(std::make_unique<TextureManager>(renderer)), renderer_(renderer) {}

    void setup_scene(Scene const& scene);

    void initialize_pending_images(Scene& scene);

    void draw_background(Scene const& scene) const;
    void draw(Scene const& scene, LayerIdentifier layer_id) const;

    std::unique_ptr<TextureManager> texture_manager_;

private:
    SDL_Renderer* renderer_;
};

#endif //GRAPHICSPAINTER_HH_