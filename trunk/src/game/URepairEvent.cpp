// URepairEvent.cpp
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

#include "URepairEvent.h"

#include <cmath>

#include "UnitOrStructure.h"
#include "unittypes.h"
#include "MoveAnimEvent.h"
#include "Structure.h"
#include "UnitAndStructurePool.h"
#include "Unit.h"
#include "ActionEventQueue.h"

namespace p {
	extern UnitAndStructurePool* uspool;
	extern ActionEventQueue* aequeue;
}

URepairEvent::URepairEvent(Uint32 p, Unit *un) : UnitAnimEvent(p,un)
{
	int		un_cost;
	Sint16	health;

	this->un	= un;
	stopping	= false;
	index		= 0;
	delay		= 0;

	fix_str_num = un->GetFixStr();
	fix_str_pos = un->GetFixPos ();

	health = un->getHealth();
	un_cost = un->getType()->getCost();
	if (health > 0)
		dmg_cost = (Uint16)(((double)un_cost/(double)un->getType()->getMaxHealth()) * ((double)un->getType()->getMaxHealth() - (double)health));
	else
		dmg_cost = (Uint16)un_cost;

	moveCounter = 0;
}

/**
 * 
 */
URepairEvent::~URepairEvent()
{
    if (un->repairanim == this)
    {
        un->repairanim = 0;
        }
}

/**
 * 
 */
void URepairEvent::stop()
{
    if (un == 0)
    {
        printf("Repair unit::stop: un is NULL!?\n");
        abort();
    }
    stopping = true;
}

void URepairEvent::update()
{
    stopping = false;
}

void URepairEvent::run()
{
	Structure* FixStr = 0;

	if( !un->isAlive() || stopping ) {
		delete this;
		return;
	}

	FixStr = p::uspool->getStructureAt(fix_str_pos);

	if ( FixStr == NULL ){
		delete this;
		return;
	}

	// Check the structure is actually fix
	if (strcmp ((char*)FixStr->getType()->getTName(), "FIX") != 0 ){
		delete this;
		return;
	}

	if (un->getPos() != fix_str_pos)
	{
		if (moveCounter > 4){
			// Can't reach target, give up
			printf ("%s line %i: Can't reach target --> give up\n", __FILE__, __LINE__);
			delete this;
			return;
		}
//		if ((un->getType()->getTName(), "HARV") == 0 ){
//			un->Harvest (fix_str_pos, NULL);
//		}

		un->move(fix_str_pos, true);
		un->moveanim->setSchedule(this);
		moveCounter++;
		return;
	}

	if (!FixStr->RepairUnint(un)){
		delete this;
		return;
	}

	// Oke, the rest has to be done by the fix structure --> stop
	stop();

	// Reschedule this..
	setDelay(14);
	p::aequeue->scheduleEvent(this);

}
