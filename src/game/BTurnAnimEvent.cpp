// BTurnAnimEvent.cpp
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

#include "BTurnAnimEvent.h"

#include <cmath>

#include "anim_nfo.h"
#include "Structure.h"

/**
 */
BTurnAnimEvent::BTurnAnimEvent(Uint32 p, Structure* str, Uint8 face) : BuildingAnimEvent(p,str,6)
{
    Uint8 layerface;
    updateDamaged();
    targetface = face;

    // layerface = (str->getImageNums()[0]&0x1f);
    layerface = str->getRealImageNum(0);
    if (layerface == face) {
        delete this;
        return;
    }
    if( ((layerface-face)&0x1f) < ((face-layerface)&0x1f) ) {
        turnmod = -1;
    } else {
        turnmod = 1;
    }
    this->str = str;
}

/**
 */
void BTurnAnimEvent::anim_func(anim_nfo* data)
{
    Uint8 layerface;
    layerface = (str->getImageNums()[0]&0x1f);
    if( abs((layerface-targetface)&0x1f) > abs(turnmod) ) {
        layerface += turnmod;
        layerface &= 0x1f;
    } else {
        layerface = targetface;
    }
    data->frame0 = layerface+data->damagedelta;
    if( layerface == targetface) {
        data->done = true;
    }
}
