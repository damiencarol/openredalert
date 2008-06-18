// BAttackAnimEvent.cpp
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

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
class BAttackAnimEvent : public BuildingAnimEvent 
{
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

#endif //BATTACKANIMEVENT_H
