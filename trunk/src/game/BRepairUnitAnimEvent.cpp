// BRepairUnitAnimEvent.cpp
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

#include "BRepairUnitAnimEvent.h"

#include <cmath>

#include "Player.h"
#include "PlayerPool.h"
#include "audio/SoundEngine.h"
#include "Unit.h"
#include "UnitAndStructurePool.h"
#include "weaponspool.h"
#include "anim_nfo.h"
#include "Structure.h"
#include "include/Logger.h"
#include "ActionEventQueue.h"

namespace p {
	extern ActionEventQueue* aequeue;
	extern UnitAndStructurePool* uspool;
	extern PlayerPool* ppool;
}
namespace pc {
    //extern ConfigType Config;
    extern SoundEngine* sfxeng;
}
extern Logger * logger;

/**
 * @param p the priority of this event
 * @param str the structure which repair
 */
BRepairUnitAnimEvent::BRepairUnitAnimEvent(uint32_t p, Structure *str) : BuildingAnimEvent(p,str,8)
{
    int		un_cost;
    Sint16	health;
    Unit	*UnitToFix = NULL;

    // The Anim is not finished
	done		= false;
    // Structure to Apply
    this->strct = str;

    StartFrame	= 0;
    frame		= 0;

	UnitToFix = p::uspool->getUnitAt(strct->UnitToRepairPos);

	if (UnitToFix == 0)
	{
		logger->error ("%s line %i: Structure anim unit not found\n", __FILE__, __LINE__);
		stop();
		return;
	}

	health = UnitToFix->getHealth();
	un_cost = UnitToFix->getType()->getCost();
	if (health > 0) {
		dmg_cost = (Uint16)(((double)un_cost/(double)UnitToFix->getType()->getMaxHealth()) * ((double)UnitToFix->getType()->getMaxHealth() - (double)health));
	} else {
		dmg_cost = (Uint16)un_cost;
    }

    setDelay(1);
}

BRepairUnitAnimEvent::~BRepairUnitAnimEvent()
{
	strct->setImageNum(StartFrame, 0);
	// Set repairunitAnim of the structure to NULL
	strct->repairunitAnim = 0;
}

void BRepairUnitAnimEvent::run()
{
	Unit* UnitToFix = 0;
	Sint16 health;
	uint16_t cost;

//	updateDamaged();

	if( !strct->isAlive() || done ) {
		delete this;
		return;
	}

	UnitToFix = p::uspool->getUnitAt(strct->UnitToRepairPos);

	if (UnitToFix == NULL){
		delete this;
		return;
	}

	health = UnitToFix->getHealth();

	if (health < UnitToFix->getType()->getMaxHealth()){
		cost = (Uint16)((double)dmg_cost/((double)UnitToFix->getType()->getMaxHealth() - (double)health));
		Player* Owner = p::ppool->getPlayer(UnitToFix->getOwner());
		if (Owner->getMoney() > cost){
			Owner->changeMoney(-1 * cost);
			dmg_cost -= cost;
			UnitToFix->ChangeHealth (1);
			UnitToFix->updateDamaged();
		}
	}else{
		//printf ("%s line %i: Unit repaired\n", __FILE__, __LINE__);
		// @todo ADD "Unit repaired" sound
		//pc::sfxeng->PlaySound(pc::Config.UnitRepaired);
		stop();
	}

	if (frame < 6){
		frame++;
	}else{
		frame = 0;
	}

	if (strct->getNumbImages (0) > frame){
		strct->setImageNum(frame,0);
	}else{
		logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, frame);
	}

	setDelay(3);
	p::aequeue->scheduleEvent(this);
}

/**
 *
 */
void BRepairUnitAnimEvent::stop()
{
	done = true;
}

void BRepairUnitAnimEvent::update()
{
	logger->error ("%s line %i: Structure anim update\n", __FILE__, __LINE__);
}

void BRepairUnitAnimEvent::anim_func(anim_nfo* data)
{
}
