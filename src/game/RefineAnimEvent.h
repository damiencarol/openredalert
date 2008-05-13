#ifndef REFINEANIMEVENT_H
#define REFINEANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"

class Structure;


/** 
 * Animation depicting the refinery processing tiberium 
 */
class RefineAnimEvent : public BuildingAnimEvent {
public:
    RefineAnimEvent(Uint32 p, Structure * str, Uint8 bails);
    void anim_func(anim_nfo* data);
    void updateDamaged();
    
private:
    Structure * str;
    Uint8 framestart;
    Uint8 frame;
    Uint8 framend;
    Uint8 bails;
};

#endif //REFINEANIMEVENT_H
