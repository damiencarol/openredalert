// LoopAnimEvent.cpp
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

#include "LoopAnimEvent.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"
#include "Structure.h"

LoopAnimEvent::LoopAnimEvent(Uint32 p, Structure* str) : BuildingAnimEvent(p, str, 1)
{
    updateDamaged();
    framend = getaniminfo().loopend;
    frame = 0;
}
void LoopAnimEvent::anim_func(anim_nfo* data)
{
    updateDamaged();
    data->frame0 = frame;
    if ((frame-data->damagedelta) < framend) {
        ++frame;
    } else {
        frame = data->damagedelta;
    }
}
