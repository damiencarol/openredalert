// BAttackAnimEvent.cpp
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

#include "BAttackAnimEvent.h"

#include <cmath>

#include "BTurnAnimEvent.h"
#include "CnCMap.h"
#include "include/common.h"
#include "PlayerPool.h"
#include "ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "Unit.h"
#include "UnitAndStructurePool.h"
#include "weaponspool.h"
#include "anim_nfo.h"
#include "UnitOrStructure.h"
#include "ActionEventQueue.h"
#include "Structure.h"
#include "include/Logger.h"

namespace p {
	extern ActionEventQueue * aequeue;
}
namespace pc {
	extern SoundEngine* sfxeng;
}
extern Logger * logger;

/**
 * @param p the priority of this event
 * @param str the attacking structure
 */
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
	strct->attackAnim = 0;
}

void BAttackAnimEvent::run()
{
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

	// @todo modify calculs
	//distance = abs()>abs(ytiles)?abs(xtiles):abs(ytiles);
	double distance = sqrt(xtiles*xtiles + ytiles*ytiles);

	if (distance > strct->getType()->getWeapon()->getRange())
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

	//
	// turn to face target first if this building have turret
	//
	if ((strct->getType()->hasTurret()) && ((strct->getImageNums()[0]&0x1f)!=facing) )
	{
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
				char* Snd = 0;
				Snd = strct->getType()->getWeapon()->getChargingSound();
				if (Snd != 0){
					pc::sfxeng->PlaySound(Snd);
					delete Snd;
				}
				Snd = 0;
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

	// Throw an event
	HandleTriggers(target, TRIGGER_EVENT_ATTACKED,
		    		p::ppool->getHouseNumByPlayerNum(strct->getOwner()));

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

/**
 */
void BAttackAnimEvent::anim_func(anim_nfo* data)
{
}
