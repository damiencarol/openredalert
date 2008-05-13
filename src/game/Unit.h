#ifndef UNIT_H
#define UNIT_H

#define LOOPEND_TURN

#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <stack>
#include <time.h>

#include "SDL/SDL_types.h"

#include "game/UnitOrStructure.h"
#include "game/UnitType.h"
#include "game/TalkbackType.h"
#include "UnitOrStructure.h"
#include "UnitType.h"
//#include "InfantryGroup.h"
#include "Weapon.h"
#include "Structure.h"
#include "TalkbackType.h"

struct L2Overlay;
class WalkAnimEvent;
class MoveAnimEvent;
class URepairEvent;
class UHarvestEvent;
class UAttackAnimEvent;
class TurnAnimEvent;
class ActionEventQueue;
class INIFile;
class WeaponsPool;
class InfantryGroup;

using std::string;

class Unit : public UnitOrStructure {
public:
	friend class UnitAnimEvent;
	friend class MoveAnimEvent;
	friend class UAttackAnimEvent;
	friend class TurnAnimEvent;
	friend class WalkAnimEvent;
	friend class UHarvestEvent;
	friend class URepairEvent;
	Unit(UnitType *type, Uint16 cellpos, Uint8 subpos, InfantryGroup *group,
			Uint8 owner, Uint16 rhealth, Uint8 facing, Uint8 action,
			string trigger_name = "None");
	~Unit();
	Uint8 getImageNums(Uint32 **inums, Sint8 **xoffsets, Sint8 **yoffsets);
	InfantryGroup * getInfantryGroup();
	void setInfantryGroup(InfantryGroup *ig);
	Uint32 getImageNum(Uint8 layer) const;
	Uint16 getNumbImages(Uint8 layer);
	void setImageNum(Uint32 num, Uint8 layer);
	Sint8 getXoffset() const; // return xoffset-type->getOffset();
	Sint8 getYoffset() const; // return yoffset-type->getOffset();
	void setXoffset(Sint8 xo);
	void setYoffset(Sint8 yo);
	UnitType * getType();
	
	
	//Uint8 getSight() const;
	
	
	Uint16 getPos() const;
	Uint16 getBPos(Uint16 pos) const;
	Uint16 getSubpos() const;
	Uint32 getNum() const;
	void setUnitnum(Uint32 unum);
	Uint16 getHealth() const;
	void ChangeHealth(Sint16 amount);

	void move(Uint16 dest);
	void move(Uint16 dest, bool stop);
	bool IsMoving(void);
	bool IsAttacking(void);
	bool canAttack(bool primary = true);
	bool UnderAttack(void);
	void attack(UnitOrStructure* target);
	void attack(UnitOrStructure* target, bool stop);
	void turn(Uint8 facing, Uint8 layer);
	void stop();

	Uint8 getOwner() const;
	void setOwner(Uint8 newowner);
	void remove();
	void applyDamage(Sint16 amount, Weapon* weap, UnitOrStructure* attacker);
	void updateDamaged();

	char* getTName() const;
	bool IsHarvester();
	bool IsHarvesting();
	unsigned int FindTiberium(void);
	void Harvest(Uint32 pos, Structure *Struct);
	bool Repair(Structure *str);

	void doRandTalk(TalkbackType ttype);
	bool deploy(void);
	bool canDeploy();
	bool checkDeployTarget(Uint32 pos);
	Uint32 calcDeployPos() const;
	Uint32 calcDeployPos(Uint32 pos) const;
	Uint32 getExitCell() const;
	double getRatio() const;
	Uint16 getDist(Uint16 pos);
	Uint16 getTargetCell();

	Structure *GetBaseRefinery();
	void SetBaseRefinery(Structure *Bref);

	void AddResource(Uint8 ResourceType);

	void EmptyResources(void);

	Uint8 GetNumResources(void);

	bool GetResourceType(Uint8 Numb, Uint8 *Type);

	bool is(const char *Name);
	Uint32 GetFixStr(void);

	Uint16 GetFixPos(void);

	bool IsAirBound(void);

	bool IsWaterBound(void);

	// The folowing var's are used by the AI
	Uint8 AI_Mission; // This tells the AI if this unit has a mission and what mission it is...
	Uint32 AI_GuardLocation; // The location whitch needs garding

	string getTriggerName(void);

	void operation1();

private:
	Sint8 yoffset;
	std::string TriggerName;
	UnitType *type;
	Uint32 unitnum;
	Uint32 fix_str_num; // Then structure number of fix (for repairing this unit)
	Uint16 fix_str_pos;
	Uint16 *imagenumbers;
	Uint16 cellpos; Uint16 health; Uint16 palettenum;
	Uint8 owner;
	Uint8 subpos;
	Sint8 xoffset;
	bool deployed;
	double ratio;

	Uint32 LastDamageTick;

	L2Overlay* l2o;
	std::multimap<Uint16, L2Overlay*>::iterator l2entry;

	InfantryGroup * infgrp;

	MoveAnimEvent* moveanim;
	URepairEvent* repairanim;
	UAttackAnimEvent* attackanim;
	UHarvestEvent* harvestanim;
	WalkAnimEvent* walkanim;
	TurnAnimEvent* turnanim1; TurnAnimEvent* turnanim2;

	Structure *BaseRefinery;

	// Resource var's for harvesters
	Uint8 NumbResources; // Number of pieces resource in harvester
	Uint8 ResourceTypes[10]; // Resource type in harvester

	Uint8 Command; // The command this unit should execute

	Uint32 mapwidth;
	Uint32 mapheight;

	Uint32 LastAttackTick;
};

#endif //UNIT_H
