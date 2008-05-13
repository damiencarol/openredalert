#ifndef POWERINFO_H
#define POWERINFO_H

#include "SDL/SDL_types.h"

class PowerInfo
{
public:	
    Uint16 power;
    Uint16 drain;
    bool powered;
};

#endif //POWERINFO_H
