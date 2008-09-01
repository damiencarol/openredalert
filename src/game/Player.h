// Player.h
// 1.5

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

#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <map>
#include <vector>

#include "SDL/SDL_types.h"

#include "ConStatus.h"
#include "UnitOrStructureType.h"
#include "Unit.h"

class MoneyCounter;
class Structure;
class StructureType;
class BQueue;
class INIFile;

using std::map;
using std::list;
using std::vector;

/**
 * Player in game
 */
class Player
{
public:
	explicit Player(const char *pname, INIFile *mapini);
	~Player();
	
	bool isLPlayer();
	void setPlayerNum(Uint8 num);
	void setMultiColour(const char* colour);
	void setMultiColour(const int colour);
	Uint8 getMultiColour (void) ;
	void setSettings(const char* nick, const char* colour, const char* mside);
	void setSettings(const char* nick, const int colour, const char* mside);

	Uint8 getPlayerNum() const ;
	const char* getName() const ;
	Uint8 getSide() const ;
	bool setSide(Uint8 Side);
	Uint8 getMSide() const ;

	bool changeMoney(Sint32 change);
	void setMoney(Sint32 NewMoney);
	Sint32 getMoney() const ;

	bool startBuilding(UnitOrStructureType * type);
	ConStatus stopBuilding(UnitOrStructureType* type);
    void pauseBuilding(void);
    void resumeBuilding(void);
	void placed(UnitOrStructureType* type);
	ConStatus getStatus(UnitOrStructureType* type, Uint8* quantity, Uint8* progress);
	BQueue* getQueue(Uint8 ptype);

	void builtUnit(Unit* un);
	void lostUnit(Unit* un, bool wasDeployed);
	void movedUnit(Uint32 oldpos, Uint32 newpos, Uint8 sight);
	void builtStruct(Structure* str);
	void lostStruct(Structure* str);

	size_t getNumUnits() ;
	size_t getNumStructs() const ;
	const vector<Unit*>& getUnits() const ;
	const vector<Structure*>& getStructures() const ;

	Uint8 getStructpalNum() const ;
	Uint8 getUnitpalNum() const ;

	Uint32 getPower() const ;
	Uint32 getPowerUsed() const ;

	Uint16 getPlayerStart() const ;
	void placeMultiUnits();

	void updateOwner(Uint8 newnum);

	bool isDefeated() const;

	bool isAllied(Player* pl) const;
	size_t getNumAllies() const ;
	bool allyWithPlayer(Player* pl);
	void didAlly(Player* pl);
	bool unallyWithPlayer(Player* pl);
	void didUnally(Player* pl);
	void setAlliances();
	void clearAlliances();

	void addUnitKill() ;
	void addStructureKill() ;
	Uint32 getUnitKills() const ;
	Uint32 getUnitLosses() const ;
	Uint32 getStructureKills() const ;
	Uint16 getStructureLosses() const ;

	size_t ownsStructure(StructureType* stype) ;
	Structure*& getPrimary(const UnitOrStructureType* uostype) ;
	Structure*& getPrimary(Uint32 ptype) ;
	void setPrimary(Structure* str);

	// SOB == Sight or Build.
	void revealAroundWaypoint(Uint32 Waypoint);
	enum SOB_update { SOB_SIGHT = 1, SOB_BUILD = 2 };
	void setVisBuild(SOB_update mode, bool val);
	vector<bool>& getMapVis() ;
	vector<bool>& getMapBuildable() ;

	/** Turns on a block of cells in either the sight or buildable matrix */
	void addSoB(Uint32 pos, Uint8 width, Uint8 height, Uint8 sight, SOB_update mode);
	/** Turns on cells around one cell depending on sight */
	void addSoB(Uint32 pos, Uint8 sight, SOB_update mode);
	
	/** Turns off a block of cells in either the sight or buildable matrix */
	void removeSoB(Uint32 pos, Uint8 width, Uint8 height, Uint8 sight, SOB_update mode);

	bool canBuildAll() const;
	bool canBuildAny() const;
	bool canSeeAll() const;
	bool hasInfMoney() const;
	void enableBuildAll();
	void enableInfMoney();
	
	Sint32 getTechLevel();
	/** Return number of radar of the player */
	Uint32 getNumberRadars();
	
	/** Set if the player is victorious */
	void setVictorious(bool victorious);
	/** Return if the player is victorious */
	bool isVictorious();
	
private:
	/** Do not want player being constructed using default constructor*/
	Player() ;
	Player(const Player&) ;

	/** This instead of a vector as we don't have to check ranges before operations*/
	map<Uint8, BQueue*> queues;

	bool defeated;
	char* playername;
	char* nickname;
	Uint8 playerside;
	Uint8 multiside;
	Uint8 playernum;
	Uint8 unitpalnum;
	Uint8 structpalnum;
	
	/** TechLevel of the player in the map */
	Sint32 techLevel;

	/** See the alliance code in the .cpp file*/
	Uint8 unallycalls;

	Sint32 money;
	Uint32 powerGenerated; 
	Uint32 powerUsed;

	Uint32 unitkills; 
	Uint32 unitlosses;
	Uint32 structurekills; 
	Uint32 structurelosses;

	Uint16 playerstart;

	// All of these pointers are owned elsewhere.
	vector<Unit*> unitpool;
	vector<Structure*> structurepool;
	map<StructureType*, list<Structure*> > structures_owned;
	map<Uint32, list<Structure*> > production_groups;
	map<Uint32, Structure*> primary_structure;

	vector<Player*> allies;
	
	/**
     * players that have allied with this player, but this player
	 * has not allied in return.  Used to force an unally when player
	 * is defeated.
     */
	vector<Player*> non_reciproc_allies;

	vector<Uint8> sightMatrix;
	vector<Uint8> buildMatrix;

	/** List of location that is visible by player */
	vector<bool> mapVisible; 
	/** List of location that is buildable by player */
	vector<bool> mapBuildable;
	
	/** cheat/debug flags: allmap (reveal all map) */
	bool allmap; 
	/** cheat/debug flags: buildany (remove proximity check) */
	bool buildany;
	/** cheat/debug flags: buildall (disable prerequisites) */
	bool buildall; 
	/** cheat/debug flags: infmoney (doesn't care if money goes negative).*/
	bool infmoney;

	/** Event that manage Money variations */ 
	MoneyCounter* counter;

	/** Number of radars structures */
	Uint32 numberRadars;

	/** buildable_radius */
	Uint8 brad;
	/** @todo : REMOVE THAT  Map Width */
	Uint16 mwid;
	
	/** Is true if the player is victorious */
	bool victorious;
};

#endif //PLAYER_H
