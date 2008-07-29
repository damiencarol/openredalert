// RefineAnimEvent.h
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

#ifndef REFINEANIMEVENT_H
#define REFINEANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"

class Structure;


/** 
 * Animation depicting the refinery processing tiberium 
 */
class RefineAnimEvent : public BuildingAnimEvent {
public:
    RefineAnimEvent(Uint32 p, Structure * str, Uint8 bails);
    void anim_func(anim_nfo* data);
    void updateDamaged();
    
private:
    Structure * str;
    Uint8 framestart;
    Uint8 frame;
    Uint8 framend;
    Uint8 bails;
};

#endif //REFINEANIMEVENT_H
