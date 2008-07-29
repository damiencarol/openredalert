// DoorAnimEvent.h
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

#ifndef DOORANIMEVENT_H
#define DOORANIMEVENT_H

#include "BuildingAnimEvent.h"

struct anim_nfo;
class Structure;

/**
 * Animation used by the Weapons Factory to animate the door opening and closing
 *
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r375
 */
class DoorAnimEvent : public BuildingAnimEvent 
{
public:
    /**
     * @param p the priority of this event
     * @param str the structure to which this animation is to be applied
     * @param opening whether the door is opening or closing
     */
    DoorAnimEvent(Uint32 p, Structure* str, bool opening);
	~DoorAnimEvent ();

    void anim_func(anim_nfo* data);
    void updateDamaged();

private:
    Structure* strct;
	Uint32 delayCounter;
    Uint8 frame;
    Uint8 framend;
    Uint8 framestart;
    Uint8 frame0;
    bool opening;
};

#endif //DOORANIMEVENT_H
