#ifndef SHPHEADER_H
#define SHPHEADER_H

#include "SDL/SDL_types.h"

class SHPHeader {
    public:
    Uint16  NumImages;
    Uint16  Width;
    Uint16  Height;
    Uint32* Offset;
    Uint8*  Format;
    Uint32* RefOffs;
    Uint8*  RefFormat;
};

#endif
