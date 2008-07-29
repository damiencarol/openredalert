// BuildAnimEvent.h
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

#ifndef BUILDANIMEVENT_H
#define BUILDANIMEVENT_H

#include "SDL/SDL_types.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"

class Structure;

/**
 * The animation that is shown when a structure is either built or sold.
 */
class BuildAnimEvent : public BuildingAnimEvent
{
public:
    /**
     * @param p the priority of this event
     * @param str pointer to the structure being built/sold
     * @param sell whether the structure is being built or sold (true if sold)
     */
    BuildAnimEvent(Uint32 p, Structure * str, bool sell);
    ~BuildAnimEvent();
    
    void anim_func(anim_nfo * data);

private:
    Structure * structure;
    Uint8 frame;
    Uint8 framend;
    bool sell;
};

#endif //BUILDANIMEVENT_H
