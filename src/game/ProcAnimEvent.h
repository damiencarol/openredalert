// ProcAnimEvent.h
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

#ifndef PROCANIMEVENT_H
#define PROCANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"

class Structure;

/**
 * Modifed LoopAnimEvent to account for when the damaged frames do not concurrently follow the normal frames.
 */
class ProcAnimEvent : public BuildingAnimEvent {
public:
    ProcAnimEvent(Uint32 delay, Structure* str);
    void anim_func(anim_nfo* data);
    void updateDamaged();
private:
    Uint8 frame;Uint8 framend;
};


#endif //PROCANIMEVENT_H
