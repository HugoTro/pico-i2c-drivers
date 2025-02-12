#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

// 43 is enough to hold from 0 to Z in the ASCII charmap.
typedef uint8_t Font[43][8];

extern Font default_font;

#endif