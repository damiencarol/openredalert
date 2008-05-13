#ifndef BUILDANIMEVENT_H
#define BUILDANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"

class Structure;

/** 
 * The animation that is shown when a structure is either built or sold.
 */
class BuildAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str pointer to the structure being built/sold
     * @param sell whether the structure is being built or sold (true if sold)
     */
    BuildAnimEvent(Uint32 p, Structure* str, bool sell);
    ~BuildAnimEvent();
    void anim_func(anim_nfo* data);
private:
	Structure* structure;
	Uint8 frame;
    Uint8 framend;
	bool sell;
};

#endif
