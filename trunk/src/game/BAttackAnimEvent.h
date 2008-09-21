// BAttackAnimEvent.cpp
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
 *
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r377
 */
class BAttackAnimEvent : public BuildingAnimEvent
{
public:
    /** Constructor */
    BAttackAnimEvent(Uint32 p, Structure* str);
    ~BAttackAnimEvent();

    void run();
    void stop();
    void anim_func(anim_nfo* data);
    void update();

private:
    uint8_t frame;
    Structure* strct;
    /** the unit or structure to be attacked */
    UnitOrStructure* target;
    bool done;
    bool NeedToCharge;
    Uint16 StartFrame;
};

#endif //BATTACKANIMEVENT_H
