#ifndef BTURNANIMEVENT_H
#define BTURNANIMEVENT_H

#include "SDL/SDL_types.h"
#include "BuildingAnimEvent.h"
#include "Structure.h"

/** The animation that turns structures to face a given direction.
* This is only used when attacking.
*/
class BTurnAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the structure to which this animation is to be applied
     * @param face the direction the structure is to face
     */
    BTurnAnimEvent(Uint32 p, Structure* str, Uint8 face);
    void anim_func(anim_nfo* data);
private:
    Uint8 frame;Uint8 targetface;
    Sint8 turnmod;
    Structure* str;
};


#endif
