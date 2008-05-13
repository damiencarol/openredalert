#ifndef DOORANIMEVENT_H
#define DOORANIMEVENT_H

#include "BuildingAnimEvent.h"

struct anim_nfo;
class Structure;

/**
 * Animation used by the Weapons Factory to animate the door opening and closing
 */
class DoorAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the structure to which this animation is to be applied
     * @param opening whether the door is opening or closing
     */
    DoorAnimEvent(Uint32 p, Structure* str, bool opening);
	~DoorAnimEvent ();
    void anim_func(anim_nfo* data);
    void updateDamaged();
private:
    Structure* strct;
	Uint32 delayCounter;
    Uint8 frame;
    Uint8 framend;
    Uint8 framestart;
    Uint8 frame0;
    bool opening;
};

#endif //DOORANIMEVENT_H
