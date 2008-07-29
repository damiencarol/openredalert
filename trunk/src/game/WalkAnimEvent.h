// WalkAnimEvent.h
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

#ifndef WALKANIMEVENT_H
#define WALKANIMEVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"
#include "Unit.h"

class WalkAnimEvent : public UnitAnimEvent
{
public:
    WalkAnimEvent(Uint32 p, Unit * un, Uint8 dir, Uint8 layer);
    virtual ~WalkAnimEvent();
    virtual void stop();
    virtual void run();
    virtual void changedir(Uint8 ndir);
    void update();
private:
    bool stopping;
    void calcbaseimage(void);
    Unit * un;
    Uint8 dir;
    Uint8 istep;
    Uint8 layer;
    Uint8 baseimage;
};

#endif //WALKANIMEVENT_H
