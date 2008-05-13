#include "BuildingAnimEvent.h"

#include <cmath>

#include "ActionEventQueue.h"
#include "include/ccmap.h"
#include "audio/SoundEngine.h"
#include "BAttackAnimEvent.h"
#include "include/Logger.h"
#include "Structure.h"
#include "StructureType.h"
#include "animinfo_t.h"

extern Logger *logger;

namespace pc {
extern ConfigType Config;
}
namespace p {
extern ActionEventQueue* aequeue;
}

BuildingAnimEvent::BuildingAnimEvent(Uint32 p, Structure* str, Uint8 mode) :
	ActionEvent(p) 
{
	strct = str;
	strct->referTo();
	this->strct = strct;

	anim_data.done = false;
	anim_data.mode = mode;
	anim_data.frame0 = 0;
	anim_data.frame1 = 0;
	anim_data.damagedelta = 0;
	anim_data.damaged = false;
	
	toAttack = false;
	
	// no delay for building anim
	if (getaniminfo().animdelay != 0 && mode != 0) {
		setDelay(getaniminfo().animdelay);
	}
	e = NULL;
	ea = NULL;

	if (strct->type->getNumLayers()==2) {
		layer2 = true;
	} else {
		layer2 = false;
	}
}

BuildingAnimEvent::~BuildingAnimEvent() {
	if ((e != NULL)||(ea!=NULL)) {
		if (toAttack) {
			strct->attackAnim = ea;
			p::aequeue->scheduleEvent(ea);
		} else {
			strct->buildAnim = e;
			p::aequeue->scheduleEvent(e);
		}
	}
	strct->unrefer();
}

void BuildingAnimEvent::run() {
	//	BuildingAnimEvent* tmp_ev;

	if ( !strct->isAlive() ) {
		delete this;
		return;
	}
	anim_func(&anim_data);

	if (strct->getNumbImages(0) > anim_data.frame0)
		strct->setImageNum(anim_data.frame0, 0);
	else
		logger->error("%s line %i: Failed to set frame %i, numb frames = %i\n",__FILE__ , __LINE__, anim_data.frame0, strct->getNumbImages (0));
		if (layer2)
		{
			if (strct->getNumbImages (1)> anim_data.frame1)
			strct->setImageNum(anim_data.frame1,1);
			else
			logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, anim_data.frame1);
		}
		if (anim_data.done)
		{
			/// Handle ending the animation
			if (anim_data.mode != 6 && anim_data.mode != 5)
			{
				if (strct->getNumbImages (0)> anim_data.damagedelta)
				strct->setImageNum(anim_data.damagedelta,0);
				else
				logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, anim_data.damagedelta);
				if (layer2)
				{
					if (strct->getNumbImages (1)> anim_data.damagedelta2)
					strct->setImageNum(anim_data.damagedelta2,1);
					else
					logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, anim_data.damagedelta2);
				}
			}
			if (anim_data.mode == 0)
			{
				if (strct->getNumbImages (0)> anim_data.damagedelta + anim_data.frame0)
				strct->setImageNum(anim_data.damagedelta + anim_data.frame0,0);
				else
				logger->error ("%s line %i: Failed to set frame %i\n", __FILE__, __LINE__, anim_data.damagedelta + anim_data.frame0);
			}
			strct->usemakeimgs = false;
			if ((anim_data.mode == 0) || (anim_data.mode == 7) || (anim_data.mode == 8) )
			{
				switch (getaniminfo().animtype)
				{
					case 1:
					strct->animating = false;
					strct->runAnim (1);
					//tmp_ev = new LoopAnimEvent(getaniminfo().animspeed,strct);
					//setSchedule(tmp_ev);
					break;
					case 4:
					//tmp_ev = new ProcAnimEvent(getaniminfo().animspeed,strct);
					//setSchedule(tmp_ev);
					strct->animating = false;
					strct->runAnim (4);
					break;
					case 8:
					strct->animating = false;
					if (strct->backup_anim_mode != 0)
					strct->runAnim (strct->backup_anim_mode);
					strct->backup_anim_mode = 0;
					break;
					default:
					strct->animating = false;
					break;
				}
			}
			else if (e == NULL)
			{
				strct->animating = false;
			}
			delete this;
			return;
		}
		p::aequeue->scheduleEvent(this);
	}
void BuildingAnimEvent::updateDamaged() {
	bool odam = anim_data.damaged;
	anim_data.damaged = strct->checkdamage();
	if (anim_data.damaged) {
		if (getaniminfo().dmgoff != 0 || getaniminfo().dmgoff2 != 0) {
			anim_data.damagedelta = getaniminfo().dmgoff;
			anim_data.damagedelta2 = getaniminfo().dmgoff2;
		} else {
			anim_data.damagedelta = getaniminfo().loopend+1;
			if (layer2) {
				anim_data.damagedelta2 = getaniminfo().loopend2+1;
			}
		}
		if (!odam && pc::sfxeng != NULL && !p::ccmap->isLoading()) {
			pc::sfxeng->PlaySound(pc::Config.StructureDestroyed);
		}
	} else {
		anim_data.damagedelta = 0;
		anim_data.damagedelta2 = 0;
	}
}
animinfo_t BuildingAnimEvent::getaniminfo() {
	return ((StructureType *)strct->getType())->getAnimInfo();
}
StructureType* BuildingAnimEvent::getType() {
	return strct->type;
}

void BuildingAnimEvent::update() {
}

void BuildingAnimEvent::setSchedule(BuildingAnimEvent* e) {
	this->e = e;
}

void BuildingAnimEvent::setSchedule(BAttackAnimEvent* ea, bool attack) {
	if (attack) {
		this->ea = ea;
	} else {
		this->e = (BuildingAnimEvent*)ea;
	}
	toAttack = attack;
}

void BuildingAnimEvent::stop() {
	anim_data.done = true;
}
