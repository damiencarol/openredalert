// UnitOrStructureType.h
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
 * type.
 *
 * @note This class is abstract, it is used for performing casts.
 */
class UnitOrStructureType
{
public:
	UnitOrStructureType();
	virtual ~UnitOrStructureType();

	/** Turn speed is measured in arbitrary units */
	virtual Uint8 getTurnspeed() const = 0;

	/** Returns a number corresponding to the type's armour class. See armour_t.h for the enum definition */
	virtual armor_t getArmor() const = 0;

	/**
	 * Returns number of layers to render, 1 or 2
	 *
	 * The only CnC structure that returns 2 is the weapons factory.
	 * Units with turrets (tanks, humvee, buggy, missile launchers) return 2.
	 */
	virtual Uint8 getNumLayers() const = 0;

	/** Returns the weapon of the Unit or Structure */
	virtual Weapon * getWeapon(bool primary) const = 0;

	/** Only applicable to units.  StructureType always returns false. */
	virtual bool isInfantry() const = 0;

	/** @brief Return true if this Unit or Structure is a Wall */
	virtual bool isWall() const = 0;

	/** Only applicable to units.  StructureType always returns zero. */
	virtual Uint8 getOffset() const = 0;

	/** Returns the internal name, e.g. E1 */
	virtual const char * getTName() const = 0;

	/** Returns the external name, e.g. Minigunner */
	virtual const char * getName() const = 0;

	/** Returns the names of the sides that can build this */
	virtual vector<char *> getOwners() const = 0;

	/** Returns whether the type is valid or not (loaded fully) */
	virtual bool isValid() const;

	/** Returns which production queue the type is for */
	virtual Uint8 getPQueue() const = 0;

	/** Returns the production type of this type. */
	Uint8 getPType() const;
	void setPType(Uint8 p);

	/** Calling a virtual function is much faster than a dynamic_cast */
	virtual bool isStructure() const = 0;

	/** Returns the prerequisites. */
	vector<char*> getPrereqs() const;

	/** Return the technology level required to build this [-1 means can't build] (def=-1)*/
	Sint32 getTechLevel() const;

	/** Cost to build object (in credits).*/
	Uint16 getCost() const;

	/** Speed is measured in artitrary units. */
	Uint8 getSpeed() const;

	/** Returns the maximum health for this type. */
	Uint16 getMaxHealth() const;

	/** Sight range, in cells (def=1). */
	Uint8 getSight() const;

protected:
	/** Sight of the Unit (in Cell) */
	Uint8 sight;
	animinfo_t animinfo;
	/** Armor of the Unit */
	armor_t armour;
	/** Secondary weapon of the Unit */
	Weapon* secondary_weapon;
	/** Primary weapon of the Unit */
	Weapon* primary_weapon;
	Uint16 maxhealth;
	Uint8 speed;
	Uint16 cost;
	/** Technology level required to build this [-1 means can't build] (default = -1) */
	Sint32 techLevel;
	vector<char*> prereqs;
	Uint8 ptype;
	bool valid;
};

#endif //UNITORSTRUCTURETYPE_H
