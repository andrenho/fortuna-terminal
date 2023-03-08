#ifndef IMAGES_HH_
#define IMAGES_HH_

#include "layer.hh"
#include "common/syncqueue.hh"

struct Image {
    static constexpr int IMAGE_W = 16;
    static constexpr int IMAGE_H = 16;
    static constexpr int IMAGE_SZ = IMAGE_W * IMAGE_H;

    uint16_t key;
    uint8_t  transparent_color;
    uint8_t  image[IMAGE_W * IMAGE_H];
};

class Images : NonCopyable {
public:
    void create_image(Image&& image) { pending_images_->emplace(std::move(image)); }

    [[nodiscard]] SyncQueueUniqPtr<Image> & pending_images() const
    { return pending_images_; }

private:
    mutable SyncQueueUniqPtr<Image> pending_images_ = std::make_unique<SyncQueue<Image>>();
};

#endif //IMAGES_HH_
