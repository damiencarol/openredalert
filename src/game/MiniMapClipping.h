#ifndef MINIMAPCLIPPING_H
#define MINIMAPCLIPPING_H

#include "SDL/SDL_types.h"


class MiniMapClipping {
public:
    Uint16 x;Uint16 y;Uint16 w;Uint16 h;
    Uint16 sidew; Uint16 sideh;
    Uint8 tilew; Uint8 tileh; Uint8 pixsize;
};

#endif
