#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

#define SH1106_CHAR_MAPS_START 0x2E
#define SH1106_CHAR_MAPS_END 0x5A // Last character address represented in fonts arrays.


typedef uint8_t Font[SH1106_CHAR_MAPS_END-SH1106_CHAR_MAPS_START+1][8];

extern Font default_font;

#endif