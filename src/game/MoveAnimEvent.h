// MoveAnimEvent.cpp
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

#ifndef MOVEANIMEVENT_H
#define MOVEANIMEVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"

class Unit;
class Path;

/**
 * 
 */
class MoveAnimEvent : public UnitAnimEvent {
public:
    MoveAnimEvent(Uint32 p, Unit * un);
    virtual ~MoveAnimEvent();
    virtual void stop();
    virtual void run();
    virtual void update();
    virtual void setRange(Uint32 nr) ;
private:
	Uint32 DefaultDelay;
	Uint16 BlockedCounter;
    bool stopping;
    void startMoveOne(bool wasblocked);
    void moveDone();
    Uint16 dest;
    Uint16 newpos;
    bool blocked;
    bool moved_half;
    bool pathinvalid;
    bool waiting;
    Sint8 xmod;
    Sint8 ymod;
    Sint8 b_xmod;
    Sint8 b_ymod;
    Unit* un;
    Path* path;
    Uint8 istep;
    Uint8 dir;
    Uint32 range;
};

#endif //MOVEANIMEVENT_H
