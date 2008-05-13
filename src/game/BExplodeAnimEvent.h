#ifndef BEXPLODEANIMEVENT_H
#define BEXPLODEANIMEVENT_H

//
#include "BuildingAnimEvent.h"
#include "anim_nfo.h"
#include "Structure.h"

/** Animation used when a building explodes
 *
 * This animation updates the UnitAndStructurePool to remove
 * the destroyed structure upon finishing.  This class also
 * overrides the run function (but calls the BuildingAnimEvent::run
 * function)
 * \see BuildingAnimEvent
 */
class BExplodeAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the structure that has just been destroyed
     */
    BExplodeAnimEvent(Uint32 p, Structure* str);

    /// Updates the UnitAndStructurePool
    /// @todo spawn survivors
    /// @todo spawn flame objects
    ~BExplodeAnimEvent();
    virtual void run();
private:
    Structure* strct;
    Uint16 lastframe; Uint16 counter;
    virtual void anim_func(anim_nfo* data);
};

#endif /*  */
