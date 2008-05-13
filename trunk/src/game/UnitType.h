#ifndef UNITTYPE_H
#define UNITTYPE_H

#include <vector>

#include "SDL/SDL_types.h"

#include "UnitOrStructureType.h"
#include "TalkbackType.h"

#define LOOPEND_TURN

class Weapon;
class Talkback;
class StructureType;
class WeaponsPool;
class INIFile;
struct L2Overlay;

using std::vector;

/**
 * Type of unit with stats
 */
class UnitType : public UnitOrStructureType 
{
public:
    UnitType(const char *typeName, INIFile* unitini);
    ~UnitType();
    
    Uint32 *getSHPNums() ;
    Uint8 getNumLayers() const ;
    bool isInfantry() const ;
    Uint8 getType() const ;
    Uint16 *getSHPTNum();
    const char* getTName() const;
    const char* getName() const;

    vector<char*> getOwners() const;
    Uint8 getOffset() const ;

    Uint8 getROT() const ;
    Sint8 getMoveMod() const ;
    Uint8 getTurnMod() const ;

    Uint8 getTurnspeed() const ;

    armour_t getArmour() const ;
#ifdef LOOPEND_TURN
    animinfo_t getAnimInfo() const {
        return animinfo;
    }
#endif
    const char* getRandTalk(TalkbackType type) const;
    
    /** Returns the primary Weapon of this type of Unit */
    Weapon* getWeapon() const;
    /** Returns the primary Weapon of this type of Unit if 
     * primary is true else returns the secondary Weapon */
    Weapon* getWeapon(bool primary) const;
    
    bool isWall() const;
    bool canDeploy() const;
    const char* getDeployTarget() const;
    StructureType* getDeployType() const;
    Uint8 getBuildlevel() const;

    /** what colour pip should be displayed for this unit when being carried*/
    Uint8 getPipColour() const;
    Uint8 getMaxPassengers() const;
    vector<Uint8> getPassengerAllow() const;
    vector<UnitType*> getSpecificTypeAllow() const;
    Uint8 getPQueue() const;
    bool isStructure() const;
	bool isDoubleOwned();

private:
    Uint32 *shpnums;
	Uint16 *shptnum;
    /** In multi player both sides can use this unit */
    bool doubleowned;
    Uint8 numlayers;
    Uint8 turnspeed;
    Uint8 turnmod;
    Uint8 offset;
    Uint8 pipcolour;
    Uint8 buildlevel;
    Uint8 unittype;
    Sint8 movemod;

    char* tname;
    char* name;
    vector<char*> owners;

    /** Talkback related members*/
    Talkback* talkback;

    bool is_infantry; 
    bool deployable;
    char* deploytarget;
    
    /** this is used to check the unit can deploy */
    StructureType* deploytype;
    
    /** max number of passenger */
    Uint8 maxpassengers;
    
    /** matches the unit's type value specified in units.ini*/
    vector<Uint8> passengerAllow;
    
    /** matches the unit's type name.*/
    vector<UnitType*> specificTypeAllow;
};

#endif //UNITTYPE_H
