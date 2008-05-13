#include "BExplodeAnimEvent.h"

#include <cmath>

#include "include/ccmap.h"
#include "include/config.h"
#include "audio/SoundEngine.h"
#include "game/UnitAndStructurePool.h"
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
	// TODO : spawn survivors and other goodies
	//printf ("%s line %i: Remove exploded structure: %i\n", __FILE__, __LINE__, (int) strct);
	p::uspool->removeStructure(strct);
}

void BExplodeAnimEvent::run()
{
	if ((counter == 0) && !(getType()->isWall()) && (pc::sfxeng != NULL) && !p::ccmap->isLoading())
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
