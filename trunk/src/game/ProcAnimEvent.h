#ifndef PROCANIMEVENT_H
#define PROCANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"

class Structure;

/**
 * Modifed LoopAnimEvent to account for when the damaged frames do not concurrently follow the normal frames.
 */
class ProcAnimEvent : public BuildingAnimEvent {
public:
    ProcAnimEvent(Uint32 delay, Structure* str);
    void anim_func(anim_nfo* data);
    void updateDamaged();
private:
    Uint8 frame;Uint8 framend;
};


#endif //PROCANIMEVENT_H
