#ifndef WSAHEADER_H
#define WSAHEADER_H

#include "SDL/SDL_types.h"

class WSAHeader {
public:
    Uint16 NumFrames;
    Uint16 xpos;
    Uint16 ypos;
    Uint16 width;
    Uint16 height;
    Uint32 delta;
    Uint32 *offsets;
};

#endif //WSAHEADER_H
