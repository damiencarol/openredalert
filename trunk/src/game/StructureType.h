// StructureType.h
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

#ifndef STRUCTURETYPE_H
#define STRUCTURETYPE_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"

#include "UnitOrStructureType.h"
#include "PowerInfo.h"

#include "armour_t.h"
#include "animinfo_t.h"

class INIFile;
//class UnitType;
class Weapon;

using std::string;
using std::vector;

/**
 * Base class for structure in game
 */
class StructureType : public UnitOrStructureType
{
public:
    /** Constructor */
    StructureType(const string& typeName, INIFile* structini, INIFile* artini,
                  const string& thext);
    ~StructureType();

	Uint16 * getSHPNums();

	Uint16 * getSHPTNum();

	vector<string> getDeployWith() const;

	vector<string> getOwners() const;

	Uint8 getNumLayers() const;

	Uint16 getMakeImg() const;

	/** Only applicable to structures.  UnitType always returns false. */	    
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

	armor_t getArmor() const;

	animinfo_t getAnimInfo() const;

	PowerInfo getPowerInfo() const;

	bool isPowered();

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

	Uint32 getAdjacent() const;

private:
    StructureType(const StructureType& orig);

	/** Index in the ImagePool of the first MAKE image */
	Uint16 makeimg;
	Uint16 blckoff;
	Sint8 xoffset;
	Sint8 yoffset;
	Uint8 turnspeed;
	Uint8 xsize;
	Uint8 ysize;
	Uint8 numshps;
	Uint8 numwalllevels;
	Uint8 defaultface;
	Uint8 buildlevel;
	Uint8 * blocked;

	vector<string> owners;
	vector<string> deploywith;
	PowerInfo powerinfo;

	bool is_wall;
	bool turret;
	bool primarysettable;
	bool charges;
	bool WaterBound;
	bool AirBoundUnits;
	Uint16* shptnum;
	Uint16* shpnums;

	/** Adjacent = distance allowed to place from other buildings (def=1) */
	Uint32 adjacent;
};

#endif
