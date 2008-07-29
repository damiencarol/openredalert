// BTurnAnimEvent.h
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

#ifndef BTURNANIMEVENT_H
#define BTURNANIMEVENT_H

#include "SDL/SDL_types.h"
#include "BuildingAnimEvent.h"
#include "Structure.h"

/** 
 * The animation that turns structures to face a given direction. This is only used when attacking.
 * 
 * @version 1.0
 * @since r378
 */
class BTurnAnimEvent : public BuildingAnimEvent
{
public:
    /**
     * @param p the priority of this event
     * @param str the structure to which this animation is to be applied
     * @param face the direction the structure is to face
     */
    BTurnAnimEvent(Uint32 p, Structure * str, Uint8 face);

    void anim_func(anim_nfo * data);

private:
    Uint8 frame;
    Uint8 targetface;
    Sint8 turnmod;
    Structure * str;
};

#endif //BTURNANIMEVENT_H
