// WalkAnmiEvent.cpp
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

#include "WalkAnimEvent.h"

#include <cmath>

#include "CnCMap.h"
#include "UnitAndStructurePool.h"
#include "weaponspool.h"
#include "PlayerPool.h"
#include "UnitOrStructure.h"
#include "unittypes.h"
#include "TurnAnimEvent.h"
#include "UAttackAnimEvent.h"
#include "Path.h"
#include "Unit.h"
#include "UnitType.h"
#include "ActionEventQueue.h"
#include "animinfo_t.h"

namespace p {
	extern ActionEventQueue* aequeue;
}

WalkAnimEvent::WalkAnimEvent(Uint32 p, Unit *un, Uint8 dir, Uint8 layer) : UnitAnimEvent(p,un)
{
    //fprintf(stderr,"debug: WalkAnim constructor\n");
    this->un = un;
    this->dir = dir;
    this->layer = layer;
    stopping = false;
    istep = 0;
    calcbaseimage();
}

WalkAnimEvent::~WalkAnimEvent()
{
#ifdef LOOPEND_TURN
	UnitType* unitTypeOf = this->un->getType();
	
    un->setImageNum((unitTypeOf->getAnimInfo().loopend+1)*dir/8, layer);
#else
	un->setImageNum(dir>>2, layer);
#endif
    
	// Unreference
	if (un->walkanim == this){
        un->walkanim = 0;
    }
}

void WalkAnimEvent::run()
{
    Uint8 layerface;
    if (!stopping) {
        layerface = baseimage + istep;
        // XXX: Assumes 6 frames to loop over
        istep = (istep + 1)%6;
		//printf ("%s line %i: Baseimg = %i, istep = %i\n", __FILE__, __LINE__, baseimage, istep);
		un->setImageNum(layerface,layer);
        p::aequeue->scheduleEvent(this);
    } else {
        delete this;
        return;
    }
}

void WalkAnimEvent::calcbaseimage()
{
    // XXX: this is really nasty, will be taken care of after the rewrite
    baseimage = 16 + 3*(dir/2);
}

void WalkAnimEvent::stop() 
{
	stopping = true;
}

void WalkAnimEvent::changedir(Uint8 ndir) 
{
	stopping = false;
	dir = ndir;
	calcbaseimage();
}

void WalkAnimEvent::update() 
{
}
