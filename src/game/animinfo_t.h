#ifndef ANIMINFO_T_H
#define ANIMINFO_T_H

#include "SDL/SDL_types.h"

struct animinfo_t {
    Uint32 animdelay;
    Uint8 loopend;
    Uint8 loopend2;
    Uint8 animspeed;
    Uint8 animtype;
    Uint8 sectype;
    Uint8 dmgoff;
    Uint8 dmgoff2;
    Uint16 makenum;
};

#endif //ANIMINFO_T_H
