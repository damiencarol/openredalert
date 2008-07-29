// Structure.cpp
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

#include "Structure.h"

#include <cstdlib>
#include <cstring>

#include "SDL/SDL_timer.h"

#include "CnCMap.h"
#include "PlayerPool.h"
#include "Player.h"
#include "BTurnAnimEvent.h"
#include "BExplodeAnimEvent.h"
#include "BuildAnimEvent.h"
#include "DoorAnimEvent.h"
#include "BuildingAnimEvent.h"
#include "audio/SoundEngine.h"
#include "BAttackAnimEvent.h"
#include "UnitAndStructurePool.h"
#include "weaponspool.h"
#include "Dispatcher.h"
#include "BRepairUnitAnimEvent.h"
#include "weaponspool.h"
#include "StructureType.h"
#include "GameMode.h"
#include "UnitOrStructure.h"
#include "RefineAnimEvent.h"
#include "LoopAnimEvent.h"
#include "RepairAnimEvent.h"
#include "ProcAnimEvent.h"
#include "Ai.h"
#include "include/Logger.h"
#include "unittypes.h"
#include "Projectile.h"
#include "Weapon.h"
#include "Unit.h"
#include "InfantryGroup.h"
#include "include/config.h"
#include "ActionEventQueue.h"
#include "BarrelExplosionActionEvent.h"

namespace p {
	extern ActionEventQueue* aequeue;
	extern CnCMap* ccmap;
}
namespace pc {
    extern ConfigType Config;
    extern Ai * ai;
    extern SoundEngine* sfxeng;
}
extern Logger * logger;

Structure::Structure(StructureType *type, Uint16 cellpos, Uint8 owner,
        Uint16 rhealth, Uint8 facing, string trigger_name) : UnitOrStructure()
{
    Uint32 i;
    
    
    targetcell = cellpos;
    this->type = type;
    imagenumbers = new Uint16[type->getNumLayers()];
    if (!type->hasTurret()) {
        facing = 0;
    }
    for(i=0;i<type->getNumLayers();i++) {
        imagenumbers[i] = facing;
        if( owner != 0xff && !type->isWall() ) {
            imagenumbers[i] |= (p::ppool->getStructpalNum(owner)<<11);
        }
    }
    exploding = false;
    this->owner = owner;
    this->cellpos = cellpos;
    bcellpos = cellpos+(type->getBlckOff() % type->getXsize()) +
               ((type->getBlckOff()/type->getXsize())*p::ccmap->getWidth());
	animating = false;
	usemakeimgs = false;
	primary = false;
	buildAnim = NULL;
	attackAnim = NULL;
	health = (Uint16)((double)rhealth/256.0f * (double)type->getMaxHealth());
	damaged = checkdamage();
	if( !type->isWall() ) {
		p::ppool->getPlayer(owner)->builtStruct(this);
	}

	// Handle deploy with...
	for (unsigned int j = 0; j < type->getDeployWith().size(); j++){
		//
		Uint16 x = cellpos%p::ccmap->getWidth();
		Uint16 y = cellpos/p::ccmap->getWidth();
		y += 2;
		int temppos = p::ccmap->translateToPos(x, y);
		while ( !p::ccmap->isBuildableAt(this->getOwner(), temppos) && y < p::ccmap->getHeight() ){
			y += 1;
			temppos = p::ccmap->translateToPos(x, y);
		}

		if (p::ccmap->isBuildableAt(this->getOwner(), temppos)){
			// Health = 256 --> max
			p::uspool->createUnit(this->type->getDeployWith()[j]/*"HARV"*/, 
					temppos, 0, owner, 256, 0, 0, "None");
		}

		// If the current structure is a PROC and we are placing a extra harvester
		if (strcmp ((char*)this->type->getDeployWith()[j], "HARV") == 0 && strcmp ((char*)type->getTName(), "PROC") == 0) {

			// Set base refinary
			if (p::uspool->getUnitAt(temppos, 0) != NULL)
				p::uspool->getUnitAt(temppos, 0)->SetBaseRefinery (this);
		}


	}

	retry_sell = false;
	retry_repair = false;
	repairing = false;

	backup_anim_mode = 0;

	TriggerName = trigger_name;

	repairunitAnim = NULL;
	
	// When created a structure is not bombed :)
	bombing = false;
}

Structure::~Structure()
{
	// Free imagenumbers
	if (imagenumbers != NULL){
		delete[] imagenumbers;
	}
	imagenumbers = NULL;
}

/** 
 * Method to get a list of imagenumbers which the renderer will draw.
 */
Uint8 Structure::getImageNums(Uint32 **inums, Sint8 **xoffsets, Sint8 **yoffsets) {
    Uint16 *shps;
    int i;

	if (retry_sell)
		runAnim(9);

	if (retry_repair)
		runAnim(8);

	shps = type->getSHPNums();

//	printf ("Get image, imagenumbers[0] = %i\n", imagenumbers[0]);

	if (usemakeimgs && (!type->isWall()) && (type->getMakeImg() != 0)) {
		*inums = new Uint32[1];
		*xoffsets = new Sint8[1];
		*yoffsets = new Sint8[1];
		(*inums)[0] = (type->getMakeImg()<<16)|imagenumbers[0];
		(*xoffsets)[0] = type->getXoffset();
		(*yoffsets)[0] = type->getYoffset();
		return 1;
	} else {
		*inums = new Uint32[type->getNumLayers()];
		*xoffsets = new Sint8[type->getNumLayers()];
		*yoffsets = new Sint8[type->getNumLayers()];
		for(i = 0; i < type->getNumLayers(); i++ ) {
			(*inums)[i] = (shps[i]<<16)|imagenumbers[i];
			(*xoffsets)[i] = type->getXoffset();
			(*yoffsets)[i] = type->getYoffset();
		}
		return type->getNumLayers();
	}
}

Uint16 Structure::getNumbImages(Uint8 layer)
{
	if (!usemakeimgs){
		if (type->getSHPTNum() != 0){
			return type->getSHPTNum()[layer];
		}
	} else {
		return type->getAnimInfo().makenum;
	}
	return 0;
}

void Structure::setImageNum(Uint32 num, Uint8 layer)
{
//	if (strcmp ((char*)type->getTName(), "PROC") == 0)
//		printf ("%s line %i: Set image numb %i, layer = %i\n", __FILE__, __LINE__, num, layer);
	if (getNumbImages( layer ) > num)
		imagenumbers[layer]=(num)|(p::ppool->getStructpalNum(owner)<<11);
	else {
		logger->error ("%s line %i: Failed to set frame %i layer %i, numb frames = %i\n", __FILE__, __LINE__, num, layer, getNumbImages( layer ));
	}
}

Uint16 Structure::getBPos(Uint16 pos) const
{
    Uint16 x,y,dx,dy,t,retpos,bpos,sc;
    Sint16 dw;
    Uint32 mwid = p::ccmap->getWidth();
    x = cellpos%mwid;
    dx = 0;
    if ((pos%mwid) > x) {
        dx = min((unsigned)(type->getXsize()-1),(unsigned)((pos%mwid)-x)-1);
    }
    y = cellpos/mwid;
    dy = 0;
    if ((pos/mwid) > y) {
        dy = min((unsigned)(type->getYsize()-1),(unsigned)((pos/mwid)-y)-1);
    }
    retpos = (x+dx)+(y+dy)*mwid;
    // just makes the bpos calculation cleaner
    sc = x+y*mwid;
    dw = type->getXsize() - mwid;
    bpos   = retpos - sc + dy*dw;
    while (!type->isBlocked(dx+dy*type->getXsize())) {
        /* This happens in this situation (P is position of attacker,
         * X is a blocked cell and _ is an unblocked cell)
         * P   P
         *  _X_
         *  XXX
         *  _X_
         * P   P
         */
        if (dx == type->getXsize()-1) {
            for (t=dx;t>0;--t) {
                retpos = (x+t)+(y+dy)*mwid;
                bpos   = retpos - sc + dy*dw;
                if (type->isBlocked(bpos)) {
                    return retpos;
                }
            }
        } else {
            for (t=dx;t<type->getXsize();++t) {
                retpos = (x+t)+(y+dy)*mwid;
                bpos   = retpos - sc + dy*dw;
                if (type->isBlocked(bpos)) {
                    return retpos;
                }
            }
        }
        ++dy;
        if (dy >= type->getYsize()) {
            logger->error("ERROR: could not find anywhere to shoot at %s!\n",type->getTName());
        }
        retpos = (x+dx)+(y+dy)*mwid;
    }
    return retpos;
}
Uint16 Structure::getFreePos(Uint8* subpos, bool findsubpos) {
    bool (*checker)(StructureType*, Uint8, Uint16, Uint8*);
    Uint8 i, xsize, ysize;
    Uint16 x, y, curpos;

    xsize = type->getXsize();
    ysize = type->getYsize();

    curpos = cellpos;
    p::ccmap->translateFromPos(curpos, &x, &y);
    y += ysize; // bottom left of building


    if (findsubpos) {
        checker = valid_possubpos;
    } else {
        checker = valid_pos;
    }

	// First check the optimal position
	//y -= 1;
    curpos = p::ccmap->translateToPos(x+1,y-1);
//	if (checker(type, this->getOwner(), curpos, subpos))
	if (p::ccmap->getCost(curpos, NULL) < 2)
    	return curpos;


    curpos = p::ccmap->translateToPos(x,y);


    if (!checker(type, this->getOwner(), curpos, subpos))
        curpos = 0xffff;


    for (i = 0; (i < xsize && 0xffff == curpos); ++i) {
        ++x;
        curpos = p::ccmap->translateToPos(x, y);
        if (!checker(type, this->getOwner(), curpos, subpos))
            curpos = 0xffff;
    }
    // ugly: I assume that the first blocks are noblocked - g0th
    for (i = 0;(i < ysize && 0xffff == curpos); ++i) {
        --y;
        curpos = p::ccmap->translateToPos(x, y);
        if (!checker(type, this->getOwner(), curpos, subpos))
            curpos = 0xffff;
    }
    for (i = 0; (i < (xsize+1) && 0xffff == curpos); ++i) {
        --x;
        curpos = p::ccmap->translateToPos(x, y);
        if (!checker(type, this->getOwner(), curpos, subpos))
            curpos=0xffff;
    }
    for (i = 0; (i < ysize && 0xffff == curpos); ++i) {
        ++y;
        curpos = p::ccmap->translateToPos(x, y);
        if (!checker(type, this->getOwner(), curpos, subpos))
            curpos=0xffff;
    }
    return curpos;
}

void Structure::remove() 
{
    if (!type->isWall()) {
        p::ppool->getPlayer(owner)->lostStruct(this);
    }
    UnitOrStructure::remove();
}

void Structure::applyDamage(Sint16 amount, Weapon* weap, UnitOrStructure* attacker)
{
	bool odam;	// Old damaged

	if (exploding){
		return;
	}
	
	if (this->getOwner() != p::ppool->getLPlayerNum()){
		// This structure is from a computer player
		if (p::ccmap->getGameMode() != GAME_MODE_SINGLE_PLAYER){
			pc::ai->DefendComputerPlayerBaseUnderAttack ( p::ppool->getPlayer(this->getOwner()), this->getOwner(), attacker, this );
		}
	}else {
		// The structure is from the local player
		if ((SDL_GetTicks() - LastAttackTick) > (60 * 1000)){
		    pc::sfxeng->PlaySound(pc::Config.BaseUnderAttack);
			LastAttackTick = SDL_GetTicks();
		}
	}


	odam = damaged;
	// If the Weapon exist
	if (weap != 0){
		amount = (Sint16)((double)amount * weap->getVersus(type->getArmor()));
	}
	
	if ((health-amount) <= 0) 
	{

		exploding = true;

		// Throw the event (-1 means nothing)
		int houseNum = -1;
		if (attacker != 0){
			p::ppool->getHouseNumByPlayerNum(attacker->getOwner());
		}
		HandleTriggers((UnitOrStructure*)this, TRIGGER_EVENT_DESTROYED, houseNum);

		if (type->isWall()) {
			p::uspool->removeStructure(this);
		} else {
			if (attacker != 0){
				p::ppool->getPlayer(attacker->getOwner())->addStructureKill();
			}
			//printf ("%s line %i: Start explode anim for structure: %i\n", __FILE__, __LINE__, (int) this);
			BExplodeAnimEvent* boom = new BExplodeAnimEvent(1, this);
			if (animating) {
				buildAnim->setSchedule(boom);
				buildAnim->stop();
			} else {
				buildAnim = boom;
				p::aequeue->scheduleEvent(boom);
			}
			
			// If it was a barrel
			if (string(type->getTName()) == "BARL" ||
				string(type->getTName()) == "BRL3")
			{
				// Create the barrel Explosion Anim
				new BarrelExplosionActionEvent(3, this->getPos());
			}
		}
		return;
	} else if ((health-amount)>type->getMaxHealth()) {
		health = type->getMaxHealth();
	} else {
		health -= amount;
	}

	damaged = checkdamage();

	//printf ("Apply damage, ratio = %02f, damaged = %i\n", ratio, damaged);
    if (animating) {
		//printf ("%s line %i: animating\n", __FILE__, __LINE__);
        buildAnim->updateDamaged();
	} else {
		if (type->isWall()) {
			if ( damaged ) { // This is correct
                if (odam != damaged) {
					changeImage(0,16);
				}
			}
		} else {
			if (damaged) { // This is correct
				Uint32 ImageNr = (unsigned)(imagenumbers[0]&~0x800);
                // only play critical damage sound once
				if (odam != damaged) {
                    if (pc::sfxeng != NULL) {
						pc::sfxeng->PlaySound(pc::Config.StructureHalfDestroyed);
					}

					if (getNumbImages( 0 ) > (unsigned)(imagenumbers[0]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff)
						setImageNum((unsigned)(imagenumbers[0]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff,0);
					else
						logger->error ("%s line %i: FAILED imagenr = %u, type->getAnimInfo().dmgoff = %u, ImgNr == %u\n", __FILE__, __LINE__, (unsigned)(imagenumbers[0]&0x7FF), (unsigned)type->getAnimInfo().dmgoff, ImageNr);

					if (type->getNumLayers() == 2){
						if (getNumbImages( 1 ) > (unsigned)(imagenumbers[1]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff2)
							setImageNum((unsigned)(imagenumbers[1]&0x7FF)+ (unsigned)type->getAnimInfo().dmgoff2,1);
						else
							logger->error ("%s line %i: Failed to set frame %i layer %i, numb frames = %i\n", __FILE__, __LINE__, (imagenumbers[1]&~0x800)+type->getAnimInfo().dmgoff2, 1, getNumbImages( 1 ));
					}
				}
			} else {
				// We used to have damage (no more?)
    			if (odam) {
					if (getNumbImages( 0 ) > (unsigned)(imagenumbers[0]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff){
						setImageNum((unsigned)(imagenumbers[0]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff,0);
                    } else {
                        logger->error ("%s line %i: Failed to set frame %i layer %i, numb frames = %i\n", __FILE__, __LINE__, (imagenumbers[0]&~0x800)-type->getAnimInfo().dmgoff, 0, getNumbImages( 0 ));
                    }
					if (type->getNumLayers() == 2){
						if (getNumbImages( 1 ) > (unsigned)(imagenumbers[1]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff)
							setImageNum((unsigned)(imagenumbers[1]&0x7FF) + (unsigned)type->getAnimInfo().dmgoff,1);
						else
							logger->error ("%s line %i: Failed to set frame %i layer %i, numb frames = %i\n", __FILE__, __LINE__, (imagenumbers[1]&~0x800)-type->getAnimInfo().dmgoff, 1, getNumbImages( 1 ));
					}
					return;
				}
			}
		}
	}
}

/**
 * If mode = 0 => BUILD ANIM
 * 
 * If mode = 9 => RETRY SELL
 * 
 * If mode = 8 => RETRY REPAIR
 */
void Structure::runAnim(Uint32 mode)
{
	Uint32 speed;

	// If we want to sell, stop current animations
	if (mode == 9 && animating ){
		this->stopAnim();
		retry_sell = true;
		return;
	}

	if (mode == 8 && animating){
		this->stopAnim();
		retry_repair = true;
		backup_anim_mode = buildAnim->anim_data.mode;
		return;
	}


	if (repairunitAnim != 0){
		return;
	}
	
	if (!animating) {
		animating = true;
		if (mode == 0) { // run build anim at const speed
			usemakeimgs = true;
			buildAnim = new BuildAnimEvent(3,this,false);
		} else if (mode == 9) { // run sell anim at const speed
			usemakeimgs = true;
			// Set the image number to the correct value for using make images ;)
			if (getNumbImages( 0 ) > type->getAnimInfo().makenum - 1)
				setImageNum(type->getAnimInfo().makenum - 1, 0);
			else
				logger->error ("%s line %i: Failed to set frame %i layer %i, numb frames = %i\n", __FILE__, __LINE__, type->getAnimInfo().makenum - 1, 0, getNumbImages( 0 ));
			buildAnim = new BuildAnimEvent(3,this,true);
			retry_sell = false;
		}  else if (mode == 8) { // run repair anim at const speed
			buildAnim = new RepairAnimEvent(3,this);
			retry_repair = false;
		} else if (mode == 10){
			buildAnim = new BRepairUnitAnimEvent(3,this);
		}else {
			speed = type->getAnimInfo().animspeed;
			switch (mode&0xf) {
				case 1:
					if (type->getAnimInfo().animtype == 4) {
//						logger->note ("%s line %i: DEBUG: run anim mode 1 (proc anim event)\n",__FILE__, __LINE__);
						buildAnim = new ProcAnimEvent(speed,this);
					} else {
//						logger->note ("%s line %i: DEBUG: run anim mode 1 (loop anim event)\n",__FILE__, __LINE__);
						buildAnim = new LoopAnimEvent(speed,this);
					}
					break;
				case 2:
                    // logger->note ("%s line %i: DEBUG: run anim mode 2 (turn anim event)\n",__FILE__, __LINE__);
					buildAnim = new BTurnAnimEvent(speed,this,(mode>>4));
					break;
				case 7:
//					logger->note ("%s line %i: DEBUG: run anim mode 7 (refine anim event)\n",__FILE__, __LINE__);
					buildAnim = new RefineAnimEvent(speed,this,5);
					break;
				default:
					buildAnim = 0;
					animating = false;
					break;
			}
		}
        if (buildAnim != 0)
        {
            p::aequeue->scheduleEvent(buildAnim);
		}
	}
	//else
	//	logger->note ("%s line %i: Warning: already animating\n", __FILE__, __LINE__);
}

void Structure::runSecAnim(Uint32 param, bool extraParam)
{	
    BuildingAnimEvent* sec_anim = 0;
    Uint8 secmode = type->getAnimInfo().sectype;
    if (secmode != 0)
    {
        switch (secmode) {
        case 7:
            sec_anim = new RefineAnimEvent(2, this, param);
            break;
        case 5:
            sec_anim = new DoorAnimEvent(2, this, extraParam);
            break;
//        case 8:
//            sec_anim = new RepairAnimEvent(3, this);
//            break;
        }
        if (animating) {
            buildAnim->setSchedule(sec_anim);
            stopAnim();
        } else {
            buildAnim = sec_anim;
            p::aequeue->scheduleEvent(buildAnim);
            animating = true;

            if (this->is("BARR")){
                printf ("!!!!!!!!!!!start animating\n");
            }
        }
    }
}

void Structure::stopAnim()
{
    buildAnim->stop();
}

void Structure::stop()
{
    if (attackAnim != NULL) {
		attackAnim->stop();
    }
}

bool Structure::IsAttacking ()
{
    if (attackAnim == 0){
        return false;
    }
    return true;
}

void Structure::attack(UnitOrStructure* target)
{
	Weapon *Weap;

    // Don't attack while the structure is being created
	if (usemakeimgs){
		return;
	}
	
	// Only attack if we have a weapon
	if (!this->canAttack()){
		return;
	}
	
	if (!target->getType()->isStructure())
	{
		switch (((Unit*)target)->getType()->getType()){
			case UN_INFANTRY:
			case UN_VEHICLE:
				Weap = type->getWeapon();
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiGround()){
						Weap = type->getWeapon(false);
						if (Weap != NULL){
							if (!Weap->getProjectile()->AntiGround())
								return;
						}else
							return;
					}
				}else
					return;
				break;
			case UN_BOAT:
				Weap = type->getWeapon();
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiGround()){
						Weap = type->getWeapon(false);
						if (Weap != NULL){
							if (!Weap->getProjectile()->AntiGround())
								return;
						}else
							return;
					}
				}else
					return;
				break;
			case UN_PLANE:
			case UN_HELICOPTER:
				Weap = type->getWeapon();
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiAir()){
						Weap = type->getWeapon(false);
						if (Weap != NULL){
							if (!Weap->getProjectile()->AntiAir())
								return;
						}else
							return;
					}
				}else
					return;
				break;
			default:
				printf ("%s line %i: ERROR unknown unit type\n", __FILE__, __LINE__);
				break;

		}
	}else{
		Weap = type->getWeapon();
		if (Weap != NULL){
			if (!Weap->getProjectile()->AntiGround()){
				Weap = type->getWeapon(false);
				if (Weap != NULL){
					if (!Weap->getProjectile()->AntiGround())
						return;
				}else
					return;
			}
		}else
			return;
	}
#if 0
	// Check that we have power enough to attack
	if ( type->getPowerInfo().powered){
		if ( p::ppool->getPlayer(this->getOwner())->getPower() < p::ppool->getPlayer(this->getOwner())->getPowerUsed()){
			return;
		}
	}

	// printf ("%s line %i: PowerNeeded = %i, Power = %i, Player = %i\n", __FILE__, __LINE__, p::ppool->getPlayer(this->getOwner())->getPowerUsed(), p::ppool->getPlayer(this->getOwner())->getPower(), this->getOwner());
#endif



    this->target = target;
    targetcell = target->getBPos(cellpos);
    if( attackAnim == NULL ) {
        attackAnim = new BAttackAnimEvent(0, this);
        p::aequeue->scheduleEvent(attackAnim);
    } else {
        attackAnim->update();
    }
}
void Structure::ChangeHealth (Sint16 amount)
{
//	printf ("%s line %i: original health = %i\n", __FILE__, __LINE__, health);
	if (health + amount > type->getMaxHealth()){
		health = type->getMaxHealth();
	}else if (health + amount < 0){
		health = 0;
	}else
		health += amount;

//	printf ("%s line %i: health = %i\n", __FILE__, __LINE__, health);
}

/**
 */
bool Structure::CreateUnitAnimation (UnitType* UnType, Uint8 owner)
{
    Player* player = p::ppool->getPlayer(owner);
    assert(player != 0);
    Structure* tmpstruct = player->getPrimary(type);
	if (tmpstruct != this){
		return false;
    }
	CreateUnitType	= UnType;
	CreateUnitOwner	= owner;

 	if(!UnType->isInfantry() ){
		tmpstruct->runSecAnim(5, true);
		return true;
	}
	return false;
}

/**
 */
bool Structure::RepairUnint (Unit *Un)
{
	if (strcmp ((char*)this->getType()->getTName(), "FIX") != 0 )
		return false;

	if (Un->getOwner() != getOwner())
		return false;

	UnitToRepairPos = Un->getPos();
    pc::sfxeng->PlaySound(pc::Config.RepairUnit);

    // runAnim(10);
	if (repairunitAnim == NULL && !animating){
		repairunitAnim = new BRepairUnitAnimEvent(3,this);
        p::aequeue->scheduleEvent(repairunitAnim);
	}else{
		printf ("%s line %i: Already animating\n", __FILE__, __LINE__);
	}

	return true;
}
Uint32 Structure::getExitCell() const
{
    return cellpos+(type->getYsize()*p::ccmap->getWidth());
}
bool Structure::checkdamage()
{
    ratio = ((double)health)/((double)type->getMaxHealth());
    if (type->isWall()) {
        if ((ratio <= 0.33)&&(type->getNumWallLevels() == 3))
            return 2;
        else
            return ((ratio <= 0.66)&&(type->getNumWallLevels() > 1));
    } else {
    	//printf ("Structure damage ratio = %.2f\n", ratio);
        return (ratio <= 0.5);
    }
}

Uint16 Structure::getTargetCell() const
{
    if (attackAnim != NULL && target != NULL) {
        return target->getBPos(cellpos);
    }
    return targetcell;
}

bool Structure::is(string Name) 
{
	if (string(getType()->getTName()) == Name){
		return true;
	}
	return false;
}

void Structure::sell()
{
	printf ("%s line %i: Start sell animation\n", __FILE__, __LINE__);
	runAnim(9);
}

void Structure::repair()
{
	printf ("%s line %i: Start repair animation\n", __FILE__, __LINE__);
	runAnim(8);
	repairing = true;
//	runSecAnim (8);
}

bool Structure::isRepairing ()
{
	return repairing;
}

void Structure::repairDone()
{
	repairing = false;
}

void Structure::bomb()
{
	printf ("%s line %i: Start bomb animation\n", __FILE__, __LINE__);
	// @todo THE BOMBING ANIM
	//runAnim(8);
	bombing = true;
//	runSecAnim (8);
}

bool Structure::isBombing()
{
	return bombing;
}

void Structure::bombingDone()
{
	bombing = false;
}

/** 
 * Helper function for getFreePos
 * 
 * @bug Doesn't check that the terrain is passable (only buildable).
 */
bool Structure::valid_pos(StructureType *type, Uint8 PlayerNr, Uint16 pos, Uint8*) {
    return p::ccmap->isBuildableAt( PlayerNr, pos, type->isWaterBound());
}

/**
 * Helper function for getFreePos
 *
 * @bug Doesn't check that the terrain is passable (only buildable).
 */
bool Structure::valid_possubpos(StructureType *type, Uint8 PlayerNr, Uint16 pos, Uint8* subpos)
{
    InfantryGroup *ig = p::uspool->getInfantryGroupAt(pos);
    if (ig == 0) {
        return p::ccmap->isBuildableAt( PlayerNr, pos, type->isWaterBound());
    } else {
	    if (ig->IsAvailable()) {
    	    *subpos = ig->GetFreePos();
        	return true;
    	}
	}
    return false;
}

Uint32 Structure::getImageNum(Uint8 layer) const
{
	return type->getSHPNums()[layer] + imagenumbers[layer];
}

Uint32 Structure::getRealImageNum(Uint8 layer)
{
	return (imagenumbers[layer]&0x7FF /*0x1f*/);
}

void Structure::changeImage(Uint8 layer, Sint16 imagechange) {
        imagenumbers[layer]+=imagechange;
}

void Structure::setStructnum(Uint32 stn) 
{
    structnum = stn;
}

Uint32 Structure::getNum() const 
{
        return structnum;
}

Uint16 Structure::getPos() const
{
	return cellpos;
}

Uint16 Structure::getSubpos() const 
{
	return 0;
}

Uint8 Structure::getOwner() const 
{
	return owner;
}

void Structure::setOwner(Uint8 newowner) 
{
	owner = newowner;
}

bool Structure::canAttack() const 
{
	return (type->getWeapon() != 0);
}

bool Structure::IsBuilding()
{
	if (usemakeimgs){
		return true;
	}
	return false;  
}

Uint16 Structure::getHealth() const 
{
	return health;
}

Sint8 Structure::getXoffset() const 
{
	return type->getXoffset();
}

Sint8 Structure::getYoffset() const 
{
	return type->getYoffset();
}

bool Structure::isPowered()
{
	if (type->isPowered()){
		return true;
	}
	return false;
}

bool Structure::isRefinery()
{
	// @todo Hack !!!
	if (strcmp ((char*)type->getTName(), "PROC") == 0){
		return true;
	}
	return false;
}

double Structure::getRatio() const 
{
	return ratio;
}

string Structure::getTriggerName() 
{
	return TriggerName;
}

void Structure::setPrimary(bool pri) 
{
	primary = pri;
}

bool Structure::isPrimary() const
{
	return primary;
}

StructureType* Structure::getType()
{
	return type;
}

Uint16* Structure::getImageNums() const
{
	return imagenumbers;
}
