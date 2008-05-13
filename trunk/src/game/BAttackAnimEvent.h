#ifndef BATTACKANIMEVENT_H
#define BATTACKANIMEVENT_H

#include "BuildingAnimEvent.h"

struct anim_nfo;
class Structure;
class UnitOrStructure;


/** 
 * Defines the attack logic
 *
 * This animation is different to the others as it overrides
 * the run function rather than the anim_func function.
 */
class BAttackAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the attacking structure
     * @param target the unit or structure to be attacked
     */
    BAttackAnimEvent(Uint32 p, Structure* str);
    ~BAttackAnimEvent();
    void run();
    void stop();
    void anim_func(anim_nfo* data) {}
    void update();
private:
    Uint8 frame;
    Structure* strct;
    UnitOrStructure* target;
    bool done;
    bool NeedToCharge;
    Uint16 StartFrame;
};

#endif
