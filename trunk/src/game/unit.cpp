#include "Unit.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <math.h>

#include "SDL/SDL_timer.h"

#include "Ai.h"
#include "ActionEventQueue.h"
#include "GameMode.h"
#include "unittypes.h"
#include "include/ccmap.h"
#include "InfantryGroup.h"
#include "L2Overlay.h"
#include "MoveAnimEvent.h"
#include "Player.h"
#include "Projectile.h"
#include "TurnAnimEvent.h"
#include "UAttackAnimEvent.h"
#include "UHarvestEvent.h"
#include "URepairEvent.h"
#include "UnitAndStructurePool.h"
#include "audio/SoundEngine.h"
#include "include/PlayerPool.h"
#include "include/Logger.h"
#include "include/config.h"
#include "TalkbackType.h"

namespace p {
	extern ActionEventQueue * aequeue;
	extern CnCMap* ccmap;
	extern PlayerPool* ppool;
}
namespace pc {
    extern ConfigType Config;
    extern Ai * ai;
    extern SoundEngine* sfxeng;
}
extern Logger * logger;

using std::string;
using std::vector;


/** 
 * note to self, pass owner, cellpos, 
 * facing and health to this (maybe subcellpos)
 */
Unit::Unit(UnitType *type, Uint16 cellpos, Uint8 subpos, InfantryGroup *group,
        Uint8 owner, Uint16 rhealth, Uint8 facing, Uint8 action, string trigger_name) : UnitOrStructure()
{
    EmptyResources ();

    BaseRefinery = NULL;
    targetcell = cellpos;
    Uint32 i;
    this->type = type;
    imagenumbers = new Uint16[type->getNumLayers()];
    for( i = 0; i < type->getNumLayers(); i++ ) {
        imagenumbers[i] = facing;
        if( owner != 0xff ) {
            if (type->getDeployTarget() != NULL) {
                palettenum = (p::ppool->getStructpalNum(owner)<<11);
            } else {
                palettenum = (p::ppool->getUnitpalNum(owner)<<11);
            }
            imagenumbers[i] |= palettenum;
        }
    }
    this->owner = owner;
    this->cellpos = cellpos;
    this->subpos = subpos;
    l2o = NULL;
    xoffset = 0;
    yoffset = 0;
    ratio = (double)rhealth/256.0f;
    health = (Uint16)(ratio * type->getMaxHealth());
    infgrp = group;

    if (infgrp) {
        if (infgrp->IsClear(subpos)) { /* else select another subpos */
            infgrp->AddInfantry(this, subpos);
        }
    }
    moveanim = NULL;
    attackanim = NULL;
    walkanim = NULL;
    harvestanim = NULL;
	repairanim = NULL;
    turnanim1 = turnanim2 = NULL;
    deployed = false;
    p::ppool->getPlayer(owner)->builtUnit(this);

    if (strcmp ((char*)type->getTName(), "HARV") == 0){
	   this->Harvest (0, NULL);
    }
    
    AI_Mission = 1;

	TriggerName = trigger_name;
	Command     = action;

	mapwidth = p::ccmap->getWidth();
	mapheight = p::ccmap->getHeight();

	fix_str_num = 0;

	// Init last damage tick var
	LastDamageTick = SDL_GetTicks();

	// testing code for medic healing
	//health -= 20;
	//updateDamaged();
}

Unit::~Unit()
{
	tm		*Tm;
	time_t  Now_epoch;

	Now_epoch = time(0);
	Tm = localtime (&Now_epoch);
//	logger->warning ("%s line %i: %02i:%02i:%02i Unit destructor, unitnr = %i\n", __FILE__, __LINE__, Tm->tm_hour, Tm->tm_min, Tm->tm_sec, unitnum);

//	if (p::uspool->getUnit(unitnum) != NULL && !pc::quit)
//		logger->warning ("%s line %i: Unit destructor for unit = %i, unitandstructerpool not derefd\n", __FILE__, __LINE__, unitnum);

//	if (p::uspool->getNumbUnits() != p::ppool->getPlayer(getOwner())->getNumUnits() && !pc::quit )
//		logger->warning ("%s line %i: Player unit pool not the same size as unitandstructurepool unitoopl\n", __FILE__, __LINE__, unitnum);

    delete[] imagenumbers;
    if (attackanim != NULL && target != NULL) {
        target->unrefer();
    }
    if( l2o != NULL ) {
        p::uspool->removeL2overlay(l2entry);
        delete l2o;
    }
    if (type->isInfantry() && infgrp) {
        infgrp->RemoveInfantry(subpos);
//        if (infgrp->GetNumInfantry() == 0) {
            //delete infgrp;
			//printf ("%s line %i: Numb infantry not 0\n", __FILE__, __LINE__);
//        }
    }
    
    if (deployed) {
        // TODO This is a client thing. Will dispatch a 
    	// "play these sounds" event when the time comes.
		if (this->getOwner() == p::ppool->getLPlayerNum()){
			pc::sfxeng->PlaySound(pc::Config.UnitDeployed);
			if (pc::Config.gamenum == GAME_TD) {
				pc::sfxeng->PlaySound("hvydoor1.aud");
			}
		}
        p::uspool->createStructure(type->getDeployTarget(),calcDeployPos(),owner,(Uint16)(ratio*256.0f),0, true, "None");
/*
	//printf ("Deploy\n");
	if (!pc::sidebar->getVisible()){
		printf ("Toggle visibility\n");
		pc::sidebar->ToggleVisible();
	}else
		printf ("NOTNOTNOT\n");
*/
    }
}

Uint8 Unit::getImageNums(Uint32 **inums, Sint8 **xoffsets, Sint8 **yoffsets)
{
    int i;
    Uint32 *shpnums;

    shpnums = type->getSHPNums();

    *inums = new Uint32[type->getNumLayers()];
    *xoffsets = new Sint8[type->getNumLayers()];
    *yoffsets = new Sint8[type->getNumLayers()];
    for(i = 0; i < type->getNumLayers(); i++ ) {
        (*inums)[i] = shpnums[i]+imagenumbers[i];
        (*xoffsets)[i] = xoffset-type->getOffset();
        (*yoffsets)[i] = yoffset-type->getOffset();
    }
    return type->getNumLayers();
}

InfantryGroup* Unit::getInfantryGroup() {
        return infgrp;
    }

void Unit::setInfantryGroup(InfantryGroup *ig) {
        infgrp = ig;
    }

Uint32 Unit::getImageNum(Uint8 layer) const {
        return type->getSHPNums()[layer]+imagenumbers[layer];
    }

Uint16 Unit::getNumbImages( Uint8 layer )
{
	if (type->getSHPTNum() != NULL)
		return type->getSHPTNum()[layer];
	return 0;
}

void Unit::setImageNum(Uint32 num, Uint8 layer)
{
	if (getNumbImages( layer ) > num)
    	imagenumbers[layer] = num | palettenum;
	else
		logger->error("%s line %i: FAILED to set imagenumb %i, numb images = %i\n", __FILE__, __LINE__, num, getNumbImages( layer ));
}

Sint8 Unit::getXoffset() const
{
    if (l2o != NULL) {
        return l2o->xoffsets[0];
    } else {
        return xoffset-type->getOffset();
    }
}

Sint8 Unit::getYoffset() const
{ 
	return yoffset; 
}

void Unit::setXoffset(Sint8 xo)
{
    if (l2o != NULL) {
        l2o->xoffsets[0] = xo;
    } else {
        xoffset = xo;
    }
}

void Unit::setYoffset(Sint8 yo)
{ 
	this->yoffset = yo; 
}

UnitType* Unit::getType() {
        return type;
    }

Uint16 Unit::getPos() const {
        return cellpos;
    }

Uint16 Unit::getBPos(Uint16 pos) const {
        return cellpos;
    }

Uint16 Unit::getSubpos() const {
        return subpos;
    }

Uint32 Unit::getNum() const {
        return unitnum;
    }

void Unit::setUnitnum(Uint32 unum) {
        unitnum = unum;
    }

Uint16 Unit::getHealth() const {
        return health;
    }

void Unit::ChangeHealth (Sint16 amount)
{
//	printf ("%s line %i: original health = %i\n", __FILE__, __LINE__, health);
	if (health + amount > type->getMaxHealth()){
		health = type->getMaxHealth();
	} else if (health + amount < 0){
		health = 0;
	} else
		health += amount;

//	printf ("%s line %i: health = %i\n", __FILE__, __LINE__, health);
}

void Unit::move(Uint16 dest)
{
    move(dest,true);
}

void Unit::move(Uint16 dest, bool stop)
{
    targetcell = dest;
    if (stop && (attackanim != NULL)) {
        attackanim->stop();
        if (target != NULL) {
            target->unrefer();
            target = NULL;
        }
    }

	if (stop && harvestanim != NULL){
		harvestanim->stop();
	}

    if (moveanim == NULL) {
        moveanim = new MoveAnimEvent(type->getSpeed(), this);
        p::aequeue->scheduleEvent(moveanim);
    } else {
        moveanim->update();
    }
}

bool Unit::IsMoving (void)
{
    if (moveanim != NULL)
        return true;
    return false;
}

bool Unit::IsAttacking (void)
{
    if (attackanim == NULL)
        return false;
    return true;
}

bool Unit::canAttack(bool primary) {
	return type->getWeapon(primary)!=NULL;
}

bool Unit::UnderAttack (void){
	// We have never been attacked if we still have all health
	if (health == getType()->getMaxHealth())
		return false;

	// If the last damage is longer than 10 sec ago we are no longer under attack
	if ((SDL_GetTicks() - LastDamageTick) > 1000*10)
		return false;

	return true;
}

void Unit::attack(UnitOrStructure* target)
{
	if (!this->canAttack())
		return;

    attack(target, true);
}

void Unit::attack(UnitOrStructure* target, bool stop)
{
	Weapon *Weap;

	if (!this->canAttack())
		return;

	if (!target->getType()->isStructure()){
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
	} else {
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

    if (stop && (moveanim != NULL)) {
        moveanim->stop();
    }
    if (this->target != NULL) {
        this->target->unrefer();
    }
    this->target = target;
    target->referTo();
    targetcell = target->getBPos(cellpos);
    if (attackanim == NULL) {
        attackanim = new UAttackAnimEvent(0, this);
        p::aequeue->scheduleEvent(attackanim);
    } else {
        attackanim->update();
    }
}

void Unit::turn(Uint8 facing, Uint8 layer)
{
    TurnAnimEvent** t;
    switch (layer) {
    case 0:
        t = &turnanim1;
        break;
    case 1:
        t = &turnanim2;
        break;
    default:
        logger->error("invalid arg of %i to Unit::turn\n",layer);
        return;
        break;
    }
    if (*t == NULL) {
        *t = new TurnAnimEvent(type->getROT(), this, facing, layer);
        p::aequeue->scheduleEvent(*t);
    } else {
        (*t)->changedir(facing);
    }

}

void Unit::stop()
{
    if (moveanim != NULL) {
        moveanim->stop();
    }
    if (attackanim != NULL) {
        attackanim->stop();
    }
}

Uint8 Unit::getOwner() const {
        return owner;
    }

void Unit::setOwner(Uint8 newowner) {
        owner = newowner;
    }

void Unit::remove() {
    p::ppool->getPlayer(owner)->lostUnit(this,deployed);
    UnitOrStructure::remove();
}

void Unit::applyDamage(Sint16 amount, Weapon* weap, UnitOrStructure* attacker)
{

//	printf ("%s line %i: Apply damage, damage = %i\n", __FILE__, __LINE__, amount);

    HandleTriggers((UnitOrStructure*)this, 6);

	LastDamageTick = SDL_GetTicks();

	if (this->getOwner() != p::ppool->getLPlayerNum()){
		// This unit is from a computer player
		if (p::ccmap->getGameMode() != GAME_MODE_SINGLE_PLAYER){
			pc::ai->DefendComputerPlayerUnitUnderAttack ( p::ppool->getPlayer(this->getOwner()), this->getOwner(), attacker, this );
		}
	}
#if 0
	else {
		// The structure is from the local player
		if ((SDL_GetTicks() - LastAttackTick) > (60 * 1000)){
			pc::sfxeng->PlaySound(pc::Config.BaseUnderAttack);
			LastAttackTick = SDL_GetTicks();
		}
	}
#endif

	if (weap != NULL){
		amount = (Sint16)((double)amount * weap->getVersus(type->getArmour()));
	}
	
    if ((health-amount) <= 0) {
		HandleTriggers((UnitOrStructure*)this, 7 );
        doRandTalk(TB_die);

        // Add a death for stats
        if (attacker != NULL){
        	p::ppool->getPlayer(attacker->getOwner())->addUnitKill();
        }
        
        // todo: add infantry death animation
                
        // remove the unit
        p::uspool->removeUnit(this);
        return;
    } else if ((health-amount) > type->getMaxHealth()) {
        health = type->getMaxHealth();
    } else {
        health -= amount;
    }
    ratio = (double)health / (double)type->getMaxHealth();
}

void Unit::updateDamaged()
{
	ratio = (double)health / (double)type->getMaxHealth();
}

char* Unit::getTName() const 
{
	return (char*)type->getTName();
}

bool Unit::IsHarvester() 
{
	if (strcmp ((char*)type->getTName(), "HARV") == 0)
		return true;
	return false;
}

bool Unit::IsHarvesting() 
{
	if (harvestanim != NULL){
		return true;
	}
	return false;
}

/**
 *	This functions returns the position of the tiberium that is closesed to this unit....
 */
Uint32 Unit::FindTiberium (void)
{
	Uint32 tiberium;
	Uint32 ClosesedPos = 0;
	Uint32 ClosesedDistance = 0;
	Uint32 Distance = 0, ClosesedExpensivePos = 0;
	Uint32 ClosesedExpensiveDistance = 0;
	bool FirstFound = false;
	bool FirstExpensiveFound = false;
	
	Uint8 type, amount;
	
	//getUnitAt(Uint32 cell, Uint8 subcell);

	for (unsigned int pos =0; pos < p::ccmap->getSize(); pos++){
		tiberium = p::ccmap->getResource(pos, &type, &amount);
		if (tiberium != 0) {
			// Found tiberium
			//printf ("Found tiberium\n");
			Distance = this->getDist(pos);
			if (Distance < ClosesedDistance || !FirstFound){
				if (p::uspool->getUnitAt(pos) == NULL){
					ClosesedPos		= pos;
					ClosesedDistance	= Distance;
					FirstFound = true;
				}

				// Initialize the expensive values
				if (type < 4 && ClosesedExpensivePos == 0){
					ClosesedExpensivePos		= pos;
					ClosesedExpensiveDistance	= Distance;
				}
			}
			if (type < 4){
				//Christal
				Distance = this->getDist(pos);
				if (Distance < ClosesedExpensiveDistance || !FirstExpensiveFound){
					if (p::uspool->getUnitAt(pos) == NULL){
						ClosesedExpensivePos		= pos;
						ClosesedExpensiveDistance	= Distance;
						FirstExpensiveFound = true;
					}
				}
			}
		}
	}
//	if (owner != p::ppool->getLPlayerNum()){
//		printf ("%s line %i: Exdist = %i, Dist = %i, ExFound = %i\n", __FILE__, __LINE__, ClosesedExpensiveDistance, ClosesedDistance, FirstExpensiveFound);
//	}
	if (owner != p::ppool->getLPlayerNum() && ((ClosesedExpensiveDistance < (ClosesedDistance*3)) || ClosesedExpensiveDistance < 10) && FirstExpensiveFound){
		//printf ("Return closesed expecive pos");
		return ClosesedExpensivePos;
	}

	return ClosesedPos;
}

void Unit::Harvest (Uint32 pos, Structure *Struct)
{
     if (harvestanim == NULL) {
        harvestanim = new UHarvestEvent(0, this);
        p::aequeue->scheduleEvent(harvestanim);
    } else {
        harvestanim->update();
    }

	if (pos != 0){
		if (moveanim != NULL)
			this->move(pos, false);
//		moveanim->stop();
		harvestanim->setHarvestingPos(pos);
	}

	if (Struct != NULL)
		SetBaseRefinery (Struct);
}

bool Unit::Repair (Structure *str)
{
	Uint16 xpos, ypos;

	if (strcmp ((char*)str->getType()->getTName(), "FIX") != 0 )
		return false;

	p::ccmap->translateFromPos(str->getPos(), &xpos, &ypos);

	xpos += str->getType()->getXsize()/2 ;
	ypos += str->getType()->getYsize()/2;

	fix_str_num = str->getNum();
	fix_str_pos = (Uint16)p::ccmap->translateToPos(xpos, ypos);

	if (repairanim == NULL) {
		repairanim = new URepairEvent(0, this);
		p::aequeue->scheduleEvent(repairanim);
	} else {
		repairanim->update();
	}

	move(fix_str_pos);

	return true;
}

void Unit::doRandTalk(TalkbackType ttype)
{
    const char* sname;
    sname = type->getRandTalk(ttype);
    if (sname != NULL) {
        pc::sfxeng->PlaySound(sname);
    }
}

bool Unit::deploy()
{
    if (canDeploy()) { // error catching
        if (type->getDeployTarget() != NULL) {
            deployed = true;
            p::uspool->removeUnit(this);
	    	return true;
        }
    }
    return false;
}

bool Unit::canDeploy()
{
    if (type->canDeploy()) {
        if (type->getDeployTarget() != NULL) {
            if (!deployed)
                return checkDeployTarget(calcDeployPos());
            return false;
        }
   }
    return false;
}

bool Unit::checkDeployTarget(Uint32 pos)
{
    Uint8 placexpos, placeypos;
    Uint32 curpos;
    Uint8 typewidth, typeheight;
    if (pos == (Uint32)(-1)) {
        return false;
    }
    if (type->getDeployType() == NULL) {
        //return (p::ccmap->getCost(pos,this)<=1);
        return false;
    }
    typewidth = type->getDeployType()->getXsize();
    typeheight = type->getDeployType()->getYsize();
    if ((pos%mapwidth)+typewidth > mapwidth) {
        return false;
    }
    if ((pos/mapwidth)+typeheight > mapheight) {
        return false;
    }
    for( placeypos = 0; placeypos < typeheight; ++placeypos) {
        for( placexpos = 0; placexpos < typewidth; ++placexpos) {
            curpos = pos+placeypos*mapwidth+placexpos;
            if( type->getDeployType()->isBlocked(placeypos*typewidth+placexpos) ) {
                if (!p::ccmap->isBuildableAt(this->getOwner(), curpos,this)) {
                    return false;
                }
            }
        }
    }
    return true;
}
Uint32 Unit::calcDeployPos() const
{
    Uint32 deploypos;
    Uint32 mapwidth = p::ccmap->getWidth();
    Uint8 w,h;

    if (type->getDeployType() == NULL) {
        if (cellpos%mapwidth == mapwidth) {
            return (Uint32)-1;
        }
        deploypos = cellpos+1;
    } else {
        w = type->getDeployType()->getXsize();
        h = type->getDeployType()->getYsize();

        deploypos = cellpos;
        if ((Uint32)(w >> 1) > deploypos)
            return (Uint32)-1; // large number
        else
            deploypos -= w >> 1;
        if ((mapwidth*(h >> 1)) > deploypos)
            return (Uint32)-1;
        else
            deploypos -= mapwidth*(h >> 1);
    }
    return deploypos;
}
Uint32 Unit::calcDeployPos(Uint32 pos) const
{
    Uint32 deploypos;
    Uint32 mapwidth = p::ccmap->getWidth();
    Uint8 w,h;

    if (type->getDeployType() == NULL) {
        if (pos%mapwidth == mapwidth) {
            return (Uint32)-1;
        }
        deploypos = pos+1;
    } else {
        w = type->getDeployType()->getXsize();
        h = type->getDeployType()->getYsize();

        deploypos = pos;
        if ((Uint32)(w >> 1) > deploypos)
            return (Uint32)-1; // large number
        else
            deploypos -= w >> 1;
        if ((mapwidth*(h >> 1)) > deploypos)
            return (Uint32)-1;
        else
            deploypos -= mapwidth*(h >> 1);
    }
    return deploypos;
}
Uint32 Unit::getExitCell() const {
        return calcDeployPos();
    }
double Unit::getRatio() const {
        return ratio;
    }
Uint16 Unit::getDist(Uint16 pos)
{
    Uint16 x, y, nx, ny, xdiff, ydiff;
    x = cellpos%p::ccmap->getWidth();
    y = cellpos/p::ccmap->getWidth();
    nx = pos%p::ccmap->getWidth();
    ny = pos/p::ccmap->getWidth();

    xdiff = abs(x-nx);
    ydiff = abs(y-ny);
//    return min(xdiff,ydiff)+abs(xdiff-ydiff);
    return (Uint32) sqrt (xdiff*xdiff+ydiff*ydiff);
}
Uint16 Unit::getTargetCell()
{
    if (attackanim != NULL && target != NULL) {
        return target->getBPos(cellpos);
    }
    return targetcell;
}
Structure *Unit::GetBaseRefinery (){
		return BaseRefinery;
    }
void Unit::SetBaseRefinery (Structure *Bref){
		printf ("Set base refinery\n");
		BaseRefinery = Bref;
		if (harvestanim == NULL) {
			harvestanim = new UHarvestEvent(0, this);
			p::aequeue->scheduleEvent(harvestanim);
		} else {
			harvestanim->update();
		}
    }
void Unit::AddResource (Uint8 ResourceType)
    {
        ResourceTypes[NumbResources] = ResourceType;  // Resource type in harvester
        NumbResources++;
    }
void Unit::EmptyResources (void)
    {
        NumbResources = 0;  // Resource type in harvester
    }
Uint8 Unit::GetNumResources (void)
    {
        return NumbResources;  // Resource type in harvester
    }
bool Unit::GetResourceType (Uint8 Numb, Uint8 *Type)
    {
        if (Numb < NumbResources){
            *Type = ResourceTypes[Numb];
            return true;
        }
        return false;
    }
bool Unit::is(const char *Name) {
		if (strcmp (getType()->getTName(), Name) == 0)
			return true;
		return false;
	}
Uint32 Unit::GetFixStr (void)
	{
		return fix_str_num;
	}
Uint16 Unit::GetFixPos (void)
	{
		return fix_str_pos;
	}
bool Unit::IsAirBound (void)
	{
		if (type->getType() == UN_PLANE || type->getType() == UN_HELICOPTER)
			return true;
		return false;
	}
bool Unit::IsWaterBound (void)
	{
		if (type->getType() == UN_BOAT)
			return true;
		return false;
	}
string Unit::getTriggerName (void) {return TriggerName;}
void Unit::operation1(){}


