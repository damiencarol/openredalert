#ifndef BREPAIRUNITANIMEVENT_H
#define BREPAIRUNITANIMEVENT_H

#include "BuildingAnimEvent.h"
#include "Structure.h"

/**
 * Defines the repair a structure logic (needed for the surface depot "FIX")
 */
class BRepairUnitAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the attacking structure
     * @param target the unit or structure to be attacked
     */
    BRepairUnitAnimEvent(Uint32 p, Structure* str);
    ~BRepairUnitAnimEvent();
    void run();
    void stop();
    void anim_func(anim_nfo* data) {}
    void update();
private:
    Structure* strct;
    Uint8 frame;
    bool done;
    /** Total cost of damage remaining */
	Uint16 dmg_cost;
    Uint16 StartFrame;
};

#endif /* STRUCTUREANIMS_H */
