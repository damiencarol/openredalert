#ifndef CPSHEADER_H
#define CPSHEADER_H

#include "SDL/SDL_types.h"

class CPSHeader {
public:
    Uint16 size;
    Uint16 unknown;
    Uint16 imsize;
    Uint32 palette;
};

#endif //CPSHEADER_H
