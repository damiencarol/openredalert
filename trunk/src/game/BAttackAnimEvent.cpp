
#include "BAttackAnimEvent.h"

#include <cmath>

#include "BTurnAnimEvent.h"
#include "include/ccmap.h"
#include "include/common.h"
#include "include/PlayerPool.h"
#include "include/ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "Unit.h"
#include "UnitAndStructurePool.h"
#include "include/weaponspool.h"
#include "anim_nfo.h"
#include "UnitOrStructure.h"
#include "ActionEventQueue.h"
#include "Structure.h"
#include "include/Logger.h"

extern Logger * logger;
namespace p
{
extern ActionEventQueue * aequeue;
}

BAttackAnimEvent::BAttackAnimEvent(Uint32 p, Structure *str) :
	BuildingAnimEvent(p, str, 8)
{
	this->strct = str;
	strct->referTo();
	this->target = str->getTarget();
	target->referTo();
	done = false;
	NeedToCharge = true;
	frame = 0;
	// StartFrame = strct->getImageNums()[0] ;
	StartFrame = 0;
}

BAttackAnimEvent::~BAttackAnimEvent()
{
	if (strct->getType()->Charges())
	{
		frame = StartFrame;
		if (strct->getNumbImages(0) > frame)
			strct->setImageNum(frame, 0);
	}

	target->unrefer();
	strct->unrefer();
	strct->attackAnim = NULL;
}

void BAttackAnimEvent::run()
{
	Uint32 distance;
	Sint32 xtiles, ytiles;
	Uint16 atkpos, mwid;
	float alpha;
	Uint8 facing;
	mwid = p::ccmap->getWidth();

	if ( !strct->isAlive() || done)
	{
		delete this;
		return;
	}

	if (!target->isAlive() || done)
	{
		if (strct->getType()->Charges())
		{
			if (strct->getNumbImages(0) > frame)
			{
				strct->setImageNum(frame, 0);
			}
			else
			{
				logger->error("%s line %i: Failed to set frame %i\n",__FILE__ , __LINE__, frame);
			}
		}
		delete this;
		return;
	}
	atkpos = target->getPos();

	xtiles = strct->getPos() % mwid - atkpos % mwid;
	ytiles = strct->getPos() / mwid - atkpos / mwid;
	
	distance = abs(xtiles)>abs(ytiles)?abs(xtiles):abs(ytiles);

	if( distance > strct->getType()->getWeapon()->getRange() )
	{
		// Since buildings can not move, give up for now.
		// Alternatively, we could just wait to see if the target ever
		// enters range (highly unlikely when the target is a structure)
		if (strct->getType()->Charges())
		{
			if (strct->getNumbImages (0)> frame)
			{
				strct->setImageNum(frame,0);
			}
			else
			{
				logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, frame);
			}
		}
		delete this;
		return;
	}
	//Make sure we're facing the right way
	if( xtiles == 0 )
	{
		if( ytiles < 0 )
		{
			alpha = -M_PI_2;
		}
		else
		{
			alpha = M_PI_2;
		}
	}
	else
	{
		alpha = atan((float)ytiles/(float)xtiles);
		if( xtiles < 0 )
		{
			alpha = M_PI+alpha;
		}
	}
	facing = (40-(Sint8)(alpha*16/M_PI))&0x1f;

	if ((strct->getType()->hasTurret())&&((strct->getImageNums()[0]&0x1f)!=facing))
	{ // turn to face target first
		setDelay(0);
		strct->buildAnim = new BTurnAnimEvent(strct->type->getTurnspeed(), strct, facing);
		strct->buildAnim->setSchedule(this, true);
		p::aequeue->scheduleEvent(strct->buildAnim);
		return;
	}

	//
	// This is the charging animation I only know of the tesla coil that uses it.
	//
	if (strct->getType()->Charges())
	{
		if (frame < StartFrame+8)
		{
			if (NeedToCharge)
			{
				frame = StartFrame;
				char *Snd = strct->getType()->getWeapon()->getChargingSound();
				if (Snd != NULL)
				pc::sfxeng->PlaySound(Snd);
				NeedToCharge = false;
			}
			if (strct->getNumbImages (0)> frame)
			{
				strct->setImageNum(frame,0);
			}
			else
			{
				logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, frame);
			}
			frame++;
			// Reschedule this..
			setDelay(3);
			p::aequeue->scheduleEvent(this);
			return;
		}
		frame = StartFrame;
		if (strct->getNumbImages (0)> frame)
		{
			strct->setImageNum(frame,0);
		}
		else
		{
			logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, frame);
		}
		NeedToCharge = true;
	}

	// We can shoot
	strct->getType()->getWeapon()->fire(strct, target->getBPos(strct->getPos()), target->getSubpos());
	setDelay(strct->getType()->getWeapon()->getReloadTime());
	p::aequeue->scheduleEvent(this);
}

void BAttackAnimEvent::stop()
{
	done = true;
}

void BAttackAnimEvent::update()
{
	target->unrefer();
	target = strct->getTarget();
	target->referTo();
}
