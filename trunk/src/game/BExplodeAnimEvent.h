// BExploreAnimEvent.h
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
 * 
 * @see BuildingAnimEvent
 */
class BExplodeAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the structure that has just been destroyed
     */
    BExplodeAnimEvent(Uint32 p, Structure* str);
    ~BExplodeAnimEvent();
    
    /**
     * Updates the UnitAndStructurePool
     * @todo spawn survivors
     * @todo spawn flame objects
     */
    virtual void run();
    
private:
    Structure* strct;
    Uint16 lastframe; Uint16 counter;
    virtual void anim_func(anim_nfo* data);
};

#endif //BEXPLODEANIMEVENT_H
