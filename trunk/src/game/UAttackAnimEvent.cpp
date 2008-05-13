#include "UAttackAnimEvent.h"

#include <cmath>

#include "SDL/SDL_types.h"

#include "ActionEventQueue.h"
#include "UnitOrStructure.h"
#include "TurnAnimEvent.h"
#include "unittypes.h"
#include "MoveAnimEvent.h"
#include "Projectile.h"
#include "Weapon.h"

#include "game/Unit.h"
#include "include/ccmap.h"
#include "include/Logger.h"
#include "Unit.h"

namespace p {
	extern ActionEventQueue * aequeue;
	extern CnCMap * ccmap;
}
extern Logger * logger;

UAttackAnimEvent::UAttackAnimEvent(Uint32 p, Unit *un) : UnitAnimEvent(p,un)
{
    //logger->debug("UAttack cons\n");
    this->un = un;
    this->target = un->getTarget();
    stopping = false;
    waiting = 0;
    target->referTo();
	Weapon *Weap;

	UsePrimaryWeapon = true;

	// Determine the weapon to use
	if (!target->getType()->isStructure()){
		switch (((Unit*)target)->getType()->getType()){
			case UN_INFANTRY:
			case UN_VEHICLE:
				Weap = un->getType()->getWeapon();
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiGround()){
						UsePrimaryWeapon = false;
					}
				}
				break;
			case UN_BOAT:
				Weap = un->getType()->getWeapon();
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiGround()){
						UsePrimaryWeapon = false;
					}
				}
				break;
			case UN_PLANE:
			case UN_HELICOPTER:
				Weap = un->getType()->getWeapon();
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiAir()){
						UsePrimaryWeapon = false;
					}
				}
				break;
			default:
				logger->error ("%s line %i: ERROR unknown unit type %i\n", __FILE__, __LINE__, ((Unit*)target)->getType()->getType());
				break;
		}
	}else{
		Weap = un->getType()->getWeapon();
		if (Weap != NULL){
			if (!Weap->getProjectile()->AntiGround()){
				UsePrimaryWeapon = false;
			}
		}
	}
	if (UsePrimaryWeapon == false){
//		printf ("%s line %i: Using secundary weapon\n", __FILE__, __LINE__);
		if (un->getType()->getWeapon(UsePrimaryWeapon) == NULL){
			logger->error ("Primary weapon not oke, secundary weapon not available\n");
			UsePrimaryWeapon = true;
			if (un->getType()->getWeapon(UsePrimaryWeapon) == NULL){
				stop();
			}
		}
	}
}

UAttackAnimEvent::~UAttackAnimEvent()
{
    //logger->debug("UAttack dest\n");
    target->unrefer();
    if (un->attackanim == this)
        un->attackanim = NULL;
}

void UAttackAnimEvent::stop()
{
    if (un == NULL) {
        logger->error("UAttackAnimEvent::stop: un is NULL!?\n");
        abort();
    }
    stopping = true;
}

void UAttackAnimEvent::update()
{
    //logger->debug("UAtk updating\n");
    target->unrefer();
    target = un->getTarget();
    target->referTo();
    stopping = false;
}

void UAttackAnimEvent::run()
{
    Uint32 distance;
    Sint32 xtiles, ytiles;
    Uint16 atkpos;
    float alpha;
    Uint8 facing;
#ifdef LOOPEND_TURN
    Uint8 loopend2=((UnitType*)un->type)->getAnimInfo().loopend2;
#endif

    //logger->debug("attack run t%p u%p\n",this,un);
    waiting = 0;
    if( !un->isAlive() || stopping ) {
        delete this;
        return;
    }

    if( !target->isAlive() || stopping) {
        if ( !target->isAlive() ) {
            un->doRandTalk(TB_postkill);
        }
        delete this;
        return;
    }

	// Check if we have a healing weapon (medic) and if we have healed someone completely
	if (un->getType()->getWeapon(UsePrimaryWeapon)->getDamage() < 0){
		// Oke, this is a healing weapon
		if ( target->getHealth() ==  target->getType()->getMaxHealth()){
			delete this;
			return;
		}
	}

    atkpos = un->getTargetCell();

    xtiles = un->cellpos % p::ccmap->getWidth() - atkpos % p::ccmap->getWidth();
    ytiles = un->cellpos / p::ccmap->getWidth() - atkpos / p::ccmap->getWidth();
    distance = abs(xtiles)>abs(ytiles)?abs(xtiles):abs(ytiles);

    if( distance > un->type->getWeapon(UsePrimaryWeapon)->getRange() /* weapons range */ ) {
        setDelay(0);
        waiting = 3;
        un->move(atkpos,false);
        un->moveanim->setRange(un->type->getWeapon(UsePrimaryWeapon)->getRange());
        un->moveanim->setSchedule(this);
        return;
    }
    //Make sure we're facing the right way
    if( xtiles == 0 ) {
        if( ytiles < 0 ) {
            alpha = -1.57079632679489661923;
        } else {
            alpha = 1.57079632679489661923;
        }
    } else {
        alpha = atan((float)ytiles/(float)xtiles);
        if( xtiles < 0 ) {
            alpha = 3.14159265358979323846+alpha;
        }
    }
#ifdef LOOPEND_TURN
    facing = ((Sint8)((loopend2+1)*(1-alpha/2/3.14159265358979323846)+8))&loopend2;
    if (un->type->isInfantry()) {
        if (facing != (un->getImageNum(0)&loopend2)) {
            un->setImageNum((Sint8)((loopend2+1)*facing/8),0);
        }
    } else if (un->type->getNumLayers() > 1 ) {
        if (abs((int)(facing - (un->getImageNum(1)&loopend2))) > un->type->getROT()) {
#else
    facing = (40-(Sint8)(alpha*16/M_PI))&0x1f;
    if (un->type->isInfantry()) {
        if (facing != (un->getImageNum(0)&0x1f)) {
			un->setImageNum(facing>>2,0);
        }
    } else if (un->type->getNumLayers() > 1 ) {
        if (abs((int)(facing - (un->getImageNum(1)&0x1f))) > un->type->getROT()) {
#endif
            setDelay(0);
            waiting = 2;
            un->turn(facing,1);
            un->turnanim2->setSchedule(this);
            return;
        }
    } else {
#ifdef LOOPEND_TURN
        if (abs((int)(facing - un->getImageNum(0)&loopend2)) > un->type->getROT()) {
#else
        if (abs((int)(facing - un->getImageNum(0)&0x1f)) > un->type->getROT()) {
#endif
            setDelay(0);
            waiting = 1;
            un->turn(facing,0);
            un->turnanim1->setSchedule(this);
            return;
        }
    }

	// If we have a healing weapon we don't want to heal a enemy
	if (un->getType()->getWeapon(UsePrimaryWeapon)->getDamage() < 0){
		if (un->getOwner() != target->getOwner()){
			delete this;
			return;
		}
	}

    // We can shoot
    un->type->getWeapon(UsePrimaryWeapon)->fire(un, target->getBPos(un->getPos()), target->getSubpos());
    // set delay to reloadtime
    setDelay(un->type->getWeapon(UsePrimaryWeapon)->getReloadTime());
    waiting = 4;
    p::aequeue->scheduleEvent(this);
}
