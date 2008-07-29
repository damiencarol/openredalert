// ProcAnimEvent.cpp
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

#include "ProcAnimEvent.h"

#include "anim_nfo.h"
#include "Structure.h"

ProcAnimEvent::ProcAnimEvent(Uint32 delay, Structure* str) : BuildingAnimEvent(delay, str, 4)
{
    updateDamaged();
    framend = getaniminfo().loopend;
    frame = 0;
}

void ProcAnimEvent::anim_func(anim_nfo* data)
{
    updateDamaged();
    data->frame0 = frame;
    ++frame;
    if ((frame-data->damagedelta) > framend) {
        frame = data->damagedelta;
    }
}

void ProcAnimEvent::updateDamaged()
{
    BuildingAnimEvent::updateDamaged();
    
    if (anim_data.damaged) {
        anim_data.damagedelta = 30; // fixme: remove magic numbers
        if (frame < 30) {
            frame += 30;
        }
    }
}
