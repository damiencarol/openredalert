// LoopAnimEvent.h
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

#ifndef LOOPANIMEVENT_H
#define LOOPANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"

struct anim_nfo;

class Structure;

/** 
 * Simple animation that linearly increases the frame number between two given limits
 */
class LoopAnimEvent : public BuildingAnimEvent {
public:
    /**
     * @param p the priority of this event
     * @param str the structure to which this animation is to be applied
     */
    LoopAnimEvent(Uint32 p, Structure* str);
    void anim_func(anim_nfo* data);
private:
    Uint8 frame; 
    Uint8 framend;
};

#endif //LOOPANIMEVENT_H
