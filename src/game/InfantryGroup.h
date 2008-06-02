#ifndef INFANTRYGROUP_H
#define INFANTRYGROUP_H

#include <cassert>
#include <map>
#include <vector>
#include <stack>
#include <time.h>

#include "SDL/SDL_types.h"

#include "game/ActionEventQueue.h"
#include "Unit.h"

#define LOOPEND_TURN

class ActionEventQueue;

namespace p {
    extern ActionEventQueue * aequeue;
}


class INIFile;
class Weapon;
class WeaponsPool;
class StructureType;
class L2Overlay;

/**
 * This should be a member of unit for infantry. When an infantry unit walks
 * into a previously empty cell a new group is created, otherwise the existing group is used.
 * We need one more bit in the unit/structure matrix to tell if infantry is in that cell.
 * TODO: Implement group reuse, or just scrap this in favour of something that
 * won't cause lots of allocations and deallocations whilst moving infantry.
 */
class InfantryGroup {
public:
    InfantryGroup();
    ~InfantryGroup();
    bool AddInfantry(Unit * inf, Uint8 subpos);
    bool RemoveInfantry(Uint8 subpos);
    bool IsClear(Uint8 subpos);
    Uint8 GetNumInfantry() const;
    bool IsAvailable() const;
    Uint8 GetFreePos() const;
    Unit * UnitAt(Uint8 subpos);

    Uint8 GetImageNums(Uint32 * * inums, Sint8 * * xoffsets, Sint8 * * yoffsets);
    void GetSubposOffsets(Uint8 oldsp, Uint8 newsp, Sint8 * xoffs, Sint8 * yoffs);
    static const Sint8 * GetUnitOffsets();
    Unit * GetNearest(Uint8 subpos);
private:
    Unit * positions[5];
    Uint8 numinfantry;
    static const Sint8 unitoffsets[];
};


#endif
