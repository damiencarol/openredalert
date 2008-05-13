#ifndef DUNE2HEADER_H
#define DUNE2HEADER_H

#include "SDL/SDL_types.h"

class Dune2Header {
public:
    Uint16 compression;
    Uint8  cy;
    Uint16 cx;
    Uint8  cy2;
    Uint16 size_in;
    Uint16 size_out;
};

#endif //DUNE2HEADER_H
