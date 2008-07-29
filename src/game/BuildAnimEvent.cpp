// BuildAnimEvent.cpp
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

#include "BuildAnimEvent.h"

#include <cmath>

#include "Player.h"
#include "PlayerPool.h"
#include "audio/SoundEngine.h"
#include "UnitAndStructurePool.h"
#include "anim_nfo.h"
#include "Structure.h"

namespace p {
	extern UnitAndStructurePool* uspool;
	extern PlayerPool* ppool;
}

BuildAnimEvent::BuildAnimEvent(Uint32 p, Structure* str, bool sell) : 
	BuildingAnimEvent(p, str, 0)
{
	updateDamaged();
	this->sell = sell;
	framend = getaniminfo().makenum;
	frame = (sell?(framend-1):1);
	structure = str;

	if (this->sell){
		printf ("Start sell animation\n");
	}
}

BuildAnimEvent::~BuildAnimEvent()
{
	Uint16 dmg_cost;

	// Remove the structure if it was sold
	if (this->sell)
    {
		//printf ("%s line %i: Remove sold structure: %li\n", __FILE__, __LINE__, (long) structure);
		p::uspool->removeStructure(structure);
		Uint16 Cost = structure->getType()->getCost();

		if (structure->getHealth() > 0) {
			dmg_cost = (Uint16)(((double)Cost/(double)structure->getType()->getMaxHealth()) * ((double)structure->getType()->getMaxHealth() - (double)structure->getHealth()));
        } else {
			dmg_cost = (Uint16)Cost;
        }

		p::ppool->getPlayer(structure->getOwner())->changeMoney(Cost - dmg_cost);
	}
}

void BuildAnimEvent::anim_func(anim_nfo* data)
{
	//    if (structure->buildAnim == NULL)
	//	structure->buildAnim = this;

    if (!sell)
    {
        if (frame < framend) {
            data->frame0 = frame;
            ++frame;
        } else {
            data->done = true;
            data->frame0 = getType()->getDefaultFace();
        }
    } else {
        if (frame > 0) {
            data->frame0 = frame;
            --frame;
        } else {
            data->done = true;
        }
    }
}
