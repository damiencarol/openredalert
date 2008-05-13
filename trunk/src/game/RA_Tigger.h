#ifndef RA_TIGGER_H
#define RA_TIGGER_H

#include "SDL/SDL_types.h"


struct RA_Tigger{
    /** Trigger event */
    int event;
    /** second parameter, -1 == off */
    int param1;
    /** second parameter, -1 == offns, this is set to 0 when not used. */
    int param2;
};

#endif //RA_TIGGER_H
