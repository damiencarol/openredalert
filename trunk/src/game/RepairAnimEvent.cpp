// RepairAnimEvent.cpp
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

#include "RepairAnimEvent.h"

#include <cmath>

#include "anim_nfo.h"
#include "Structure.h"
#include "Player.h"
#include "PlayerPool.h"

namespace p {
	extern PlayerPool* ppool;
}

RepairAnimEvent::RepairAnimEvent(Uint32 p, Structure* str) : BuildingAnimEvent(p,str,0)
{
	int	str_cost;
	Sint16	health;

	updateDamaged();
	
	structure = str;
	health = structure->getHealth();
	str_cost = structure->getType()->getCost();
	if (health > 0)
		dmg_cost = (Uint16)(((double)str_cost/(double)structure->getType()->getMaxHealth()) * ((double)structure->getType()->getMaxHealth() - (double)health));
	else
		dmg_cost = (Uint16)str_cost;


	// For looping animated structures
	framend = getaniminfo().loopend;
	frame = (structure->getImageNums()[0]&0x1f); //structure->getImageNum(0)&0x7FF;

//	printf ("%s line %i: Repair anim CONSTRUCTOR, frame = %i\n", __FILE__, __LINE__, frame);
}

RepairAnimEvent::~RepairAnimEvent()
{
	updateDamaged();
	structure->repairDone();
//	printf ("%s line %i: Repair anim DESTRUCTOR\n", __FILE__, __LINE__);
}

void RepairAnimEvent::anim_func(anim_nfo* data)
{
	Sint16	health;
	Uint16	cost;
	
	//
	data->frame0 = structure->getRealImageNum(0);	//(structure->getImageNums()[0]&0x7FF /*0x1f*/); //structure->getImageNum(0)&0x7FF;

	health = structure->getHealth();

	if (health < structure->getType()->getMaxHealth()){
		cost = (Uint16)((double)dmg_cost/((double)structure->getType()->getMaxHealth() - (double)health));
		Player* Owner = 0;
		// Get the owner
		Owner = p::ppool->getPlayer(structure->getOwner());
		if (Owner->getMoney() > cost){
			Owner->changeMoney(-1 * cost);
			dmg_cost -= cost;
			structure->ChangeHealth (1);
			updateDamaged();
			data->frame0 = data->damagedelta;
			if (structure->getType()->getNumLayers() == 2)
				data->frame1 = data->damagedelta2;
#if 0
			// For looping animated structures
			if (getaniminfo().animtype == 1 && data->damagedelta == 0){
				printf ("%s line %i: Loopend\n", __FILE__, __LINE__);
				data->frame0 = frame;
				if ((frame-data->damagedelta) < framend) {
					++frame;
				} else {
					frame = data->damagedelta;
				}
			// For turn animated structures
			}else if (getaniminfo().animtype == 6){
//				printf ("%s line %i: Repair anim, frame = %i, turndelta = %i, damagedelta = %i\n", __FILE__, __LINE__, data->frame0, data->turndelta, data->damagedelta);
			}else{
//				printf ("%s line %i: Repair anim, frame = %i, type = %i, turndelta = %i, damagedelta = %i\n", __FILE__, __LINE__, data->frame0, getaniminfo().animtype, data->turndelta, data->damagedelta);
				data->frame0 = data->damagedelta;
				if (structure->getType()->getNumLayers() == 2)
					data->frame1 = data->damagedelta2;
			}
#endif
		}
	}else{
//		printf ("%s line %i: Repair anim, frame = %i, turndelta = %i, damagedelta = %i\n", __FILE__, __LINE__, data->frame0, data->turndelta, data->damagedelta);
//		if (data->frame0 != 0 && health == structure->getType()->getMaxHealth())
//			data->frame0 = 0;
		if (data->frame0 > getaniminfo().dmgoff){
			data->frame0 -= getaniminfo().dmgoff;
		}else{
			stop();
		}
	}
}

