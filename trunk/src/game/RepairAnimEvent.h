#ifndef REPAIRANIMEVENT_H
#define REPAIRANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"

class Structure;

/** The animation that is shown when a structure is either built or sold*/
class RepairAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str pointer to the structure being built/sold
     * @param sell whether the structure is being built or sold (true if sold)
     */
    RepairAnimEvent(Uint32 p, Structure* str);
    ~RepairAnimEvent();
    void anim_func(anim_nfo* data);
private:
	Structure* structure;
	Uint8 frame;Uint8 framend;
    /**  Total cost of damage remaining */
	Uint16 dmg_cost;
//	bool sell;
};


#endif
