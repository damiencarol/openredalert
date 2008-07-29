// RefineAnimEvent.cpp
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

#include "RefineAnimEvent.h"

#include "SDL/SDL_types.h"

#include "include/Logger.h"
#include "PlayerPool.h"
#include "ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "Unit.h"
#include "Player.h"
#include "anim_nfo.h"
#include "Structure.h"

//extern Logger * logger;
namespace p {
	extern PlayerPool* ppool;
}

RefineAnimEvent::RefineAnimEvent(Uint32 p, Structure* str, Uint8 bails) : BuildingAnimEvent(p, str, 7)
{
	updateDamaged();
	
    this->bails = bails;
    this->str = str;
    frame = framestart;
    
    //logger->error ("%s line %i: Start refine animation\n", __FILE__, __LINE__);
}

void RefineAnimEvent::anim_func(anim_nfo* data)
{
    updateDamaged();
    
    if(bails>0) {
        if (frame < framend) {
            ++frame;
        } else {
            frame = framestart;
            --bails;
            // @todo CHANGE IT TO GET FROM STRUCTURE
            p::ppool->getPlayer(str->getOwner())->changeMoney(100);
        }
    } else {
        data->done = true;
    }
    data->frame0 = frame;
}

void RefineAnimEvent::updateDamaged()
{
    BuildingAnimEvent::updateDamaged();
    
    if (anim_data.damaged) {
        if (frame < getaniminfo().dmgoff) {
            frame += getaniminfo().dmgoff;
        }
    }
    
    // @todo fixme: avoid hardcoded values
    framestart = getaniminfo().loopend+1+anim_data.damagedelta;
    framend = framestart + 17; 
}
