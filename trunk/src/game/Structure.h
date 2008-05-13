#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <string>

#include "SDL/SDL_types.h"

#include "UnitOrStructure.h"
#include "UnitOrStructureType.h"
#include "StructureType.h"
#include "Weapon.h"
#include "UnitType.h"
#include "Unit.h"

using std::string;

class BuildingAnimEvent;
class BAttackAnimEvent;
class BRepairUnitAnimEvent;
class DoorAnimEvent;

class Structure : public UnitOrStructure
{
public:
    friend class BuildingAnimEvent;
    friend class BAttackAnimEvent;
    friend class BRepairUnitAnimEvent;
    friend class DoorAnimEvent;

    Structure(StructureType *type, Uint16 cellpos, Uint8 owner,
            Uint16 rhealth, Uint8 facing, string trigger_name = "None");
    ~Structure();
    
    Uint8 getImageNums(Uint32 **inums, Sint8 **xoffsets, Sint8 **yoffsets);
    Uint16* getImageNums() const {
        return imagenumbers;
    }
    void changeImage(Uint8 layer, Sint16 imagechange) ;
	/** Return the actual current image number (without side color etc)*/
	Uint32 getRealImageNum ( Uint8 layer );
    Uint32 getImageNum(Uint8 layer) const ;
	Uint16 getNumbImages( Uint8 layer );
    void setImageNum(Uint32 num, Uint8 layer);
    StructureType* getType() ;
    void setStructnum(Uint32 stn) ;
    Uint32 getNum() const ;
    Uint16 getPos() const ;
    Uint16 getBPos(Uint16 curpos) const;
    Uint16 getFreePos(Uint8* subpos, bool findsubpos);
    void remove();
    Uint16 getSubpos() const ;
    void applyDamage(Sint16 amount, Weapon* weap, UnitOrStructure* attacker);
    void runAnim(Uint32 mode);
    void runSecAnim(Uint32 param, bool extraParam = false);
    void stopAnim();
    void stop();
    Uint8 getOwner() const ;
    void setOwner(Uint8 newowner) ;
    bool canAttack() const ;

    bool IsAttacking (void);
    bool IsBuilding (void)  ;

    void attack(UnitOrStructure* target);


    Uint16 getHealth() const ;
    void ChangeHealth (Sint16 amount);

    Sint8 getXoffset() const ;
    Sint8 getYoffset() const ;

    bool isPowered ();

    bool isRefinery (void) ;

	bool CreateUnitAnimation (UnitType* UnType, Uint8 owner);

    bool isWall() const ;
    double getRatio() const ;
    bool isPrimary() const ;
    void setPrimary(bool pri) ;
	bool RepairUnint (Unit *Un);

	Uint32 getExitCell() const;
	void resetLoadState(bool runsec, Uint32 param);
	bool checkdamage();
	Uint16 getTargetCell() const;

	bool is(string Name) ;

	void sell();
	void repair();
	bool isRepairing();
	void repairDone(void);

	bool underAttack(void);
	Uint16 getAttackerPosition(void);

	string getTriggerName(void) ;
	
	static bool valid_pos(StructureType *type, Uint8 PlayerNr, Uint16 pos, Uint8*);
	static bool valid_possubpos(StructureType *type, Uint8 PlayerNr, Uint16 pos, Uint8* subpos);
	
private:
	string TriggerName;
	StructureType *type;
	Uint32 structnum;
	Uint16 *imagenumbers;

	Uint16 cellpos;
	Uint16 bcellpos;
	Uint16 health;

	Uint8 owner;

	bool damaged;
	bool animating;
	bool usemakeimgs;
	bool exploding;
	bool primary;

    /** health/maxhealth */
	double ratio;
    /** If we stop a loop animation because of a repair animation
     we use this var to start the loop animation again */
	Uint8 backup_anim_mode;

	bool retry_sell;
    bool retry_repair;
    bool repairing;

	UnitType*	CreateUnitType;
	Uint8		CreateUnitOwner;
	Uint32		UnitToRepairPos;
	Uint32		LastAttackTick;

	BuildingAnimEvent* buildAnim;
	BAttackAnimEvent* attackAnim;
	BRepairUnitAnimEvent* repairunitAnim;
};

#endif //STRUCTURE_H
