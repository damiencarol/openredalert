#ifndef ANIM_NFO_H
#define ANIM_NFO_H

#include "SDL/SDL_types.h"

/**
 * Rather than have each derived class be a friend of the Structure
 * class, all changes to the structure are made in the BuildingAnimEvent::run
 * function, based on information stored in this struct (passed to the derived
 * class's anim_func function)
 */
struct anim_nfo {
    /** layer zero of the structure will be set to this value */
    Uint16 frame0;
    /** layer one (if it exists) will be set to this value */
    Uint16 frame1;
    /** 
     * offset in the loop animation for layer zero if structure is damaged
     */
    Uint16 damagedelta;
    /** offset in the loop animation for layer one if structure is damaged */
    Uint16 damagedelta2;
    /** has the animation finished */
    bool done;
    /** is the structure damaged */
    bool damaged;
    /** identifying constant for the animation type */
    Uint8 mode;
};

#endif
