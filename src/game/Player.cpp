// Player.cpp
// 1.8

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

#include "Player.h"

#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>

#include "CnCMap.h"
#include "include/config.h"
#include "Dispatcher.h"
#include "misc/INIFile.h"
#include "PlayerPool.h"
#include "include/Logger.h"
#include "MoneyCounter.h"
#include "UnitAndStructurePool.h"
#include "StructureType.h"
#include "UnitOrStructureType.h"
#include "Structure.h"
#include "ConStatus.h"
#include "pside.h"
#include "Unit.h"
#include "PowerInfo.h"

#include "misc/INIFile.h"
#include "BQueue.h"

using std::map;

namespace pc {
    extern ConfigType Config;
}
namespace p {
	extern UnitAndStructurePool* uspool;
	extern CnCMap* ccmap;
}
extern Logger * logger;

/**
 * @todo Make hardcoded side names customisable (Needed to make RA support
 * cleaner)
 *
 * @todo The colour lookup code needs to ensure duplicates don't happen.
 *
 * @todo Do something about the player start points in multiplayer: server
 * should take points from map, shuffle them, and then hand out only the start
 * point for each player (possibly include start points for allies if we have
 * map sharing between allies).  This requires some work on the USPool too (see
 * that file for details).
 */
Player::Player(const char *pname, INIFile *mapini)
{
    Uint32 mapsize;
    playername = cppstrdup(pname);
    multiside = 0;
    unallycalls = 0;
    playerstart = 0;
    defeated = false;
    
    if( !strcmp((playername), ("Spain"))) 
    {
        playerside = PS_GOOD;
        unitpalnum = 0;
        structpalnum = 0;
    } else if( !strcmp((playername), ("Greece"))) 
    {
    	playerside = PS_GOOD;
    	unitpalnum = 1;
    	structpalnum = 1;
    } 
    else if( !strcmp((playername), ("USSR"))) 
    {
    	playerside = PS_BAD;
    	unitpalnum = 2;
    	structpalnum = 2;
    } 
    else if( !strcmp((playername), ("England"))) 
    {
    	playerside = PS_GOOD;
    	unitpalnum = 3;
    	structpalnum = 3;
    } 
    else if( !strcmp((playername), ("Germany"))) 
    {
    	playerside = PS_GOOD;
    	unitpalnum = 6;
    	structpalnum = 6;
    } 
    else if( !strcmp((playername), ("France"))) 
    {
    	playerside = PS_GOOD;
    	unitpalnum = 7;
    	structpalnum = 7;
    } 
    else if( !strcmp((playername), ("Turkey"))) 
    {
    	playerside = PS_BAD;
    	unitpalnum = 8;
    	structpalnum = 8;
    }
    else if( !strcmp((playername), ("GoodGuy")) ) 
    {
    	playerside = PS_GOOD;
        unitpalnum = 0;
        structpalnum = 0;
    } 
    else if( !strcmp((playername), ("BadGuy")) ) 
    {
        playerside = PS_BAD;
        unitpalnum = 2;
        structpalnum = 1;
    } 
    else if( !strcmp((playername), ("Neutral")) ) 
    {
        playerside = PS_NEUTRAL;
        unitpalnum = 0;
        structpalnum = 0;
    } 
    else if( !strcmp((playername), ("Special")) ) 
    {
        playerside = PS_SPECIAL;
        unitpalnum = 0;
        structpalnum = 0;
    } 
    else if( !strncmp((playername), ("multi"), (5)) ) 
    {
        playerside = PS_MULTI;

        if (playername[5] < 49 || playername[5] > 57) {
            logger->error("Invalid builtin multi name: %s\n",playername);
            /// @todo Nicer error handling here
            multiside = 9;
        } else {
            multiside = playername[5] - 48;
        }
        if (multiside > 0) {
            unitpalnum = multiside - 1;
            structpalnum = multiside - 1;
        } else {
            unitpalnum = structpalnum = 0;
        }
        playerstart = p::ppool->getAStart();
#if 0
        if (playerstart != 0) {
            // conversion algorithm from loadmap.cpp
            playerstart = p::ccmap->normaliseCoord(playerstart);
        }
#endif
	} 
    else 
	{
        logger->warning("Player Side \"%s\" not recognised, using gdi instead\n",pname);
        playerside = PS_GOOD;
        unitpalnum = 0;
        structpalnum = 0;
    }
    money = mapini->readInt(playername, "Credits", 0) * 100;
    powerGenerated = 0;
    powerUsed = 0;
    unitkills = unitlosses = structurekills = structurelosses = 0;

    // this size is just to size matrixs
    //mapsize = p::ccmap->getWidth()*p::ccmap->getHeight();
    mapsize = mapini->readInt("Map", "Width", 255)*mapini->readInt("Map", "Height", 255);
    sightMatrix.resize(mapsize);
    buildMatrix.resize(mapsize);
    mapVisible.resize(mapsize);
    mapBuildable.resize(mapsize);

    // cheats ...
    allmap = buildall = buildany = infmoney = false;

    queues[0] = new BQueue(this);

    counter = new MoneyCounter(&money, this, &counter);

    // Set number of radars to zero
    numberRadars = 0;

    brad = getConfig().buildable_radius;
    mwid = mapini->readInt("Map", "Width", 255); // 255 -> max ???
    
    // Read the TechLevel
    this->techLevel = mapini->readInt(pname, "TechLevel", 30);
}

Player::~Player()
{
	map<Uint8, BQueue*>::iterator i, end;
	
	// Free name of the player
	if (playername != 0){
		delete[] playername;
	}
	playername = 0;
   
    end = queues.end();
    for (i = queues.begin(); i != end; ++i) {
		if (i->second != NULL)
			delete i->second;
		i->second = NULL;
    }
	if (counter != NULL)
		delete counter;
	counter = NULL;
}

/**
 * Return true if the player is the local player
 */
bool Player::isLPlayer()
{
	// If the number of this player is == to local player number of the pool
	if (playernum == p::ppool->getLPlayerNum()){
		// it's true
		return true;
	}
	// else it's false
	return false;
}


void Player::setPlayerNum(Uint8 num)
{
	playernum = num;

	/*
	if (p::ccmap->getGameMode() == 0){
		if (playernum !=  p::ppool->getLPlayerNum() && playerside != PS_NEUTRAL && playerside != PS_SPECIAL){
			if (pc::Config.mside == "gdi")
				setMultiColour("red");
		}
	}
	*/
}

/**
 * Set the palette of the player
 * 
 * @param colour name of the color
 */
void Player::setMultiColour(const char* colour)
{
	if (strcmp((colour), ("yellow")) == 0) {
		unitpalnum = structpalnum = 0;
		return;
	} else if (strcmp((colour), ("blue")) == 0) {
		unitpalnum = structpalnum = 1;
		return;
	} else if (strcmp((colour), ("red")) == 0) {
		unitpalnum = structpalnum = 2;
		return;
	} else if (strcmp((colour), ("green")) == 0) {
		unitpalnum = structpalnum = 3;
		return;
	}else if (strcmp((colour), ("yellow")) == 0) {
		unitpalnum = structpalnum = 4;
		return;
	} else if (strcmp((colour), ("orange")) == 0) {
		unitpalnum = structpalnum = 5;
		return;
	} else if (strcmp((colour), ("gray")) == 0) {
		unitpalnum = structpalnum = 6;
		return;
	} else if (strcmp((colour), ("turquoise")) == 0) {
		unitpalnum = structpalnum = 7;
		return;
	}else if (strcmp((colour), ("darkred")) == 0) {
		unitpalnum = structpalnum = 8;
		return;
	}
}

/**
 * Set the palette of the player
 * 
 * @param colour number of the palette
 */
void Player::setMultiColour(const int colour)
{
	// set to default
	unitpalnum = structpalnum = 0;

	// Check that the number is beteween 0 and 8 
	if (colour >= 0 && colour < 9)
	{
		unitpalnum = structpalnum = colour;
	}
}


Uint8 Player::getMultiColour()
{
	return structpalnum; 
}

void Player::setSettings(const char* nick, const char* colour, const char* mside)
{
//	printf ("%s line %i: Player set settings, colour = %s\n", __FILE__, __LINE__, colour);
    if (mside == NULL || strlen(mside) == 0) {
        //playerside |= PS_GOOD;
		playerside = PS_GOOD;
    } else {
        if (strcmp((mside), ("gdi")) == 0 || strcmp((mside), ("goodguy")) == 0) {
            //playerside |= PS_GOOD;
	   playerside = PS_GOOD;
        } else if (strcmp((mside), ("nod")) == 0 || strcmp((mside), ("badguy")) == 0) {
            //playerside |= PS_BAD;
	      playerside = PS_BAD;
        }
    }
    if (colour != NULL && strlen(colour) != 0) {
        setMultiColour(colour);
    }
    if (nick != NULL && strlen(nick) != 0) {
        if (strlen(nick) < strlen(playername)) {
            strncpy(playername, nick, strlen(playername));
        } else {
			if (playername != NULL)
				delete[] playername;
			playername = NULL;

			playername = cppstrdup(nick);
        }
    }
}

void Player::setSettings(const char* nick, const int colour, const char* mside)
{
//	printf ("%s line %i: Player set settings, colour = %i\n", __FILE__, __LINE__, colour);

    if (mside == NULL || strlen(mside) == 0) {
        //playerside |= PS_GOOD;
	playerside = PS_GOOD;
    } else {
        if (strcmp((mside), ("gdi")) == 0 || strcmp((mside), ("goodguy")) == 0) {
            //playerside |= PS_GOOD;
	   playerside = PS_GOOD;
        } else if (strcmp((mside), ("nod")) == 0 || strcmp((mside), ("badguy")) == 0) {
            //playerside |= PS_BAD;
	      playerside = PS_BAD;
        }
    }

        setMultiColour(colour);

    if (nick != NULL && strlen(nick) != 0) {
        if (strlen(nick) < strlen(playername)) {
            strncpy(playername, nick, strlen(playername));
        } else {
			if (playername != NULL)
				delete[] playername;
			playername = NULL;
			playername = cppstrdup(nick);
        }
    }
}

Uint8 Player::getPlayerNum() const 
{
	return playernum;
}

const char* Player::getName() const 
{
	return playername;
}

Uint8 Player::getSide() const 
{
	return playerside;
}

bool Player::setSide(Uint8 Side)
{
	if (Side == PS_GOOD || Side == PS_BAD )
	{
		playerside = Side;
		return true;
	} else {
		return false;
	}
}

Uint8 Player::getMSide() const 
{
	return multiside;
}

bool Player::changeMoney(Sint32 change) 
{
    if (0 == change) {
        return true;
    } else if (change > 0) {
        counter->addCredit(change, playernum);
        return true;
    } else {
        Sint32 after = money + change - counter->getDebt();
        if (!infmoney && after < 0) {
            return false;
        }
        counter->addDebt(-change, playernum);
        return true;
    }
}

void Player::setMoney(Sint32 NewMoney)
{
	money = NewMoney;
}

Sint32 Player::getMoney() const 
{
	return money;
}

bool Player::startBuilding(UnitOrStructureType *type)
{
    BQueue* queue = getQueue(type->getPQueue());    
    if (0 == queue) {
        logger->error("Didn't find build queue for \"%s\" (pqueue: %i)\n",
                type->getTName(), type->getPQueue());
        return false;
    }
    // Add the type in the queue
    return queue->Add(type);
}

ConStatus Player::stopBuilding(UnitOrStructureType *type)
{
    BQueue* queue = getQueue(type->getPQueue());
    if (0 == queue) {
        logger->error("Didn't find build queue for \"%s\" (pqueue: %i)\n",
                type->getTName(), type->getPQueue());
        return BQ_EMPTY;
    }
    return queue->PauseCancel(type);
}

void Player::pauseBuilding()
{
    map<Uint8, BQueue*>::iterator i;
    for (i = queues.begin(); i != queues.end(); ++i) {
		if (i->second->getStatus() == BQ_RUNNING){
			//printf ("%s line %i: Buildque running\n", __FILE__, __LINE__);
			i->second->Pause ();
		}
    }
}

void Player::resumeBuilding(void)
{
    map<Uint8, BQueue*>::iterator i;
    for (i = queues.begin(); i != queues.end(); ++i) {
		if (i->second->getStatus() == BQ_ALL_PAUSED){
			//printf ("%s line %i: Buildque paused\n", __FILE__, __LINE__);
			i->second->Resume ();
		}
    }
}

void Player::placed(UnitOrStructureType *type)
{
    BQueue* queue = getQueue(type->getPQueue());
    if (0 == queue) {
        logger->error("Didn't find build queue for \"%s\" (pqueue: %i) - especially weird as we've just built it\n",
                type->getTName(), type->getPQueue());
        return;
    }
    queue->Placed();
}

ConStatus Player::getStatus(UnitOrStructureType* type, Uint8* quantity, Uint8* progress)
{
    BQueue* queue = getQueue(type->getPQueue());
    if (0 == queue) {
        logger->error("No possible status for type \"%s\" as we have no primary building\n",
                type->getTName());
        return BQ_INVALID;
    }
    return queue->getStatus(type, quantity, progress);
}

BQueue* Player::getQueue(Uint8 queuenum)
{
    map<Uint8, BQueue*>::iterator it = queues.find(queuenum);
    if (queues.end() == it) {
        return 0;
    }
    return it->second;
}

void Player::builtUnit(Unit* un)
{
	// add in the unit pool
    unitpool.push_back(un);

	// add visible
    addSoB(un->getPos(), 1, 1, un->getType()->getSight(), SOB_SIGHT);

    if (defeated) {
        defeated = false;
        p::ppool->playerUndefeated(this);
    }
}

/**
 * @param un the Unit lost
 * @param wasDeployed true if the Unit was lost after deploy
 */
void Player::lostUnit(Unit* un, bool wasDeployed)
{
    Uint32 i;

//	logger->error ("%s line %i: *********************************** ERROR Player->lostUnit\n", __FILE__, __LINE__);

    removeSoB(un->getPos(), 1, 1, un->getType()->getSight(), SOB_SIGHT);

    if (!wasDeployed) {
//        logger->gameMsg("%s has %d structs and %d units", playername, (Uint32)structurepool.size(), (Uint32)unitpool.size()-1);
        ++unitlosses;
    }
    if( unitpool.size() <= 1 && structurepool.empty() && !wasDeployed) 
    {
    	// Defeat this player
        defeated = true;
        p::ppool->playerDefeated(this);
    } else {
        for (i=0;i<unitpool.size();++i) {
            if (unitpool[i] == un) {
                break;
            }
        }
        for (i=i+1;i<unitpool.size();++i) {
            unitpool[i-1] = unitpool[i];
        }
        unitpool.resize(unitpool.size()-1);
    }
}

void Player::movedUnit(Uint32 oldpos, Uint32 newpos, Uint8 sight)
{
    removeSoB(oldpos, 1, 1, sight, SOB_SIGHT);
    addSoB(newpos, 1, 1, sight, SOB_SIGHT);
}

/**
 * Called after a Player build a structure
 */
void Player::builtStruct(Structure* str)
{
    StructureType* st = 0; // Ref to the new Structure
    
    // Get the type of the structure
    st = dynamic_cast<StructureType*> (str->getType());
    
    // Add this structure to the pool
    structurepool.push_back(str);
    
	// Add some sight (sight of the building)
    // @todo change this feature to test during the placement
    addSoB(str->getPos(), st->getXsize(), st->getYsize(), st->getSight(), SOB_SIGHT);
    // If building 
    // @todo change this feature to test during the placement
    addSoB(str->getPos(), st->getXsize(), st->getYsize(), 2, SOB_BUILD);
    
    // get the power info and update constants
    PowerInfo newpower = st->getPowerInfo();
    powerGenerated += newpower.power;
    powerUsed += newpower.drain;
    
    // Add this structure to Player owned structures
    structures_owned[st].push_back(str);
    
    // ?????
    if (st->primarySettable()) 
    {
		printf ("%s line %i: Set primary, ptype = %i\n", __FILE__, __LINE__, st->getPType() );
        production_groups[st->getPType()].push_back(str);
        if ((production_groups[st->getPType()].size() == 1) ||
                (0 == getPrimary(str->getType()))) {
            setPrimary(str);
            queues[st->getPType()] = new BQueue(this);
        }
    }//else if (st->primarySettable()){
	//	setPrimary(str);
	//}
    
    // REMOVE the defeat
    if (defeated) {
        defeated = false;
        p::ppool->playerUndefeated(this);
    }
        
    // Update the number of radar
    if (string(st->getTName()) == "DOME") 
    {
    	numberRadars++;
    };
    
    // if it's local player update the sidebar
    // @todo CHANGE THAT
    if (playernum == p::ppool->getLPlayerNum()) {
    	p::ppool->updateSidebar();
    }

    // If this player has no power
    if (this->getPowerUsed() > this->getPower())
    {
    	// Check TRIGGER "Low Power"
        HandleGlobalTrigger(TRIGGER_EVENT_LOW_POWER, p::ppool->getHouseNumByPlayerNum(this->getPlayerNum()));
    }
}

void Player::lostStruct(Structure* str)
{
    StructureType* st = (StructureType*)str->getType();
    std::list<Structure*>& sto = structures_owned[st];
    Uint32 i;
    removeSoB(str->getPos(), ((StructureType*)str->getType())->getXsize(), ((StructureType*)str->getType())->getYsize(), 1, SOB_BUILD);
    PowerInfo newpower = ((StructureType*)str->getType())->getPowerInfo();

//	printf ("%s line %i: Lost structure, power before = %i", __FILE__, __LINE__, powerGenerated);
    powerGenerated -= newpower.power;
    powerUsed -= newpower.drain;
//	printf ("power after = %i, playernum = %i\n", powerGenerated, playernum);
//    printf ("%s has %d structs and %d units\n", playername, (Uint32)structurepool.size()-1, (Uint32)unitpool.size());
//    printf ("Structure = %c%c%c%c, pointer = %i\n", str->getType()->getTName()[0], str->getType()->getTName()[1], str->getType()->getTName()[2], str->getType()->getTName()[3],(int)str);
//    logger->gameMsg("%s has %d structs and %d units", playername, (Uint32)structurepool.size()-1, (Uint32)unitpool.size());
    std::list<Structure*>::iterator it = std::find(sto.begin(), sto.end(), str);
    assert(it != sto.end());
    sto.erase(it);
    if (st->primarySettable() /*&& (st->getPType() != 0)*/) {
        std::list<Structure*>& prg = production_groups[st->getPType()];
        it = std::find(prg.begin(), prg.end(), str);
        assert(it != prg.end());
        prg.erase(it);
        if (str == getPrimary(str->getType())) {
            if (prg.empty()) {
                getPrimary(str->getType()) = 0;
                map<Uint8, BQueue*>::iterator it = queues.find(st->getPType());
                assert(it != queues.end());
				if ((*it).second != NULL)
					delete (*it).second;
				(*it).second = NULL;
                queues.erase(it);
            } else {
                setPrimary(prg.front());
            }
        }
    }//else if (st->primarySettable()){

	//}
    
    // Update number of radars
    if (string(st->getTName()) == "DOME") {
    	numberRadars--;
    }
    
    // Add it for stats
    ++structurelosses;

    if( unitpool.empty() && structurepool.size() <= 1 ) {
        logger->gameMsg("Player \"%s\" defeated", playername);
        defeated = true;
        p::ppool->playerDefeated(this);
    } else {
        for (i=0;i<structurepool.size();++i) {
            if (structurepool[i] == str) {
                break;
            }
        }
        for (i=i+1;i<structurepool.size();++i) {
            structurepool[i-1] = structurepool[i];
        }
        structurepool.resize(structurepool.size()-1);
    }
    
    // If it's the local player
    // @todo MOVE THAT
    if (playernum == p::ppool->getLPlayerNum()) {
            p::ppool->updateSidebar();
    }
    
    // Check TRIGGER "Low Power"
    HandleGlobalTrigger(TRIGGER_EVENT_LOW_POWER, this->getPlayerNum());
}

size_t Player::getNumUnits() 
{
	return unitpool.size();
}

size_t Player::getNumStructs() const 
{
	return structurepool.size();
}

const std::vector<Unit*>& Player::getUnits() const {return unitpool;}
const std::vector<Structure*>& Player::getStructures() const {return structurepool;}
Uint8 Player::getStructpalNum() const {return structpalnum;}
Uint8 Player::getUnitpalNum() const {return unitpalnum;}
Uint32 Player::getPower() const {return powerGenerated;}

Uint32 Player::getPowerUsed() const 
{
	return powerUsed;
}

Uint16 Player::getPlayerStart() const 
{
	return playerstart;
}

void Player::placeMultiUnits()
{
	printf ("%s line %i: Place multi units (MCV)\n", __FILE__, __LINE__);
	// Create the unit in the pool
    p::uspool->createUnit("MCV", playerstart, 0, playernum, 256, 0, 0, "None");
}

void Player::updateOwner(Uint8 newnum)
{
    Uint32 i;
    for (i=0;i<unitpool.size();++i){
        unitpool[i]->setOwner(newnum);
    }
    for (i=0;i<structurepool.size();++i){
        structurepool[i]->setOwner(newnum);
    }
}

bool Player::isDefeated() const
{
	return defeated;
}

bool Player::isAllied(Player* pl) const 
{
    for (Uint16 i = 0; i < allies.size() ; ++i) {
        if (allies[i] == pl)
            return true;
    }
    return false;
}

size_t Player::getNumAllies() const 
{
	return allies.size() - unallycalls;
}

bool Player::allyWithPlayer(Player* pl)
{
    Uint16 i;
    if (isAllied(pl)) {
        return false;
    }
    if (unallycalls == 0) {
        allies.push_back(pl);
        pl->didAlly(this);
        return true;
    } else {
        for (i=0;i<allies.size();++i) {
            if (allies[i] == NULL) {
                allies[i] = pl;
                pl->didAlly(this);
                --unallycalls;
                return true;
            }
        }
    }
    // shouldn't get here, but in case unallycalls becomes invalid
    allies.push_back(pl);
    pl->didAlly(this);
    unallycalls = 0;
    return true;
}

void Player::didAlly(Player* pl)
{
    Uint32 i;
    if (isAllied(pl)) {
        return;
    }
    for (i=0;i<non_reciproc_allies.size();++i) {
        if (non_reciproc_allies[i] == pl) {
            // player has reciprocated alliance, remove from one-sided ally list
            non_reciproc_allies[i] = NULL;
            return;
        }
    }
    non_reciproc_allies.push_back(pl);
}

bool Player::unallyWithPlayer(Player* pl)
{
    Uint16 i;
    if (pl == this) {
        return false;
    }
    for (i=0;i<allies.size();++i) {
        if (allies[i] == pl) {
            allies[i] = NULL;
            pl->didUnally(this);
            ++unallycalls;
            return true;
        }
    }
    return true;
}

void Player::didUnally(Player* pl)
{
    if (isAllied(pl)) {
        unallyWithPlayer(pl);
    }
}

void Player::setAlliances()
{
    INIFile *mapini;
    std::vector<char*> allies_n;
    char *tmp;

	// Get map ini
	mapini = p::ppool->getMapINI();
    
	// populate "allies_n" with allies
    tmp = mapini->readString(playername, "Allies");
    if( tmp != NULL ) {    	
    	allies_n = splitList(tmp,',');
		if (tmp != NULL){
			delete[] tmp;
		}
		tmp = NULL;
    }

    // always allied to self
    allyWithPlayer(this);
    // no initial allies for multiplayer
    if (multiside == 0) {
        for (Uint16 i=0;i<allies_n.size();++i) {
            if (strcmp((allies_n[i]), (playername))) {
                allyWithPlayer(p::ppool->getPlayerByName(allies_n[i]));
            }
			if (allies_n[i] != NULL)
				delete[] allies_n[i];
			allies_n[i] = NULL;
        }
    } else {
        for (Uint16 i=0;i<allies_n.size();++i) {
			if (allies_n[i] != NULL)
				delete[] allies_n[i];
			allies_n[i] = NULL;
        }
    }
    // since this part of the initialisation is deferred until after map
    // is loaded check for no units or structures
    if (unitpool.empty() && structurepool.empty()) {
        defeated = true;
        clearAlliances();
        return;
    }
}

void Player::clearAlliances()
{
    Uint32 i;
    Player* tmp;
    for (i = 0; i < allies.size() ; ++i) {
        if (allies[i] != NULL) {
            tmp = allies[i];
            unallyWithPlayer(tmp);
        }
    }
    for (i = 0; i < non_reciproc_allies.size() ; ++i) {
        if (non_reciproc_allies[i] != NULL) {
            non_reciproc_allies[i]->unallyWithPlayer(this);
        }
    }
}

void Player::addUnitKill() {++unitkills;}

void Player::addStructureKill() {++structurekills;}

Uint32 Player::getUnitKills() const {return unitkills;}

Uint32 Player::getUnitLosses() const {return unitlosses;}

Uint32 Player::getStructureKills() const {return structurekills;}

Uint16 Player::getStructureLosses() const {return structurelosses;}

size_t Player::ownsStructure(StructureType* stype) 
{
	return structures_owned[stype].size();
}

Structure*& Player::getPrimary(const UnitOrStructureType* uostype) 
{
	return primary_structure[uostype->getPType()];
}
	
Structure*& Player::getPrimary(Uint32 ptype) 
{
	return primary_structure[ptype];
}
	
void Player::setPrimary(Structure* str)
{
    StructureType* st = (StructureType*)str->getType();
    if (st->primarySettable()) {
        Structure*& os = getPrimary(st);
        if (0 != os) {
            os->setPrimary(false);
        }
        os = str; // This works because os is a reference.
        str->setPrimary(true);
    }
}

/**
 * Reveal the map for the player around a waypoint
 * 
 * @param waypointNumber number of the waypoint of the map
 */
void Player::revealAroundWaypoint(Uint32 waypointNumber)
{
	Uint32 wp_cellpos; // Position of the waypoint
	
	// Get the position of the waypoint
	wp_cellpos = p::ccmap->getWaypoint(waypointNumber);
	
	// Add the visibility for the player (1x1 with diameter/2 of 4)
	addSoB(wp_cellpos, 1, 1, 4, SOB_SIGHT);
}

/**
 * si SOB_update = SOB_SIGHT alors update la vue sinon le build
 */
void Player::setVisBuild(SOB_update mode, bool val)
{
    if (mode == SOB_SIGHT) {
        fill(mapVisible.begin(), mapVisible.end(), val);
    } else {
        fill(mapBuildable.begin(), mapBuildable.end(), val);
    }
}

vector<bool>& Player::getMapVis() 
{
	return mapVisible;
}

vector<bool>& Player::getMapBuildable() 
{
	return mapBuildable;
}

/**
 */
void Player::addSoB(Uint32 pos, Uint8 width, Uint8 height, Uint8 sight, SOB_update mode)
{
	// for each line of the screen
	for (int j=0; j<height; j++)
	{
		// for each column of the screen
		for (int i = 0; i<width; i++)
		{
			// Turns on cells
			addSoB(pos + i + (j*p::ccmap->getWidth()), sight, mode);
		}
	}
}

#if 1

void Player::addSoB(Uint32 pos, Uint8 sight, SOB_update mode)
{
	Uint32 initX = 0;
	Uint32 endX = 0;
	Uint32 initY = 0;
	Uint32 endY = 0;
	vector<bool>* mapVoB = 0;
	


	if (mode == SOB_SIGHT) {
        mapVoB = &mapVisible;
    } else if (mode == SOB_BUILD) {
        mapVoB = &mapBuildable;
        sight  = brad;			// Buildable radius from config file internal-global.ini
    } else {
        //logger->error("addSoB was given an invalid mode: %i\n", mode);
        //return;
        // By default it's about visibility
        mapVoB = &mapVisible;
    }
    		
	
	// check min X
	if ((pos % p::ccmap->getWidth() - sight) < 0)
	{
		initX = 0;
	} else {
		initX = pos % p::ccmap->getWidth() - sight;
	}
	
	// check max X
	if ((pos % p::ccmap->getWidth() + sight +1) > p::ccmap->getWidth())
	{
		endX = p::ccmap->getWidth();
	} else {
		endX = pos % p::ccmap->getWidth() + sight+1;
	}
	
	
	// check min Y
	if ((pos / p::ccmap->getWidth() - sight) < 0)
	{
		initY = 0;
	} else {
		initY = pos / p::ccmap->getWidth() - sight;
	}
	
	// check max Y
	if ((pos / p::ccmap->getWidth() + sight + 1) > p::ccmap->getWidth())
	{
		endY = p::ccmap->getWidth();
	} else {
		endY = pos / p::ccmap->getWidth() + sight+1;
	}
	
	
	// parse all cell
	Uint32 curpos = 0;
	Uint32 xtiles = 0;
	Uint32 ytiles = 0;
	Uint32 xpos, ypos;	
	for (ypos = initY; ypos < endY; ypos++)
	{
		for (xpos = initX; xpos < endX; xpos++)
		{
			curpos = xpos + ypos * p::ccmap->getWidth();
			
			// x
			if (pos % p::ccmap->getWidth() > curpos % p::ccmap->getWidth()) 
			{
				xtiles = pos % p::ccmap->getWidth() - curpos % p::ccmap->getWidth();
			} else {
				xtiles = curpos % p::ccmap->getWidth() - pos % p::ccmap->getWidth();			
			}
			// y
			if (pos / p::ccmap->getWidth() > curpos / p::ccmap->getWidth()) {
				ytiles = pos / p::ccmap->getWidth() - curpos / p::ccmap->getWidth();
			} else {
				ytiles = curpos / p::ccmap->getWidth() - pos / p::ccmap->getWidth();
			}
			
			
			int distance = xtiles*xtiles + ytiles*ytiles;
			
			//printf("curpos=%d\n", curpos);
			
			//printf("x=%d y=%d xt=%d yt=%d  distance=%d\n", xpos, ypos, xtiles, ytiles, distance);
			//double dSight = sight;
	
			if (distance <= (sight*sight))
			{
				sightMatrix[curpos] += (mode == SOB_SIGHT);
				buildMatrix[curpos] += (mode == SOB_BUILD);
				(*mapVoB)[curpos] = true;        	
			}
		}
	}
	
	
}

#else

void Player::addSoB(Uint32 pos, Uint8 width, Uint8 height, Uint8 sight, SOB_update mode)
{
    Uint32 curpos, xsize, ysize, cpos;
    Sint32 xstart, ystart;
    vector<bool>* mapVoB = 0;


    if (mode == SOB_SIGHT) {
        mapVoB = &mapVisible;
    } else if (mode == SOB_BUILD) {
        mapVoB = &mapBuildable;
        sight  = brad;			// Buildable radius from config file internal-global.ini
    } else {
        logger->error("addSoB was given an invalid mode: %i\n", mode);
        return;
    }
    xstart = pos%p::ccmap->getWidth() - sight;
    xsize = 2*sight+width;
    if( xstart < 0 ) {
        xsize += xstart;
        xstart = 0;
    }
    if( xstart+xsize > p::ccmap->getWidth() ) {
        xsize = p::ccmap->getWidth()-xstart;
    }
    ystart = pos/p::ccmap->getWidth() - sight;
    ysize = 2*sight+height;
    if( ystart < 0 ) {
        ysize += ystart;
        ystart = 0;
    }
    if( ystart+ysize > p::ccmap->getHeight() ) {
        ysize = p::ccmap->getHeight()-ystart;
    }
    curpos = ystart*p::ccmap->getWidth()+xstart;
    for( cpos = 0; cpos < xsize*ysize; cpos++ ) {
        sightMatrix[curpos] += (mode == SOB_SIGHT);
        buildMatrix[curpos] += (mode == SOB_BUILD);
        (*mapVoB)[curpos] = true;
        curpos++;
        if (cpos%xsize == xsize-1)
            curpos += p::ccmap->getWidth()-xsize;
    }
}

#endif

void Player::removeSoB(Uint32 pos, Uint8 width, Uint8 height, Uint8 sight, SOB_update mode)
{
    Uint32 curpos, xsize, ysize, cpos;
    Sint32 xstart, ystart;

//	printf ("%s line %i: Remove sob, pos = %i, mapsize = %i, sightMatrix.size() = %i\n", __FILE__, __LINE__,  pos, p::ccmap->getWidth()*p::ccmap->getHeight(), sightMatrix.size());
//	printf ("%s line %i: width = %i, height = %i, sight = %i\n", __FILE__, __LINE__, width, height, sight);
//	printf ("%s line %i: mwid = %i\n", __FILE__, __LINE__,  mwid);
//	printf ("%s line %i: brad = %i\n", __FILE__, __LINE__, brad);


    if (mode == SOB_BUILD) {
        sight = brad;
    }

//	printf ("%s line %i: pos%%mwid = %i, sight = %i\n", __FILE__, __LINE__, pos%mwid, sight);
    xstart = pos%mwid - sight;
    xsize = 2*sight+width;
    if( xstart < 0 ) {
        xsize += xstart;
        xstart = 0;
    }
    if( xstart+xsize > mwid ) {
        xsize = mwid-xstart;
    }



    ystart = pos/mwid - sight;
    ysize = 2*sight+height;
    if( ystart < 0 ) {
        ysize += ystart;
        ystart = 0;
    }
    if( ystart+ysize > p::ccmap->getHeight() ) {
        ysize = p::ccmap->getHeight()-ystart;
    }
    curpos = ystart*mwid+xstart;

//	printf ("%s line %i: Starting curpos = %i, xsize*ysize = %i\n", __FILE__, __LINE__, curpos, xsize*ysize);

    // I've done it this way to make each loop more efficient. (zx64)
    if (mode == SOB_SIGHT) {
        for( cpos = 0; cpos < xsize*ysize; cpos++ ) {
            // sightMatrix[curpos] will never be < 1 here
			if ( curpos >= sightMatrix.size() ){
				printf ("%s line %i: ERROR sigtMatrix size error, curpos = %i, mapsize = %i, width = %i height = %i\n", __FILE__, __LINE__, curpos, p::ccmap->getWidth()*p::ccmap->getHeight(), p::ccmap->getWidth(), p::ccmap->getHeight());
				return;
			}
			sightMatrix[curpos]--;
            curpos++;
            if (cpos%xsize == xsize-1)
                curpos += mwid-xsize;
        }
    } else if (mode == SOB_BUILD && !buildany) {
        for( cpos = 0; cpos < xsize*ysize; cpos++ ){

           // sightMatrix[curpos] will never be < 1 here
			if ( curpos >= sightMatrix.size() ){
				printf ("%s line %i: ERROR sigtMatrix size error, curpos = %i, mapsize = %i, width = %i height = %i\n", __FILE__, __LINE__, curpos, p::ccmap->getWidth()*p::ccmap->getHeight(), p::ccmap->getWidth(), p::ccmap->getHeight());
				return;
			}

            if (buildMatrix[curpos] <= 1) {
                mapBuildable[curpos] = false;
                buildMatrix[curpos] = 0;
            } else {
                --buildMatrix[curpos];
            }
            curpos++;
            if (cpos%xsize == xsize-1)
                curpos += mwid-xsize;
        }
    }
}

bool Player::canBuildAll() const 
{
	return buildall;
}
	
bool Player::canBuildAny() const 
{
	return buildany;
}
	
bool Player::canSeeAll() const 
{
	return allmap;
}

bool Player::hasInfMoney() const 
{
	return infmoney;
}
	
void Player::enableBuildAll() 
{
	buildall = true;
}

void Player::enableInfMoney() 
{
	infmoney = true;
}


Sint32 Player::getTechLevel()
{
	return techLevel;
}

Player::Player() 
{
}

Player::Player(const Player&) 
{
}

/**
 * @return Number of Radars.
 */
Uint32 Player::getNumberRadars()
{
	return numberRadars;
}
	
/** 
 * @param value Victorious Set if the player is victorious
 */
void Player::setVictorious(bool value)
{
	this->victorious = value;
}
	
/** 
 * @return <code>true</code> if the player is victorious else <code>false</code>
 */
bool Player::isVictorious()
{
	return this->victorious;
}

