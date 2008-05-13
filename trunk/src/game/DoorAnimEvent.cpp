#include "DoorAnimEvent.h"

#include <cmath>

#include "anim_nfo.h"
#include "Structure.h"
#include "include/Logger.h"
#include "UnitAndStructurePool.h"

extern Logger * logger;
namespace p {
	extern UnitAndStructurePool* uspool;
}

DoorAnimEvent::DoorAnimEvent(Uint32 p, Structure* str, bool opening) : BuildingAnimEvent(p, str, 5)
{
    updateDamaged();
    if (opening) {
        frame = framestart;
    } else {
        frame = framend;
    }
    this->opening = opening;
	strct = str;
	delayCounter = 0;
}

DoorAnimEvent::~DoorAnimEvent()
{
	if (this->opening){
		setDelay(20);
		strct->runSecAnim(5, false);
	}
}

void DoorAnimEvent::anim_func(anim_nfo* data)
{
	Uint8 subpos = 0;
	Uint16 pos;
	
	updateDamaged();
	
	if (opening) {
		if (frame < framend) {
			++frame;
		} else {
			delayCounter++;
			if (delayCounter > 15)
				data->done = true;

			if (delayCounter == 8){
				pos = strct->getFreePos(&subpos, strct->CreateUnitType->isInfantry());
				if (pos != 0xffff) {
					strct->runAnim(1);
					// (256 = FULLHEALTH)
					p::uspool->createUnit(strct->CreateUnitType, pos, subpos, strct->CreateUnitOwner, 256, 16);
				} else {
					logger->error("%s line %i: No free position for %s\n", __FILE__, __LINE__, strct->CreateUnitType->getTName());
				}
			}

		}
	} else {
		if (frame > framestart) {
			--frame;
		} else {
			frame = framestart;
			data->done = true;
		}
	}
	data->frame1 = frame;
	data->frame0 = frame0;
}

void DoorAnimEvent::updateDamaged()
{
    BuildingAnimEvent::updateDamaged();
    if (anim_data.damaged) {
        framestart = getaniminfo().loopend2+1;
        framend = framestart+getaniminfo().loopend2;
        if (frame < framestart) {
            frame += framestart;
        }
    } else {
        framestart = 0;
        framend = getaniminfo().loopend2;
    }
    frame0 = anim_data.damagedelta;
}
