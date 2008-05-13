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
#define AI_MISSION_ATTACK_BASE		3	// Attack the base of the human user

class Player;
class Structure;
class AiRules;

class Ai {
public:
    Ai();
    ~Ai();
    void SetDifficulty(int Diff);
    void DefendUnits(Player* Player, int PlayerNumb);
    void DefendComputerPlayerBaseUnderAttack(Player * Player, int PlayerNumb, UnitOrStructure * Enimy,
       Structure * StructureUnderAttack);
    void DefendComputerPlayerUnitUnderAttack(Player * Player, int PlayerNumb, UnitOrStructure * Enimy, Unit * UnitUnderAttack);
    void handle();

private:
    std::vector < Uint32 > LastHarvestTickCount;
    std::vector < Uint32 > LastDefendTickCount;
    std::vector < Uint32 > LastAttackTickCount;
    std::vector < Uint32 > LastBuildTickCount;
    std::vector < Uint32 > LastGuideGatherTickCount;
    int NumbPlayers;
    int HumanPlayerNumb;
    int Difficulty;
    std::vector < Uint32 > LastGuideTickCount;
    std::vector < Uint32 > RetryDeploy;
    Uint32 LplayerBasePos;
    std::vector < Uint16 > UnitBuildMultiplexer;
    bool guide;
    AiRules * Rules;
    std::vector < UnitOrStructure * > player_targets;

    // Uint16 getDist(Uint16 pos1, Uint16 pos2);
    void guideAttack(Player * Player, int PlayerNumb);
    Uint32 getDist(Uint32 pos1, Uint32 pos2);
    bool CanBuildAt(Uint8 PlayerNumb, const char * structname, Uint32 pos);
    bool BuildStructure(Player * Player, int PlayerNumb, const char * structname, Uint32 ConstYardPos);
    unsigned int FindClosesedTiberium(Unit * Unit);
    void DefendStructures(Player * Player, int PlayerNumb);
    void Build(Player * Player, int PlayerNumb);
    Unit * EnemyUnitInRange(int MyPlayerNumb, Unit * MyUnit, int AttackRange = -1);
    Unit * EnemyUnitInRange(int MyPlayerNumb, Structure * MyStructure, int AttackRange = -1);
    Structure * EnemyStructureInRange(int MyPlayerNumb, Unit * MyUnit, int AttackRange = -1);
    void Harvest(Player * Player, int PlayerNumb);

    void patrolAndAttack(Player * Player, int PlayerNumb);

};

#endif //AI_H
