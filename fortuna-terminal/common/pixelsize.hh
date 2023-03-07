#ifndef PIXELSIZE_HH_
#define PIXELSIZE_HH_

#include <cstddef>

struct PixelSize {
    PixelSize() = delete;

    static constexpr size_t TextChar_W = 6;
    static constexpr size_t TextChar_H = 9;
    static constexpr size_t TextBorder = 12;
};

#endif //PIXELSIZE_HH_
