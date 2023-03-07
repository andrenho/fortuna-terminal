#ifndef PALETTE_HH_
#define PALETTE_HH_

#include "common/color.hh"

#define PALETTE_SZ 16

using Palette = Color[PALETTE_SZ];

void palette_init(Palette& palette);

#endif //PALETTE_HH_
