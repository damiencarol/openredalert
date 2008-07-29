// Ai.h
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

#ifndef AI_H
#define AI_H

#include <vector>

#include "SDL/SDL_types.h"
#include "UnitOrStructure.h"
#include "Unit.h"

using std::vector;


#define DIFFICUTY_EASY		1
#define DIFFICUTY_MEDIUM	2
#define DIFFICUTY_HARD		3
#define DIFFICUTY_VERYHARD	4


#define AI_MISSION_NONE			1	// No mission
#define AI_MISSION_GUARD		2	// Guard a area
#define AI_MISSION_ATTACK_BASE	3	// Attack the base of the human user

class Player;
class Structure;
class AiRules;

/**
 * Artificial Intelligence that manage Units of a Player during game
 */
class Ai 
{
public:
    Ai();
    ~Ai();
    
    void SetDifficulty(int Diff);
    void DefendUnits(Player* Player, int PlayerNumb);
    void DefendComputerPlayerBaseUnderAttack(Player* Player, int PlayerNumb, UnitOrStructure * Enimy, Structure* StructureUnderAttack);
    void DefendComputerPlayerUnitUnderAttack(Player* Player, int PlayerNumb, UnitOrStructure * Enimy, Unit* UnitUnderAttack);
    void handle();

private:
    vector<Uint32> LastHarvestTickCount;
    vector<Uint32> LastDefendTickCount;
    vector<Uint32> LastAttackTickCount;
    vector<Uint32> LastBuildTickCount;
    vector<Uint32> LastGuideGatherTickCount;
    int NumbPlayers;
    int HumanPlayerNumb;
    int Difficulty;
    vector<Uint32> LastGuideTickCount;
    vector<Uint32> RetryDeploy;
    Uint32 LplayerBasePos;
    vector<Uint16> UnitBuildMultiplexer;
    bool guide;
    AiRules * Rules;
    vector<UnitOrStructure*> player_targets;

    // Uint16 getDist(Uint16 pos1, Uint16 pos2);
    void guideAttack(Player * Player, int PlayerNumb);
    Uint32 getDist(Uint32 pos1, Uint32 pos2);
    bool CanBuildAt(Uint8 PlayerNumb, const char * structname, Uint32 pos);
    bool BuildStructure(Player * Player, int PlayerNumb, const char * structname, Uint32 ConstYardPos);
    unsigned int FindClosesedTiberium(Unit * Unit);
    void DefendStructures(Player * Player, int PlayerNumb);
    void Build(Player * Player, int PlayerNumb);
    Unit* EnemyUnitInRange(int MyPlayerNumb, Unit* MyUnit, int AttackRange = -1);
    Unit* EnemyUnitInRange(int MyPlayerNumb, Structure* MyStructure, int AttackRange = -1);
    Structure * EnemyStructureInRange(int MyPlayerNumb, Unit * MyUnit, int AttackRange = -1);
    void Harvest(Player * Player, int PlayerNumb);

    void patrolAndAttack(Player * Player, int PlayerNumb);
};

#endif //AI_H
