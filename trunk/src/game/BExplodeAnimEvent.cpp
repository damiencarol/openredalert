// BExplodeAnimEvent.cpp
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

#include "BExplodeAnimEvent.h"

#include <cmath>

#include "CnCMap.h"
#include "include/config.h"
#include "audio/SoundEngine.h"
#include "UnitAndStructurePool.h"
#include "anim_nfo.h"
#include "Structure.h"

namespace pc
{
	extern ConfigType Config;
	extern SoundEngine* sfxeng;
}
namespace p
{
	extern UnitAndStructurePool* uspool;
}

/**
 * 
 */
BExplodeAnimEvent::BExplodeAnimEvent(Uint32 p, Structure* str) :
	BuildingAnimEvent(p, str, 9)
{
	this->strct = str;
	if (getType()->isWall())
	{
		lastframe = strct->getImageNums()[0];
	}
	else
	{
		lastframe = getType()->getSHPTNum()[0]-1;
	}
	counter = 0;
	setDelay(1);
}

BExplodeAnimEvent::~BExplodeAnimEvent()
{
	// @todo : spawn survivors and other goodies
	//printf ("%s line %i: Remove exploded structure: %i\n", __FILE__, __LINE__, (int) strct);
	p::uspool->removeStructure(strct);
}

void BExplodeAnimEvent::run()
{
	if ((counter == 0) && !(getType()->isWall()) && (pc::sfxeng != 0) && !p::ccmap->isLoading())
	{
		pc::sfxeng->PlaySound(pc::Config.StructureDestroyed);
		// add code to draw flames
	}
	BuildingAnimEvent::run();
}

void BExplodeAnimEvent::anim_func(anim_nfo* data)
{
	++counter;
	data->frame0 = lastframe;
	if (counter < 10)
	{
		data->done = false;
	}
	else
	{
		data->done = true;
	}
}
