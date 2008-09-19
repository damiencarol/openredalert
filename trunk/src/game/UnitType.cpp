// UnitType.cpp
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

#include "UnitType.h"

#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_timer.h"

#include "include/common.h"
#include "CnCMap.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "PlayerPool.h"
#include "audio/SoundEngine.h"
#include "Unit.h"
#include "UnitAndStructurePool.h"
#include "ui/Sidebar.h"
#include "video/ImageNotFound.h"
#include "Talkback.h"
#include "video/ImageCache.h"
#include "video/ImageCacheEntry.h"
#include "weaponspool.h"
#include "InfantryGroup.h"
#include "StructureType.h"
#include "GameMode.h"
#include "TalkbackType.h"
#include "Weapon.h"


using std::string;
using std::vector;

namespace pc {
	extern ImageCache* imgcache;
	extern vector<SHPImage*>* imagepool;
}
namespace p {
	extern UnitAndStructurePool* uspool;
	extern WeaponsPool* weappool;
}
extern Logger * logger;

/**
 */
UnitType::UnitType(const char *typeName, INIFile* unitini) :
	UnitOrStructureType(),
	shpnums(0),
	name(0),
	deploytarget(0),
	c4(false)
{
    SHPImage* shpimage = 0;
    Uint32 i;
    string shpname;
#ifdef LOOPEND_TURN
//    char* imagename;
#endif
	Uint32 shpnum;
	Uint32 tmpspeed;

	// Set deploy target to NULL
	deploytarget = 0;

    // Ensure that there is a section in the ini file
    if (unitini->isSection(typeName) == false)
    {
    	// Log it
    	logger->error("%s line %i: Unknown type: %s\n", __FILE__, __LINE__, typeName);

        name = 0;
        shpnums = 0;
        shptnum = 0;
        return;
    }

	tname = cppstrdup(typeName);


	name = unitini->readString(tname, "name");

	char* tmp = unitini->readString(tname, "prerequisites");
	if (0 != tmp)
	{
		prereqs = splitList(tmp, ',');
		delete[] tmp;
	}

	unittype = unitini->readInt(tname, "unittype", 0);
	if (0 == unittype)
	{
		logger->warning("No unit type specified for \"%s\"\n", tname);
	}

	numlayers = unitini->readInt(tname, "layers", 1);

	shpnums = new Uint32[numlayers];
	shptnum = new Uint16[numlayers];

	// get string with this name
	shpname = string(typeName);
	shpname += ".shp";
	// @todo TRY THIS !!!
	try
	{
		shpimage = new SHPImage(shpname.c_str(), -1);
	} catch (ImageNotFound&) {
		logger->error("Image not found: \"%s\"\n", shpname.c_str());
		numlayers = 0;
		return;
	}
	shpnum = static_cast<Uint32>(pc::imagepool->size());
	pc::imagepool->push_back(shpimage);
	shpnum <<= 16;

	for( i = 0; i < numlayers; i++ )
	{
		// get layer offsets from inifile
		//shpnums[i] = pc::imagepool->size();
		shpnums[i] = shpnum;
		shptnum[i] = shpimage->getNumImg();
		shpnum += 32;
	}
	// @todo REFACTOR TO IT !!!
	 /*
	// Load the SHPimage
	Uint32 numSHP = pc::imgcache->loadImage(shpname.c_str());
	for (i = 0; i < numlayers; i++)
	{
		// get layer offsets from inifile
		shpnums[i] = numSHP;
		shptnum[i] = pc::imgcache->getImage(numSHP).NumbImages;
		shpnum += 32;
	}
*/
	is_infantry = false;

	//buildlevel = unitini->readInt(tname, "buildlevel", 99);
	techLevel = (Sint32)(unitini->readInt(tname, "TechLevel", (Uint32)-1));

	tmp = unitini->readString(tname, "owners");
	if (tmp != NULL)
	{
		owners = splitList(tmp, ',');
		delete[] tmp;
	}

	if (unittype == 1)
	{
		is_infantry = true;
	}

	tmpspeed = unitini->readInt(tname, "speed");
	if (is_infantry)
	{
		if (tmpspeed == 0x7fffffff)
		{
			speed = 4; // default for infantry is slower
			movemod = 1;
		}
		else
		{
			speed = (tmpspeed>4) ? 2 : (7-tmpspeed);
			movemod = (tmpspeed>4) ? (tmpspeed-4) : 1;
		}
	}
	else
	{
		if (tmpspeed == 0x7fffffff)
		{
			speed = 2;
			movemod = 1;
		}
		else
		{
			speed = (tmpspeed>4) ? 2 : (7-tmpspeed);
			movemod = (tmpspeed>4) ? (tmpspeed-4) : 1;
		}
	}


	char* talkmode = 0;
	if (is_infantry)
	{
		talkmode = unitini->readString(tname, "talkback", "Generic");
		sight = unitini->readInt(tname, "sight", 3);
	}
	else
	{
		talkmode = unitini->readString(tname, "talkback", "Generic-Vehicle");
		sight = unitini->readInt(tname, "sight", 5);
	}
	talkback = p::uspool->getTalkback(talkmode);
	delete[] talkmode;
	maxhealth = unitini->readInt(tname, "health", 50);
	cost = unitini->readInt(tname, "cost", 0);
	if (0 == cost)
	{
		logger->error("\"%s\" has no cost, setting to 1\n", tname);
		cost = 1;
	}

	// Set the turn speed
	try
	{
		tmpspeed = unitini->readInt(tname, "turnspeed");

		// ok
		turnspeed = (tmpspeed>4)?2:(7-tmpspeed);
		turnmod = (tmpspeed>4)?(tmpspeed-4):1;
	}
	catch(...)
	{
		turnspeed = 2;
		turnmod = 1;
	}

	if (is_infantry)
	{
		//      size = 1;
		offset = 0;
	}
	else
	{
		//size = shpimage->getWidth();
		offset = (shpimage->getWidth()-24)>>1;
	}

	char* downed = unitini->readString(tname, "DoubleOwned");
	doubleowned = false;
	if (downed == 0)
	{
		doubleowned = false;
	}
	else
	{
		//if (downed[0] == 'y' && downed[1] == 'e' && downed[2] == 's')
		if (strcmp((char*)downed, "yes") == 0)
			doubleowned = true;
		delete[] downed;
	}

	// primary weapon
	char* miscnames;
	// Read primary weapon
	miscnames = unitini->readString(tname, "Primary");
	if (miscnames == 0)
	{
		primary_weapon = 0;
	}
	else
	{
		primary_weapon = p::weappool->getWeapon(miscnames);
		delete[] miscnames;
	}
	miscnames = unitini->readString(tname, "Secondary");
	if (miscnames == 0)
	{
		secondary_weapon = NULL;
	}
	else
	{
		secondary_weapon = p::weappool->getWeapon(miscnames);
		delete[] miscnames;
	}
	deploytarget = unitini->readString(tname, "deploysto");
	if (deploytarget != NULL)
	{
		deployable = true;
		deploytype = p::uspool->getStructureTypeByName(deploytarget);
	}
	pipcolour = unitini->readInt(tname, "pipcolour", 0);

	// Read the armor
	char* charArmor = unitini->readString(tname, "Armour");
	if (charArmor == 0)
		armour = AC_none;
	else
	{
		if (string(charArmor) == "none")
			armour = AC_none;
		else if (string(charArmor) == "wood")
			armour = AC_wood;
		else if (string(charArmor) == "light")
			armour = AC_light;
		else if (string(charArmor) == "heavy")
			armour = AC_heavy;
		else if (string(charArmor) == "concrete")
			armour = AC_concrete;
	}
	delete[] charArmor;
	valid = true;

#ifdef LOOPEND_TURN
	animinfo.loopend = unitini->readInt(tname, "loopend", 31);
	animinfo.loopend2 = unitini->readInt(tname, "loopend2", 0);

	animinfo.animspeed = unitini->readInt(tname, "animspeed", 3);
	animinfo.animspeed = abs(animinfo.animspeed);
	animinfo.animspeed = (animinfo.animspeed>1 ? animinfo.animspeed : 2);
	animinfo.animdelay = unitini->readInt(tname, "delay", 0);

	animinfo.animtype = unitini->readInt(tname, "animtype", 0);
	animinfo.sectype = unitini->readInt(tname, "sectype", 0);

	animinfo.dmgoff = unitini->readInt(tname, "dmgoff", ((shptnum[0]-1)>>1));
#endif

	// Read the C4 caract
	if (string(unitini->readString(tname, "C4", "no")) == "yes")
	{
		c4 = true;
	} else {
		c4 = false;
	}

	// Read the Infiltrate caracteristic
	if (string(unitini->readString(tname, "Infiltrate", "no")) == "yes" ||
		string(unitini->readString(tname, "Infiltrate", "no")) == "Yes")
	{
		infiltrate = true;
	} else {
		infiltrate = false;
	}
}

/**
 * Destructor
 */
UnitType::~UnitType()
{
	Uint16 i;
	if (name != NULL)
		delete[] name;
	if (shpnums != NULL)
		delete[] shpnums;
	if (shptnum != NULL)
		delete[] shptnum;
	if (deploytarget != NULL)
		delete[] deploytarget;
	for (i=0; i<owners.size(); ++i)
	{
		if (owners[i] != NULL)
			delete[] owners[i];
	}
	for (i=0; i<prereqs.size(); ++i)
	{
		if (prereqs[i] != NULL)
			delete[] prereqs[i];
	}
}

const char* UnitType::getRandTalk(TalkbackType type) const
{
	// If talkback != 0
	if (talkback != 0)
	{
		return talkback->getRandTalk(type);
	}
	return 0;
}

bool UnitType::isInfantry() const
{
	return is_infantry;
}

bool UnitType::isWall() const
{
	return false;
}

bool UnitType::canDeploy() const
{
	return deployable;
}

bool UnitType::isStructure() const
{
	return false;
}

Uint8 UnitType::getType() const
{
	return unittype;
}

bool UnitType::isDoubleOwned()
{
	if (doubleowned)
	{
		return true;
	}
	return false;
}

Uint32 *UnitType::getSHPNums()
{
	return shpnums;
}

Uint8 UnitType::getNumLayers() const
{
	return numlayers;
}

Uint16* UnitType::getSHPTNum()
{
        return shptnum;
}

const char* UnitType::getTName() const
{
        return tname;
}

const char* UnitType::getName() const
{
	return name;
}

vector<char*> UnitType::getOwners() const
{
	return owners;
}

Uint8 UnitType::getOffset() const
{
        return offset;
}

Uint8 UnitType::getROT() const
{
	return turnspeed;
}

Sint8 UnitType::getMoveMod() const
{
	return movemod;
}

Uint8 UnitType::getTurnMod() const
{
	return turnmod;
}

Uint8 UnitType::getTurnspeed() const
{
	return turnspeed;
}

armor_t UnitType::getArmor() const
{
	return armour;
}

Weapon* UnitType::getWeapon() const
{
	return getWeapon(true);
}

Weapon* UnitType::getWeapon(bool primary) const
{
	if (primary == true) {
		return primary_weapon;
	}
	return secondary_weapon;
}

const char* UnitType::getDeployTarget() const
{
	return deploytarget;
}

StructureType* UnitType::getDeployType() const
{
	return deploytype;
}

Uint8 UnitType::getPipColour() const
{
	return pipcolour;
}

Uint8 UnitType::getMaxPassengers() const
{
	return maxpassengers;
}

vector<Uint8> UnitType::getPassengerAllow() const
{
	return passengerAllow;
}

vector<UnitType*> UnitType::getSpecificTypeAllow() const
{
	return specificTypeAllow;
}

Uint8 UnitType::getPQueue() const
{
	return ptype;
}

bool UnitType::isC4() const
{
	return c4;
}

bool UnitType::isInfiltrate()
{
	return infiltrate;
}

void UnitType::setInfiltrate(bool infiltrate)
{
	this->infiltrate = infiltrate;
}
