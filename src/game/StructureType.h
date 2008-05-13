#ifndef STRUCTURETYPE_H
#define STRUCTURETYPE_H

#include <vector>

#include "SDL/SDL_types.h"

#include "include/common.h"
#include "include/weaponspool.h"
#include "UnitOrStructureType.h"
#include "armour_t.h"
#include "Weapon.h"
#include "animinfo_t.h"

class INIFile;
class UnitType;
class WeaponsPool;

class StructureType : public UnitOrStructureType {
public:
    StructureType(const char * typeName, INIFile* structini, INIFile * artini, const char * thext);
    ~StructureType();

    Uint16 * getSHPNums();

    Uint16 * getSHPTNum();

    const char * getTName() const;

    const char * getName() const;

    std::vector < char * > getDeployWith() const;

    std::vector < char * > getOwners() const;

    Uint8 getNumLayers() const;

    Uint16 getMakeImg() const;

    bool isWall() const;

    bool isWaterBound() const;

    bool hasAirBoundUnits() const;

    Uint8 getXsize() const;

    Uint8 getYsize() const;

    Uint8 isBlocked(Uint16 tile) const;

    Sint8 getXoffset() const;

    Sint8 getYoffset() const;

    Uint8 getOffset() const;

    Uint8 getTurnspeed() const;

    /**
     * Surcharge speed with 0 (because it's a building). 
     */
    Uint8 getSpeed() const;

    armour_t getArmour() const;

    animinfo_t getAnimInfo() const;

    powerinfo_t getPowerInfo() const;

    bool isPowered();

    Weapon * getWeapon(bool primary = true) const;

    bool hasTurret() const;

    Uint16 getBlckOff() const;

    bool isInfantry() const;

    Uint8 getNumWallLevels() const;

    Uint8 getDefaultFace() const;

    Uint8 getBuildlevel() const;

    bool primarySettable() const;

    bool Charges();

    Uint8 getPQueue() const;

    bool isStructure() const;

private:
    Uint16 makeimg;
    Uint16 blckoff;
    Sint8 xoffset;
    Sint8 yoffset;
    Uint8 turnspeed;
    Uint8 sight;
    Uint8 xsize;
    Uint8 ysize;
    Uint8 numshps;
    Uint8 numwalllevels;
    Uint8 defaultface;
    Uint8 buildlevel;
    Uint8 * blocked;
    char tname[12];
    char * name;
    std::vector < char * > owners;
    std::vector < char * > deploywith;
    powerinfo_t powerinfo;

    bool is_wall;
    bool turret;
    bool primarysettable;
    bool charges;
    bool WaterBound;
    bool AirBoundUnits;
	Uint16 *shptnum;
    Uint16 * shpnums;
};

#endif
