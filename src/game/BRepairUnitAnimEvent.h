// BRepairUnitAnimEvent.h
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

#ifndef BREPAIRUNITANIMEVENT_H
#define BREPAIRUNITANIMEVENT_H

#include "BuildingAnimEvent.h"

class Structure;

/**
 * Defines the repair a structure logic (needed for the surface depot "FIX")
 */
class BRepairUnitAnimEvent : public BuildingAnimEvent 
{
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
    void anim_func(anim_nfo* data) ;
    void update();

private:
    Structure* strct;
    Uint8 frame;
    bool done;
    /** Total cost of damage remaining */
	Uint16 dmg_cost;
    Uint16 StartFrame;
};

#endif //BREPAIRUNITANIMEVENT_H
