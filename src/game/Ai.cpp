// Ai.cpp
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

#include "Ai.h"

#include <stdexcept>
#include <math.h>

#include "SDL/SDL_timer.h"

#include "ActionEventQueue.h"
#include "CnCMap.h"
#include "PlayerPool.h"
#include "game/Unit.h"
#include "UnitAndStructurePool.h"
//#include "Weaponspool.h"
#include "audio/SoundEngine.h"
#include "Dispatcher.h"
#include "video/Renderer.h"
#include "misc/INIFile.h"
#include "GameMode.h"
#include "UnitOrStructure.h"
#include "Structure.h"
#include "pside.h"
#include "Player.h"
#include "AiRules.h"
#include "Unit.h"
#include "include/Logger.h"
#include "include/config.h"

#define DEBUG_AI

namespace p {
	extern CnCMap* ccmap;
	extern UnitAndStructurePool* uspool;
	extern PlayerPool* ppool;
	extern Dispatcher* dispatcher;
}
extern Logger * logger;

/**
 *  AI constructor
 */
Ai::Ai()
{
	this->Difficulty = 1;
	LplayerBasePos = 0;
	guide = false;

	NumbPlayers = p::ppool->getNumPlayers();

	for (unsigned int i = 0; i < (unsigned)NumbPlayers; i++)
	{
		LastGuideTickCount.push_back(SDL_GetTicks());
		LastBuildTickCount.push_back(SDL_GetTicks());
		LastAttackTickCount.push_back(SDL_GetTicks());
		LastDefendTickCount.push_back(SDL_GetTicks());
		LastHarvestTickCount.push_back(SDL_GetTicks());
		LastGuideGatherTickCount.push_back(SDL_GetTicks());
		RetryDeploy.push_back(false);
		player_targets.push_back(0);
		UnitBuildMultiplexer.push_back(0);
	}
//  AiDifficultyTimer.Start();
	
	// Create Rules
	Rules = new AiRules();


	/// Load the sounds ini file
	INIFile *rules_ini = GetConfig("rules.ini");
	if (rules_ini != 0)
	{
		Rules->AttackInterval = rules_ini->readInt	("AI", "AttackInterval", 3);
		Rules->AttackDelay	= rules_ini->readInt	("AI", "AttackDelay", 5);
		Rules->PatrolScan	= rules_ini->readFloat	("AI", "PatrolScan", .016);
		Rules->CreditReserve	= rules_ini->readInt	("AI", "CreditReserve", 100);
		Rules->PathDelay		= rules_ini->readFloat	("AI", "PathDelay", .01);
		Rules->OreNearScan	= rules_ini->readInt	("AI", "OreNearScan", 6);
		Rules->OreFarScan	= rules_ini->readInt	("AI", "OreFarScan", 48);
		Rules->AutocreateTime	= rules_ini->readInt	("AI", "AutocreateTime", 5);
		Rules->InfantryReserve	= rules_ini->readInt	("AI", "InfantryReserve", 3000);
		Rules->InfantryBaseMult	= rules_ini->readInt	("AI", "InfantryBaseMult", 1);
		Rules->PowerSurplus	= rules_ini->readInt	("AI", "PowerSurplus", 50);
		Rules->BaseSizeAdd	= rules_ini->readInt	("AI", "BaseSizeAdd", 3);
		Rules->RefineryRatio	= rules_ini->readFloat	("AI", "RefineryRatio", .16);
		Rules->RefineryLimit	= rules_ini->readInt	("AI", "RefineryLimit", 4);
		Rules->BarracksRatio	= rules_ini->readFloat	("AI", "BarracksRatio", .16);
		Rules->BarracksLimit	= rules_ini->readInt	("AI", "BarracksLimit", 2);
		Rules->WarRatio		= rules_ini->readFloat	("AI", "WarRatio", .1);
		Rules->WarLimit		= rules_ini->readInt	("AI", "WarLimit", 2);
		Rules->DefenseRatio	= rules_ini->readFloat	("AI", "DefenseRatio", .4);
		Rules->DefenseLimit	= rules_ini->readInt	("AI", "DefenseLimit", 40);
		Rules->AARatio		= rules_ini->readFloat	("AI", "AARatio", .14);
		Rules->AALimit		= rules_ini->readInt	("AI", "AALimit", 10);
		Rules->TeslaRatio	= rules_ini->readFloat	("AI", "TeslaRatio", .16);
		Rules->TeslaLimit	= rules_ini->readInt	("AI", "TeslaLimit", 10);
		Rules->HelipadRatio	= rules_ini->readFloat	("AI", "HelipadRatio", .12);
		Rules->HelipadLimit	= rules_ini->readInt	("AI", "HelipadLimit", 5);
		Rules->AirstripRatio	= rules_ini->readFloat	("AI", "AirstripRatio", .12);
		Rules->AirstripLimit	= rules_ini->readInt	("AI", "AirstripLimit", 5);
//		Rules->CompEasyBonus	= rules_ini->readInt("AI", "CompEasyBonus", true);
//		Rules->Paranoid		= rules_ini->readInt("AI", "Paranoid", true);
		Rules->CompEasyBonus	= true; // @todo read this value from the config file
		Rules->Paranoid		= true; // @todo read this value from the config file
		Rules->PowerEmergency	= rules_ini->readInt	("AI", "PowerEmergency", 75);
	}else{
		Rules->AttackInterval	= 3;
		Rules->AttackDelay	= 5;
		Rules->PatrolScan	=.016;
		Rules->CreditReserve	=100;
		Rules->PathDelay		=.01;
		Rules->OreNearScan	= 6;
		Rules->OreFarScan	= 48;
		Rules->AutocreateTime	=5;
		Rules->InfantryReserve	=3000;
		Rules->InfantryBaseMult	= 1;
		Rules->PowerSurplus	= 50;
		Rules->BaseSizeAdd	= 3;
		Rules->RefineryRatio	= .16;
		Rules->RefineryLimit	= 4;
		Rules->BarracksRatio	= .16;
		Rules->BarracksLimit	= 2;
		Rules->WarRatio		=.1;
		Rules->WarLimit		= 2;
		Rules->DefenseRatio	= .4;
		Rules->DefenseLimit	= 40;
		Rules->AARatio		=.14;
		Rules->AALimit		= 10;
		Rules->TeslaRatio	= .16;
		Rules->TeslaLimit	= 10;
		Rules->HelipadRatio	= .12;
		Rules->HelipadLimit	= 5;
		Rules->AirstripRatio	= .12;
		Rules->AirstripLimit	= 5;
		Rules->CompEasyBonus	= true;
		Rules->Paranoid		= true;
		Rules->PowerEmergency	= 75;
	}

	Rules->AlwaysAlly = true;
}

Ai::~Ai()
{
	// Free ai rules
	delete Rules;
}

void Ai::SetDifficulty(int Diff)
{
	if (Diff > 0 && Diff <= 4)
	{
		this->Difficulty = Diff;
	}
}

void Ai::DefendUnits(Player* pPlayer, int pPlayerNumb)
{
	UnitOrStructure     *lEnemy = NULL;
	int                 lNumbUnits;
	vector<Unit*>	lUnitpool;
	Unit                *lUnit;
	Unit				*lEnemyUnit;
	Structure           *lEnemyStructure;

	// Get the number of Unit of the player
	lNumbUnits = pPlayer->getNumUnits();

	// Get a list of Units of the player
	lUnitpool = pPlayer->getUnits();

	// For each unit from this player
	for (int lUnitNumb = 0; lUnitNumb < lNumbUnits; lUnitNumb++)
	{

		lUnit = lUnitpool[lUnitNumb];

		// Check that the unit can attack and that the unit doens't have other thing to do
		if (!lUnit->canAttack() || lUnit->IsAttacking() || lUnit->IsMoving ()){
			continue;
		}
		
		lEnemyUnit = EnemyUnitInRange (pPlayerNumb, lUnit );
		
		if (lEnemyUnit != NULL && lUnit->canAttack()){

			// Don't distract units on a AI mission (only abort the mission if we see a harvester)
			if (!lUnit->UnderAttack () && lUnit->AI_Mission != 1 ){
				// Abort the ai mission if we 'see' a harvester
				if (lEnemyUnit->is("HARV"))
					lUnit->AI_Mission = 1;
				else
					continue;
			}

//			Enemy  = p::uspool->getUnitOrStructureAt(EnemyUnit->getPos(), EnemyUnit->getSubpos());
			lEnemy  = (UnitOrStructure*) p::uspool->getGroundUnitAt ( lEnemyUnit->getPos(), lEnemyUnit->getSubpos() );

			if (lEnemy == NULL)
				lEnemy  = (UnitOrStructure*) p::uspool->getFlyingAt ( lEnemyUnit->getPos(), lEnemyUnit->getSubpos() );

			if (lEnemy == NULL)
				lEnemy  = (UnitOrStructure*) p::uspool->getStructureAt ( lEnemyUnit->getPos(), lEnemyUnit->getSubpos(), false );


			if (lEnemy != NULL){
				lUnit->attack(lEnemy);
				return;
			}
		}

		lEnemyStructure = EnemyStructureInRange ( pPlayerNumb, lUnit );
		if (lEnemyStructure != NULL && lUnit->canAttack())
		{
			//Enemy  = p::uspool->getUnitOrStructureAt(EnemyStructure->getPos(), EnemyStructure->getSubpos());

//			if ( Enemy != NULL ){
//				Unit->attack(Enemy);
				lUnit->attack(lEnemyStructure);
				return;
//			}
		}
	}
}

void Ai::DefendComputerPlayerBaseUnderAttack (Player *Player, int PlayerNumb, UnitOrStructure *Enemy, Structure *StructureUnderAttack)
{
	int			NumbUnits = 0;
	vector<Unit*>	unitpool;
	Unit* Unit = 0;
	int	Distance;
	int SightRange;
	int StructureUnderAttackPos;

	if (Enemy == 0 || StructureUnderAttack == 0){
		return;
	}
	if (Enemy->getOwner() == StructureUnderAttack->getOwner()){
		return;
	}
	//
	// don't try to build for the human player
	// We should actually check if building is allowed and only build when allowed (by triggers)
	//
	if (PlayerNumb == this->HumanPlayerNumb){
		return;
	}

	NumbUnits				= Player->getNumUnits();
	unitpool				= Player->getUnits();
	StructureUnderAttackPos	= StructureUnderAttack->getPos();

	// Repair the structure (if needed)
	if (StructureUnderAttack->getType()->getMaxHealth() > StructureUnderAttack->getHealth()){
		if (!StructureUnderAttack->isRepairing ())
			StructureUnderAttack->repair();
	}

	for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++)
	{
		Unit = unitpool[UnitNumb];

		if (/*Unit->IsAttacking () ||*/ !Unit->canAttack())
			continue;

		Distance	= Unit->getDist(StructureUnderAttackPos);
		SightRange	= Unit->getType()->getSight();

		if ( Distance < (SightRange * 3) /*&& !Unit->IsAttacking ()*/){
			Unit->attack ((UnitOrStructure*)Enemy);
		}
	}
}

void Ai::DefendComputerPlayerUnitUnderAttack (Player *Player, int PlayerNumb, UnitOrStructure *Enemy, Unit *UnitUnderAttack)
{
	int			NumbUnits = 0;
	vector<Unit*>	unitpool;
	Unit* lUnit = 0;
	int	Distance, SightRange, UnitUnderAttackPos;
	
	

	if ( Enemy == NULL || UnitUnderAttack == NULL)
		return;

	if (Enemy->getOwner() == UnitUnderAttack->getOwner())
		return;

	// don't try to defend for the human player
	if (PlayerNumb == this->HumanPlayerNumb){
		return;
	}

	// Start with shooting back
	if (UnitUnderAttack->canAttack())
		UnitUnderAttack->attack (Enemy);


	NumbUnits	= Player->getNumUnits();
	unitpool	= Player->getUnits();

	UnitUnderAttackPos = UnitUnderAttack->getPos();

	for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++){
		lUnit = unitpool[UnitNumb];

		if (/*lUnit->IsAttacking () ||*/ !lUnit->canAttack())
			continue;

		Distance	= lUnit->getDist(UnitUnderAttackPos);
		SightRange	= lUnit->getType()->getSight();

		// If I can see my comrade under attack
		if ( Distance < SightRange){
			lUnit->attack ((UnitOrStructure*)Enemy);
		}
	}
}


/**
 *	This is a beginning, for now this function only deploy's the starting MCV
 */
void Ai::handle()
{
	Player * CurPlayer;
	static bool DelayAttack = true;
	// Structures
	//char			*Name;

	// Don't burn our fingers on multi player mode...
	if (p::ccmap->getGameMode() == GAME_MODE_MULTI_PLAYER){
		return;
	}

	switch (this->Difficulty){
		case 1:
//			if (AiDifficultyTimer.Elapsed () < 2)
//				return;
			break;

		case 2:
//			if (AiDifficultyTimer.Elapsed () < 1)
//				return;
			break;

		case 3:
//			if (AiDifficultyTimer.Elapsed () < 0.5)
//				return;
			break;

		case 4:
			// Don't wait
			break;
	}


//	c 	= p::ppool->getNumPlayers();
	this->HumanPlayerNumb	= p::ppool->getLPlayerNum();

	// This part of the AI takes care of the computer player attacking a enemy within range
	// and of the human player fighting back when attacked.
	for (int PlayerNumb = 0; PlayerNumb < this->NumbPlayers; PlayerNumb++){

		CurPlayer = p::ppool->getPlayer(PlayerNumb);

		// Don't try to control defeated players
		if (CurPlayer->isDefeated()){
			continue;
		}

		if ((SDL_GetTicks() - LastDefendTickCount[PlayerNumb]) > 100){
			LastDefendTickCount[PlayerNumb] = SDL_GetTicks();
			// Let the units defend themselfes
			this->DefendUnits (CurPlayer,PlayerNumb);
			// Let the structures with weapons defend themselfes
			this->DefendStructures (CurPlayer,PlayerNumb);
		}

		//The next part is only for computer players
		if (PlayerNumb == HumanPlayerNumb){
			continue;
		}
		
		if ((SDL_GetTicks() - LastHarvestTickCount[PlayerNumb]) > 1000){
			LastHarvestTickCount[PlayerNumb] = SDL_GetTicks();
			this->Harvest (CurPlayer, PlayerNumb);
		}

		// Retry moving when the path was blocked (maybe we should add some code so tanks will wait for infantry (move in steps to the target)
		if ((SDL_GetTicks() - LastGuideTickCount[PlayerNumb]) > Rules->PathDelay * 60 * 1000 ){
			LastGuideTickCount[PlayerNumb] = SDL_GetTicks();
			this->guideAttack (CurPlayer, PlayerNumb);
		}

		if ((SDL_GetTicks() - LastBuildTickCount[PlayerNumb]) > 24000 || RetryDeploy[PlayerNumb]){
			LastBuildTickCount[PlayerNumb] = SDL_GetTicks();
			RetryDeploy[PlayerNumb] = false;
			this->Build (CurPlayer, PlayerNumb);
		}

		// The first attack is delayed, all other attacks use the attack interval
		if (DelayAttack){
			if ((SDL_GetTicks() - LastAttackTickCount[PlayerNumb]) > (unsigned)(Rules->AttackDelay * 60 * 1000)){
				LastAttackTickCount[PlayerNumb] = SDL_GetTicks();
				this->patrolAndAttack (CurPlayer, PlayerNumb);
			}
			DelayAttack = false;
		}else{
			if ((SDL_GetTicks() - LastAttackTickCount[PlayerNumb]) > (unsigned)(Rules->AttackInterval * 60 * 1000)){
				LastAttackTickCount[PlayerNumb] = SDL_GetTicks();
				this->patrolAndAttack (CurPlayer, PlayerNumb);
			}
		}
	}
}

void Ai::guideAttack (Player *Player, int PlayerNumb) 
{
    int NumbUnits;
    int NumbStructures;
    int lPlayerNumbStructures;
    int lPlayerNumbUnits;
    vector<Structure*>		structurepool, lPlayerStructurePool;
    vector<Structure*>		EnemyTeslaCoils, EnemyPowerPlants, EnemyOreRefs;
    vector<Unit*> unitpool;
    vector<Unit*> lPlayerUnitPool;
    Unit* Unit = NULL, *FirstUnit = NULL, *ClosestUnit = NULL;
    Uint32	MaxDist = 0, TargetDist = 0, NextTargetDist = 0;
    bool StillMoving = false;

	// don't try to control for the human player
	if (PlayerNumb == this->HumanPlayerNumb){
		return;
	}

	// Abort if there is no target
	if (player_targets[PlayerNumb] == NULL)
		return;


	// Handle unit vars
	NumbUnits		= Player->getNumUnits();
	unitpool		= Player->getUnits();

	// Handle structure vars
	structurepool		= Player->getStructures();
	NumbStructures		= Player->getNumStructs();

	// Handle lPlayer vars
	lPlayerStructurePool	= p::ppool->getLPlayer()->getStructures();
	lPlayerNumbStructures	= lPlayerStructurePool.size();
	lPlayerUnitPool		= p::ppool->getLPlayer()->getUnits();
	lPlayerNumbUnits	= lPlayerUnitPool.size();


	if ((lPlayerNumbStructures > 0 || lPlayerNumbUnits > 0) && NumbUnits > 0 ){

		// Init all units to the first unit in the pool
		for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++){
			if (unitpool[UnitNumb]->AI_Mission == 3){
				FirstUnit = ClosestUnit = unitpool[UnitNumb];
			}
		}

		// We don't need to do anything if there are no units attacking the human player
		if (FirstUnit == NULL)
			return;

		// If the target was destroyed, find a new good target
		if (!player_targets[PlayerNumb]->isAlive()){
			player_targets[PlayerNumb] = NULL;

			if (lPlayerStructurePool.size() > 0)
			{
				if (lPlayerStructurePool[0]->isAlive()){
					NextTargetDist = FirstUnit->getDist(lPlayerStructurePool[0]->getPos());
					player_targets[PlayerNumb] = lPlayerStructurePool[0];
				}else
					NextTargetDist = 10000;

				// Find the closest target
				for (unsigned int i = 0; i < lPlayerStructurePool.size(); i++)
				{
					if (FirstUnit->getDist(lPlayerStructurePool[i]->getPos()) < NextTargetDist && lPlayerStructurePool[i]->isAlive()){
						NextTargetDist = FirstUnit->getDist(lPlayerStructurePool[i]->getPos());
						player_targets[PlayerNumb] = lPlayerStructurePool[i];
					}
				}

				// Find the preferred targets
				for (unsigned int i = 0; i < lPlayerStructurePool.size(); i++)
				{
					// Make a list of all nearby tesla's
					if ( lPlayerStructurePool[i]->is("TSLA") && lPlayerStructurePool[i]->isAlive()){
						if (FirstUnit->getDist(lPlayerStructurePool[i]->getPos()) < 4 * NextTargetDist)
							EnemyTeslaCoils.push_back(lPlayerStructurePool[i]);
					}

					// Make a list of all nearby advanced power plants
					if ( lPlayerStructurePool[i]->is("APWR") && lPlayerStructurePool[i]->isAlive()){
						if (FirstUnit->getDist(lPlayerStructurePool[i]->getPos()) < 4 * NextTargetDist)
							EnemyPowerPlants.push_back(lPlayerStructurePool[i]);
					}

					// Make a list of all nearby ore refinery's
					if (lPlayerStructurePool[i]->is("PROC") && lPlayerStructurePool[i]->isAlive()){
						if (FirstUnit->getDist(lPlayerStructurePool[i]->getPos()) < 4 * NextTargetDist)
							EnemyOreRefs.push_back(lPlayerStructurePool[i]);
					}
				}

				// Prefer attacking power plants (so powered defences don't work any more
				if (EnemyPowerPlants.size() > 0){

					player_targets[PlayerNumb] = EnemyPowerPlants[0];
					NextTargetDist = FirstUnit->getDist(EnemyPowerPlants[0]->getPos());

					// find the best power plant to attack
					for (unsigned int i = 0; i < EnemyPowerPlants.size(); i++){
						if (FirstUnit->getDist(EnemyPowerPlants[i]->getPos()) < NextTargetDist){
							NextTargetDist = FirstUnit->getDist(EnemyPowerPlants[i]->getPos());
							player_targets[PlayerNumb] = EnemyPowerPlants[i];
						}
					}

				// Prefer attacking tesla coils next
				}else if (EnemyTeslaCoils.size() > 0 ){

					player_targets[PlayerNumb] = EnemyTeslaCoils[0];
					NextTargetDist = FirstUnit->getDist(EnemyTeslaCoils[0]->getPos());

					// find the best tesla coil to attack
					for (unsigned int i = 0; i < EnemyTeslaCoils.size(); i++){
						if (FirstUnit->getDist(EnemyTeslaCoils[i]->getPos()) < NextTargetDist){
							NextTargetDist = FirstUnit->getDist(EnemyTeslaCoils[i]->getPos());
							player_targets[PlayerNumb] = EnemyTeslaCoils[i];
						}
					}
				// Prefer attacking ore refinery's next
				}else if (EnemyOreRefs.size() > 0){
					player_targets[PlayerNumb] = EnemyOreRefs[0];
					NextTargetDist = FirstUnit->getDist(EnemyOreRefs[0]->getPos());

					// find the best ore refinery to attack
					for (unsigned int i = 0; i < EnemyOreRefs.size(); i++){
						if (FirstUnit->getDist(EnemyOreRefs[i]->getPos()) < NextTargetDist){
							NextTargetDist = FirstUnit->getDist(EnemyOreRefs[i]->getPos());
							player_targets[PlayerNumb] = EnemyOreRefs[i];
						}
					}
				}
			}else if (lPlayerUnitPool.size() > 0){
				///@TDOO, fix this, it is not working jet
				if (lPlayerUnitPool[0]->isAlive()) {
					NextTargetDist = FirstUnit->getDist(lPlayerUnitPool[0]->getPos());
					player_targets[PlayerNumb] = lPlayerUnitPool[0];
				}else
					NextTargetDist = 10000;


				for (unsigned int i = 0; i < lPlayerUnitPool.size(); i++){
					if (FirstUnit->getDist(lPlayerUnitPool[i]->getPos()) < NextTargetDist && lPlayerUnitPool[i]->isAlive()){
						NextTargetDist = FirstUnit->getDist(lPlayerUnitPool[i]->getPos());
						player_targets[PlayerNumb] = lPlayerUnitPool[i];
					}
				}
			}
		}

		// Get out if we can't find any more targets
		if (player_targets[PlayerNumb] == NULL)
			return;

		// Keep our attacking units close together
		for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++){
			Unit = unitpool[UnitNumb];

			if (!Unit->canAttack())
				continue;

			if (Unit->AI_Mission == 3){
				if (Unit->IsMoving() && !Unit->IsAttacking ())
					StillMoving = true;

				if (FirstUnit->getDist(Unit->getPos()) > MaxDist){
					MaxDist = FirstUnit->getDist(Unit->getPos());
					//printf ("Maxdist = %u, FirstUnitName = %s, UnitName = %s\n", MaxDist, FirstUnit->getType()->getName(), Unit->getType()->getName() );
				}

				if (ClosestUnit->getDist(player_targets[PlayerNumb]->getPos()) > Unit->getDist(player_targets[PlayerNumb]->getPos())){
					ClosestUnit	= Unit;
					TargetDist	= Unit->getDist(player_targets[PlayerNumb]->getPos());
				}
			}
		}

		// Attack the target
		for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++){
			Unit = unitpool[UnitNumb];

			if (!Unit->canAttack())
				continue;
			if (Unit->AI_Mission == 3){
				if (player_targets[PlayerNumb]->isAlive() ){
					if (MaxDist > 5 && TargetDist > MaxDist ){
						//printf ("Maxdist = %u\n", MaxDist);
						Unit->move (ClosestUnit->getPos());
					}else{
						if (!Unit->IsAttacking () ){
							Unit->attack( player_targets[PlayerNumb] );
							break;
						}
					}
				}
			}
		}
	}
}

/**
 * 
 */
Uint32 Ai::getDist(Uint32 pos1, Uint32 pos2)
{
    Uint16 x, y, nx, ny, xdiff, ydiff;
    x = pos2%p::ccmap->getWidth();
    y = pos2/p::ccmap->getWidth();
    nx = pos1%p::ccmap->getWidth();
    ny = pos1/p::ccmap->getWidth();

    xdiff = abs(x-nx);
    ydiff = abs(y-ny);
//    return min(xdiff,ydiff)+abs(xdiff-ydiff);
    return (Uint32) sqrt (xdiff*xdiff+ydiff*ydiff);
}


bool Ai::CanBuildAt (Uint8 PlayerNumb, const char *structname, Uint32 pos)
{
	Uint32 placeypos;
	Uint32 placexpos;
	Uint32 curpos;
	Uint16 xpos;
	Uint16 ypos;
	Uint32	br;

    p::ccmap->translateFromPos(pos, &xpos, &ypos);

	StructureType* Type	= p::uspool->getStructureTypeByName(structname);

	vector<bool>& buildable = p::ppool->getPlayer(PlayerNumb)->getMapBuildable();

	// Check that we don't try to build outside the map (copied check from unitandstructurepool.cpp!!
	br = pos + p::ccmap->getWidth()*(Type->getYsize()-1);
	if (pos > p::ccmap->getSize() || (br > p::ccmap->getSize() && 0))
		return false;

	// Prevent wrapping around
	if (xpos + Type->getXsize() > p::ccmap->getWidth())
		return false;

	for (placeypos = 0; placeypos < Type->getYsize(); placeypos++) {
		for (placexpos = 0; placexpos < Type->getXsize(); placexpos++) {

            curpos = pos+placeypos*p::ccmap->getWidth()+placexpos;

			if (!buildable[curpos])
				return false;

			if (p::uspool->getGroundUnitAt(curpos) != NULL || p::uspool->getFlyingAt(curpos) != NULL || p::uspool->getStructureAt(curpos) != NULL)    //p::uspool->getUnitAt(curpos+x) != NULL
				return false;

			if (!p::ccmap->isBuildableAt(PlayerNumb, curpos))
				return false;

		}
	}
	return true;
}

/**
 * 
 */
bool Ai::BuildStructure (Player *Player, int PlayerNumb, const char *structname, Uint32 ConstYardPos)
{
	Uint32 pos = 0;
	Uint16 ConstYard_x;
	Uint16 ConstYard_y;

	p::ccmap->translateFromPos(ConstYardPos, &ConstYard_x, &ConstYard_y);

	// Get the structure type so we can see what the cost are for this structure
	StructureType* type = p::uspool->getStructureTypeByName(structname);
#if 0
	//Check if we can pay for this structure ;)
	if (Player->getMoney() > type->getCost()){
		for (Uint32 offset = 5; offset < 100; offset += 4){
//		Uint32 offset = 20;
			curpos = ConstYardPos - ccmap->getWidth()*(offset/2);
			for (Uint32 pos = curpos; pos < curpos + offset; pos +=4){
				if (CanBuildAt (PlayerNumb, structname, pos) ){
					if (!p::dispatcher->structurePlace(structname, pos, PlayerNumb))
                        			logger->error("Failed to build, ConstYardPosition = %u, pos = %u\n", ConstYardPos, pos);
					else{
						// Succes at building
						return true;
					}
				}
			}
		}
	}
#else
	//Check if we can pay for this structure ;)
	if (Player->getMoney() > type->getCost()){
		for (Uint32 offset = 5; offset < 100; offset += 4){
			for (Uint32 x = ConstYard_x-offset; x < ConstYard_x + offset; x += 4){
				for (Uint32 y = ConstYard_y-offset; y < ConstYard_y + offset; y += 3){
					pos = p::ccmap->translateToPos (x,y);
					if (CanBuildAt (PlayerNumb, structname, pos) ){
						if (!p::dispatcher->structurePlace(structname, pos, PlayerNumb)){
							logger->error("Failed to build, ConstYardPosition = %u, pos = %u\n", ConstYardPos, pos);
						} else {
							// Succes at building
							return true;
						}
					}
				}
			}
		}
	}

#endif
	logger->error("%s line %i: Failed to find free pos\n", __FILE__, __LINE__);
	return false;
}

/**
 * 
 */
unsigned int Ai::FindClosesedTiberium(Unit *Unit)
{
    Uint32 tiberium = 0;
    Uint32 ClosesedPos = 0;
    Uint32 ClosesedDistance = 0;
    Uint32 Distance = 0;
    bool FirstFound = false;

	for (unsigned int pos =0; pos < p::ccmap->getSize(); pos++){
		tiberium = p::ccmap->getResourceFrame(pos);
		if (tiberium != 0) {
			// Found tiberium
			Distance = Unit->getDist(pos);
			if (Distance < ClosesedDistance || !FirstFound){
				ClosesedPos		= pos;
				ClosesedDistance	= Distance;
				FirstFound = true;
			}
		}
	}
	return ClosesedPos;

}


void Ai::DefendStructures (Player *Player, int PlayerNumb)
{
	std::vector<Structure*> structurepool;
//UnitOrStructure         *Enemy = NULL;
int                     NumbStructures;
Structure               *Structure;
Unit                    *EnemyUnit;

	//
	// Make structures defend themselfes
	//
	structurepool = Player->getStructures();
	NumbStructures = Player->getNumStructs();

	// For each structure from this player
	for (int StructNumb = 0; StructNumb < NumbStructures; StructNumb++){
		Structure = structurepool[StructNumb];

//        if (Structure->IsBuilding ())
//            continue;

		if (!Structure->canAttack())
			continue;

		// Check if we have enoug power
		StructureType* st = (StructureType*)Structure->getType();
		if ( st->getPowerInfo().powered){
			if (Player->getPower() < Player->getPowerUsed()){
				continue;
			}
		}

		EnemyUnit = EnemyUnitInRange (PlayerNumb, Structure );

		if (EnemyUnit != NULL && Structure->canAttack()){

//			Enemy  = p::uspool->getUnitOrStructureAt(EnemyUnit->getPos(), EnemyUnit->getSubpos());

//			if (Enemy != NULL && !Structure->IsAttacking ()){
			if (!Structure->IsAttacking ()){
				Structure->attack(EnemyUnit);
			}
		}
	}
}

void Ai::Build (Player *Player, int PlayerNumb)
{
	vector<Structure*>	structurepool;
	int					NumbUnits;
	int					NumbStructures;
	vector<Unit*>	unitpool;
	Unit*			theUnit;
	Structure		*OreRefinery;
	Uint32			NumbOfInfantry = 0, NumbWarfactorys = 0, NumbOreRefs = 0, NumbConstYards = 0, NumbBarracks = 0, NumbOfOreTrucks = 0, NumbTanks = 0, NumbTeslaCoils = 0 , NumbOfPowerPlants = 0;
	Structure*		Structure;
Uint32				ConstYardPos = 0;
//UnitType			*BuildUnitType;
int					cost;
bool				StructureWasBuild = false;
//static bool			Direction = false;
//static Uint32			MoveToPos = 0;
//char				UnitName[10];
Uint16				xpos,
				ypos;

	// Don't build in single player mode (jet).
	if (p::ccmap->getGameMode() == GAME_MODE_SINGLE_PLAYER){
		return;
	}

	// don't try to build for the human player
	if (PlayerNumb == this->HumanPlayerNumb){
		return;
	}

	NumbUnits	= Player->getNumUnits();
	structurepool	= Player->getStructures();
	NumbStructures	= Player->getNumStructs();

	unitpool = Player->getUnits();

	NumbWarfactorys = NumbOreRefs = NumbConstYards = NumbBarracks = 0;


	// Count structures from this computer player
	OreRefinery = NULL;
	for (int StructNumb = 0; StructNumb < NumbStructures; StructNumb++){
		Structure = structurepool[StructNumb];

		if ( Structure->is ("WEAP")){
			NumbWarfactorys++;
		}

		if (Structure->is ("PROC")){
			NumbOreRefs++;
			OreRefinery = Structure;
		}

		if (Structure->is ("FACT")){
			NumbConstYards++;
			ConstYardPos = Structure->getPos();
		}

		if (Structure->is ("TSLA")){
			NumbTeslaCoils++;
		}

		if (Structure->is ("APWR") || Structure->is ("POWR")){
			NumbOfPowerPlants++;
		}

		if (Structure->is ("TENT") || Structure->is ("BARR")){
			NumbBarracks++;
		}
	}

	// Count Units from this computer player
	for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++)
	{
		theUnit = unitpool[UnitNumb];

		if (theUnit->getType()->isInfantry()){
			NumbOfInfantry++;
		}

		if (theUnit->is("HARV") == true)
		{
			if (theUnit->GetBaseRefinery() == 0)
			{
				theUnit->SetBaseRefinery(OreRefinery);
			}
			NumbOfOreTrucks++;
		}

		if (theUnit->is("1TNK") == true)
		{
			NumbTanks++;
		}
	}

	// Check our units if we can deploy them (only for MCV yet)
	if (NumbConstYards == 0)
	{
		for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++)
		{
			theUnit = unitpool[UnitNumb];

			if ( theUnit->is ("MCV") )
			{
				if (theUnit->canDeploy(p::ccmap) == true)
				{
					// We don't have any structures, randomly deploy
					theUnit->deploy();
				} else {
					if (!theUnit->IsMoving())
					{
						p::ccmap->translateFromPos(theUnit->getPos(), &xpos, &ypos);
						if (xpos + 20 < p::ccmap->getWidth()){
							theUnit->move(p::ccmap->translateToPos(xpos + 20, ypos));
						}else{
							theUnit->move(p::ccmap->translateToPos(xpos - 20, ypos));
						}
					}
					RetryDeploy[PlayerNumb] = true;
				}
			}
		}
	}

	if (NumbConstYards > 0){

		// Build ore refinery
		// We should use Rules->RefineryLimit somewhere around here
		if (!StructureWasBuild && (NumbOreRefs == 0 || ( NumbOreRefs < NumbStructures * Rules->RefineryRatio && NumbOreRefs < Rules->RefineryLimit))){
			cost = p::uspool->getStructureTypeByName("PROC")->getCost();
			if (Player->getMoney() > cost){
				if (BuildStructure (Player, PlayerNumb, "PROC", ConstYardPos))
					Player->changeMoney(-1 * cost);
				StructureWasBuild = true;
			}
			// @todo sell some stuff if we can't afford a new refinery and we dont't have any refinery's anymore
		}

		// Build the weapen factory
		if (!StructureWasBuild && (NumbWarfactorys == 0 || (NumbWarfactorys < Rules->WarRatio && NumbWarfactorys < Rules->WarLimit))){
			cost = p::uspool->getStructureTypeByName("WEAP")->getCost();
			if (Player->getMoney() > cost){
				if (BuildStructure (Player, PlayerNumb, "WEAP", ConstYardPos))
					Player->changeMoney(-1 * cost);
				NumbWarfactorys++;
				StructureWasBuild = true;
			}
		}

		// build power plant (if needed)
		//printf ("Power = %i, PowerUsed = %i, PowerSurplus = %i\n", Player->getPower(), Player->getPowerUsed(), Rules->PowerSurplus);
		if (!StructureWasBuild && ((signed)(Player->getPower() - Rules->PowerSurplus) < (signed)Player->getPowerUsed()))
		{
			cost = p::uspool->getStructureTypeByName("APWR")->getCost();
			if (Player->getMoney() > cost)
			{
				if (NumbOfPowerPlants > 0)
				{
					if (BuildStructure (Player, PlayerNumb, "APWR", ConstYardPos))
						Player->changeMoney(-1 * cost);
				}else{
					if (BuildStructure (Player, PlayerNumb, "POWR", ConstYardPos))
						Player->changeMoney(-1 * cost);
				}
				StructureWasBuild = true;
			}
		}

		// Build the barracks
		if (!StructureWasBuild && (NumbBarracks == 0 )){
			cost = p::uspool->getStructureTypeByName("BARR")->getCost();
			if (Player->getMoney() > cost){
				if (BuildStructure (Player, PlayerNumb, "BARR", ConstYardPos))
					Player->changeMoney(-1 * cost);
				NumbBarracks++;
				StructureWasBuild = true;
			}
		}

		// Build tesla coils
		if (!StructureWasBuild && (NumbTeslaCoils < NumbStructures * Rules->TeslaRatio && NumbTeslaCoils < Rules->TeslaLimit)){
			cost = p::uspool->getStructureTypeByName("TSLA")->getCost();
			if (Player->getMoney() > cost){
				if (BuildStructure (Player, PlayerNumb, "TSLA", ConstYardPos))
					Player->changeMoney(-1 * cost);
				StructureWasBuild = true;
			}
		}
	}

	if (NumbWarfactorys > 0){
		if (NumbOfOreTrucks < 1 || (NumbOfOreTrucks <3 && NumbTanks > 5)){
			// Get the structure type so we can see what the cost are for this structure
			UnitType* type = p::uspool->getUnitTypeByName("HARV");
			cost = type->getCost();
			if (Player->getMoney() > cost){
				if (p::dispatcher->unitSpawn(type, PlayerNumb))
					Player->changeMoney(-1 * cost);
			}else if (NumbOreRefs > 0 && NumbOfOreTrucks > 0)
				return;
		}else{
			// Build tanks
			// Get the structure type so we can see what the cost are for this structure
			// UnitType* type = p::uspool->getUnitTypeByName("1TNK"); // allied tank
			UnitType* type = p::uspool->getUnitTypeByName("3TNK");	// sovejet tank
			cost = type->getCost();
			if (Player->getMoney() > cost){
				if (p::dispatcher->unitSpawn(type, PlayerNumb)){
					Player->changeMoney(-1 * cost);
				}
			}
		}
	}

	// Build soldier
	if (NumbBarracks > 0 && ((unsigned)Player->getMoney() > Rules->InfantryReserve || (unsigned) (NumbStructures * Rules->InfantryBaseMult) > NumbOfInfantry)){
		UnitType* type;
		switch (UnitBuildMultiplexer[PlayerNumb]){
			case 0:
				// Rifle Infantry, cost 100
				type = p::uspool->getUnitTypeByName("E1");
				cost = type->getCost();
				if (Player->getMoney() > cost){
					if (p::dispatcher->unitSpawn(type, PlayerNumb))
						Player->changeMoney(-1 * cost);
				}
				break;
			case 1:
				// Grenadier, cost 160
				type = p::uspool->getUnitTypeByName("E2");
				cost = type->getCost();
				if (Player->getMoney() > cost){
					if (p::dispatcher->unitSpawn(type, PlayerNumb))
						Player->changeMoney(-1 * cost);
				}
				break;
			case 2:
				// Rocket Soldier, cost 300
				type = p::uspool->getUnitTypeByName("E3");
				cost = type->getCost();
				if (Player->getMoney() > cost){
					if (p::dispatcher->unitSpawn(type, PlayerNumb))
						Player->changeMoney(-1 * cost);
				}
				break;
			case 3:
				// Flamethrower, cost 300
				type = p::uspool->getUnitTypeByName("E4");
				cost = type->getCost();
				if (Player->getMoney() > cost){
					if (p::dispatcher->unitSpawn(type, PlayerNumb))
						Player->changeMoney(-1 * cost);
				}
				break;
		}
		if (UnitBuildMultiplexer[PlayerNumb] < 3)
			UnitBuildMultiplexer[PlayerNumb]++;
		else
			UnitBuildMultiplexer[PlayerNumb] = 0;
	}

}
/**
 *	Returns a enemy **unit** if one is in range of our **unit**
 */
Unit* Ai::EnemyUnitInRange (int MyPlayerNumb, Unit* MyUnit, int AttackRange )
{
Player                  *EnemyPlayer,
                        *MyPlayer;
int                     EnemyNumbUnits;
std::vector<Unit*>      Enemyunitpool;
Unit*                   EnemyUnit;

	if (MyUnit == NULL)
		return NULL;

	if (!MyUnit->canAttack())
		return NULL;

	#if 1
	if (AttackRange == -1)
		AttackRange = MyUnit->getType()->getWeapon()->getRange();    //  MyUnit->getType()->getSight();
	#else
	if (AttackRange == -1)
		AttackRange = MyUnit->getType()->getSight();

	#endif

	for (int i = 0; i < this->NumbPlayers; i++){
		// Don't find my own units
		if (MyPlayerNumb == i)
			continue;

		EnemyPlayer = p::ppool->getPlayer(i);

		MyPlayer = p::ppool->getPlayer(MyPlayerNumb);

		// Check to see if these are both ai players and if ai players are allied
		if (!MyPlayer->isLPlayer() && !EnemyPlayer->isLPlayer() && Rules->AlwaysAlly)
			continue;

		// Check if the Enemy player is a allie ;)
		if (EnemyPlayer->isAllied(MyPlayer))
			continue;

		EnemyNumbUnits = EnemyPlayer->getNumUnits();

		Enemyunitpool = EnemyPlayer->getUnits();

		// For each unit from this player
		for (int UnitNumb = 0; UnitNumb < EnemyNumbUnits; UnitNumb++){
			EnemyUnit = Enemyunitpool[UnitNumb];
			int distance = MyUnit->getDist(EnemyUnit->getPos());
			if (distance <= AttackRange && EnemyPlayer->getSide() != PS_NEUTRAL){
				return EnemyUnit;
			}

			// Make it easyer to attack harvesters
			if (EnemyUnit->is ("HARV") && MyPlayerNumb != this->HumanPlayerNumb){
				if (distance <= 4*AttackRange && EnemyPlayer->getSide() != PS_NEUTRAL){
					return EnemyUnit;
				}
			}
		}
	}
	return NULL;
}

/**
 * Returns a enemy **unit** if one is in range of our **structure**
 */
Unit* Ai::EnemyUnitInRange (int MyPlayerNumb, Structure* MyStructure, int AttackRange )
{
Player              *EnemyPlayer, *MyPlayer;
int                 EnemyNumbUnits;
std::vector<Unit*>	Enemyunitpool;
Unit*               EnemyUnit;

	if (MyStructure == NULL)
		return NULL;

	if (!MyStructure->canAttack())
		return NULL;

	if (AttackRange == -1)
		AttackRange = MyStructure->getType()->getWeapon(true)->getRange();

	for (int i = 0; i < this->NumbPlayers; i++){

		// Don't defend against my own units
		if (MyPlayerNumb == i)
			continue;

		EnemyPlayer = p::ppool->getPlayer(i);

		MyPlayer = p::ppool->getPlayer(MyPlayerNumb);

		// Check to see if these are both ai players and if ai players are allied
		if (!MyPlayer->isLPlayer() && !EnemyPlayer->isLPlayer() && Rules->AlwaysAlly)
			continue;

		// Check if the Enemy player is a allie ;)
		if (EnemyPlayer->isAllied(MyPlayer))
			continue;

		EnemyNumbUnits = EnemyPlayer->getNumUnits();

		Enemyunitpool = EnemyPlayer->getUnits();

		// For each unit from this player
		for (int UnitNumb = 0; UnitNumb < EnemyNumbUnits; UnitNumb++){
			EnemyUnit = Enemyunitpool[UnitNumb];
//			int distance = MyStructure->getDist(EnemyUnit->getPos());
			int distance = EnemyUnit->getDist(MyStructure->getPos());
			if (distance <= AttackRange  && EnemyPlayer->getSide() != PS_NEUTRAL){
				return EnemyUnit;
			}

		}
	}
	return NULL;
}

/**
 * Returns a enemy **structure** if one is in range of our **unit**
 */
Structure* Ai::EnemyStructureInRange (int MyPlayerNumb, Unit* MyUnit, int AttackRange )
{
	Player*		EnemyPlayer;
	Player*		MyPlayer;
	int         EnemyNumbStructures;
	std::vector<Structure*>	Enemystructurepool;
	Structure*  EnemyStructure;

	
	if (MyUnit == NULL)
		return NULL;

    if (!MyUnit->canAttack())
        return NULL;

    if (AttackRange == -1)
        AttackRange = MyUnit->getType()->getWeapon()->getRange();  //MyUnit->getType()->getSight();

    for (int i = 0; i < this->NumbPlayers; i++){
        // Don't find my own units
        if (MyPlayerNumb == i)
            continue;

        EnemyPlayer = p::ppool->getPlayer(i);

        MyPlayer = p::ppool->getPlayer(MyPlayerNumb);

	// Check to see if these are both ai players and if ai players are allied
	if (!MyPlayer->isLPlayer() && !EnemyPlayer->isLPlayer() && Rules->AlwaysAlly)
		continue;


        // Check if the Enemy player is a allie ;)
        if (EnemyPlayer->isAllied(MyPlayer))
            continue;

        EnemyNumbStructures	= EnemyPlayer->getNumStructs();
        Enemystructurepool	= EnemyPlayer->getStructures();

        // For each unit from this player
        for (int StructureNumb = 0; StructureNumb < EnemyNumbStructures; StructureNumb++){
            EnemyStructure = Enemystructurepool[StructureNumb];
            int distance = MyUnit->getDist(EnemyStructure->getPos());
            if (distance <= AttackRange && EnemyPlayer->getSide() != PS_NEUTRAL){
                return EnemyStructure;
            }
        }
    }
    return NULL;
}
/**
 * Assign tasks to the units
 */
void Ai::Harvest (Player *Player, int PlayerNumb)
{
int                         NumbUnits;
std::vector<Unit*>          unitpool;
Unit                        *Unit;

	// don't try to control for the human player
	if (PlayerNumb == this->HumanPlayerNumb){
		return;
	}

	// Handle unit vars
	NumbUnits		= Player->getNumUnits();
	unitpool		= Player->getUnits();


	for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++){
		Unit = unitpool[UnitNumb];
		if (Unit->is("HARV")){
			if (!Unit->IsHarvesting()){
				Unit->Harvest(0, NULL);
			}
		}
	}
}


/**
 * For now the ai assumes that all non local players are computer players
 * This means no support for multiplayer with computer players mix.
 */
void Ai::patrolAndAttack (Player *Player, int PlayerNumb)
{
    int NumbUnits;
    int NumbStructures;
    int lPlayerNumbStructures;
    int lPlayerNumbUnits;

    std::vector<Structure*> structurepool, lPlayerStructurePool;
    std::vector<Unit*> unitpool, lPlayerUnitPool;
    Unit *Unit;
    UnitOrStructure* target = NULL;
    Uint16 RndStr;
    bool PreferLastStructures = false;

	// don't try to control for the human player
	if (PlayerNumb == this->HumanPlayerNumb){
		return;
	}

	PreferLastStructures	= (bool) ((double)2*rand()/(RAND_MAX));

	// Handle unit vars
	NumbUnits		= Player->getNumUnits();
	unitpool		= Player->getUnits();

	// Handle structure vars
	structurepool		= Player->getStructures();
	NumbStructures		= Player->getNumStructs();

	// Handle lPlayer vars
	lPlayerStructurePool	= p::ppool->getLPlayer()->getStructures();
	lPlayerNumbStructures	= p::ppool->getLPlayer()->getNumStructs();
	lPlayerUnitPool		= p::ppool->getLPlayer()->getUnits();
	lPlayerNumbUnits	= p::ppool->getLPlayer()->getNumUnits();

	// First find a good target
	if (lPlayerNumbStructures > 0){
		RndStr = (int) ((double)lPlayerNumbStructures*rand()/(RAND_MAX));

		if (PreferLastStructures){
			if (RndStr < lPlayerNumbStructures/2){
				RndStr = lPlayerNumbStructures - RndStr;
			}
		}

		if (RndStr >= lPlayerNumbStructures)
			RndStr = lPlayerNumbStructures -1;

//		printf ("%s line %i: Random structure numb = %i, Numb strucutres = %i, preferlast = %i\n", __FILE__, __LINE__, RndStr, lPlayerNumbStructures, PreferLastStructures);

		if (lPlayerStructurePool[RndStr]->isAlive()){
			target = lPlayerStructurePool[RndStr];
		}else{
			for (int str =0; str < lPlayerNumbStructures; str++){
				if (lPlayerStructurePool[str]->isAlive()){
					target = lPlayerStructurePool[str];
					break;
				}
			}
		}
	} else {
		for (int un =0; un < lPlayerNumbUnits; un++){
			if (lPlayerUnitPool[un]->isAlive()){
				target = lPlayerUnitPool[un];
				break;
			}
		}
	}

	if ((lPlayerNumbStructures > 0 || lPlayerNumbUnits > 0) && NumbUnits > 0){

		for (int UnitNumb = 0; UnitNumb < NumbUnits; UnitNumb++){
			Unit = unitpool[UnitNumb];
			if (!Unit->canAttack())
				continue;
			if (Unit->AI_Mission == 1 || Unit->AI_Mission == 3){
				if ( Unit->is ("HARV") ){
					logger->error ("%s line %i: !!!!!!!!!!Error harvesters can't attack\n", __FILE__, __LINE__);
				}else{

					Unit->AI_Mission		= 3;
					player_targets[PlayerNumb]	= target;

					// Make all our units attack the same target.
					Unit->attack(target);
				}
			}
		}
	}
}

// AI destructor






//
//	Actually this function attacks enemy units and structures in range :)
//


// Send all units to defend if our base is under attack


// Help a computer player unit under attack


