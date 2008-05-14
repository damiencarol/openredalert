#ifndef UNITORSTRUCTURETYPE_H
#define UNITORSTRUCTURETYPE_H

#include <vector>

#include "SDL/SDL_types.h"

#include "armour_t.h"
#include "animinfo_t.h"

class Weapon;

using std::vector;

/**
 * UnitOrStructureType is used when you can't or don't need to know whether you are dealing with a unit type or a structure
 * type.  Note that this class is abstract, it is only used for performing casts.
 */
class UnitOrStructureType 
{
public :
    UnitOrStructureType();
    virtual ~UnitOrStructureType();

    /** Turn speed is measured in arbitrary units */
    virtual Uint8 getTurnspeed() const = 0;

    /** @brief Returns a number corresponding to the type's armour class. See common.h for the enum definition */
    virtual armour_t getArmour() const = 0;

    /**
     * @brief Returns number of layers to render, 1 or 2.  The only TD
     * structure that returns 2 is the weapons factory.  Units with
     * turrets (tanks, humvee, buggy, missile launchers) return 2.
     */
    virtual Uint8 getNumLayers() const = 0;

    /**
     * Units and structures can have at most two weapons. Currently any secondary weapons are ignored.
     * @todo Write a version that accepts an armour type and returns the weapon that'll cause the most damage.
     */
    virtual Weapon * getWeapon(bool primary) const = 0;

    /** Only applicable to units.  StructureType always returns false. */
    virtual bool isInfantry() const = 0;

    /** Only applicable to structures.  UnitType always returns false. */
    virtual bool isWall() const = 0;

    /** Only applicable to units.  StructureType always returns zero. */
    virtual Uint8 getOffset() const = 0;

    /** @returns the internal name, e.g. E1 */
    virtual const char * getTName() const = 0;

    /** @returns the external name, e.g. Minigunner */
    virtual const char * getName() const = 0;

    /** @returns the names of the sides that can build this */
    virtual std::vector < char * > getOwners() const = 0;

    /** @returns whether the type is valid or not (loaded fully) */
    virtual bool isValid() const;

    /** @returns which production queue the type is for */
    virtual Uint8 getPQueue() const = 0;

    /** @returns the production type of this type. */
    Uint8 getPType() const;
    void setPType(Uint8 p);

    /** Calling a virtual function is much faster than a dynamic_cast */
    virtual bool isStructure() const = 0;

    /** Returns the prerequisites. */
    vector <char* > getPrereqs() const;

    /** tech level required to build this [-1 means can't build] (def=-1)*/
    Sint32 getTechLevel() const;

    /** Cost to build object (in credits).*/
    Uint16 getCost() const;

    /** Speed is measured in artitrary units. */
    Uint8 getSpeed() const;

    /**
     * Returns the maximum health for this type. 
     */
    Uint16 getMaxHealth() const;

    /**
     * Sight range, in cells (def=1). 
     */
    Uint8 getSight() const;
    
protected :
    Uint8 sight;
    animinfo_t animinfo;
    armour_t armour;
    Weapon* secondary_weapon;
    Weapon* primary_weapon;
    Uint16 maxhealth;
    Uint8 speed;
    Uint16 cost;
    Sint32 techLevel;
    vector<char*> prereqs;
    Uint8 ptype;
    bool valid;
};

#endif //UNITORSTRUCTURETYPE_H
