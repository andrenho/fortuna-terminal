#ifndef IMAGELAYER_HH_
#define IMAGELAYER_HH_

#include "layer.hh"
#include "../../common/syncqueue.hh"

struct Image {
    static constexpr size_t IMAGE_W = 16;
    static constexpr size_t IMAGE_H = 16;
    static constexpr size_t IMAGE_SZ = IMAGE_W * IMAGE_H;

    uint16_t key;
    uint8_t  transparent_color;
    uint8_t  image[IMAGE_W * IMAGE_H];
};

class ImageLayer : public Layer {
public:
    void create_image(Image&& image) { pending_images_->emplace(std::move(image)); }

    static constexpr size_t MAX_IMAGES = 512;

    [[nodiscard]] std::unique_ptr<SyncQueue<Image>>& pending_images() { return pending_images_; }

protected:
    ImageLayer() = default;
    std::unique_ptr<SyncQueue<Image>> pending_images_ = std::make_unique<SyncQueue<Image>>();
};

#endif //IMAGELAYER_HH_
