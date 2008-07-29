// InfantryGroup.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INFANTRYGROUP_H
#define INFANTRYGROUP_H

#include "SDL/SDL_types.h"

class Unit;

/**
 * This should be a member of unit for infantry. When an infantry unit walks
 * into a previously empty cell a new group is created, otherwise the existing group is used.
 * We need one more bit in the unit/structure matrix to tell if infantry is in that cell.
 * @todo: Implement group reuse, or just scrap this in favour of something that
 * won't cause lots of allocations and deallocations whilst moving infantry.
 */
class InfantryGroup 
{
public:
    InfantryGroup();
    ~InfantryGroup();
    bool AddInfantry(Unit* inf, Uint8 subpos);
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
    Unit* positions[5];
    Uint8 numinfantry;
    static const Sint8 unitoffsets[];
};

#endif //INFANTRYGROUP_H
