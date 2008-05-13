#ifndef LOOPANIMEVENT_H
#define LOOPANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"

struct anim_nfo;

class Structure;

/** 
 * Simple animation that linearly increases the frame number between two given limits
 */
class LoopAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the structure to which this animation is to be applied
     */
    LoopAnimEvent(Uint32 p, Structure* str);
    void anim_func(anim_nfo* data);
private:
    Uint8 frame; 
    Uint8 framend;
};

#endif //LOOPANIMEVENT_H
