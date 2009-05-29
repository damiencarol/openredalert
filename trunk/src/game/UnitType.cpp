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

#include "misc/common.h"
#include "CnCMap.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "PlayerPool.h"
#include "audio/SoundEngine.h"
#include "Unit.hpp"
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
UnitType::UnitType(const string& typeName, INIFile* unitini) :
	UnitOrStructureType(),
	shpnums(0),
	c4(false)
{
    string tname = typeName;
    SHPImage* shpimage = 0;

    string shpname;

	Uint32 shpnum;
	Uint32 tmpspeed;


    // Ensure that there is a section in the ini file
    if (unitini->isSection(typeName) == false)
    {
    	// Log it
    	logger->error("%s line %i: Unknown type: %s\n", __FILE__, __LINE__, typeName.c_str());

        shpnums = 0;
        shptnum = 0;
        return;
    }

    // Set the internal name
    this->setName(typeName);



	string tmp = unitini->readString(tname, "prerequisites", "");
	Split(prereqs, tmp, ',');

	unittype = unitini->readInt(tname, "unittype", 0);
	if (0 == unittype)
	{
		logger->warning("No unit type specified for \"%s\"\n", tname.c_str());
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

	for(unsigned int i = 0; i < numlayers; i++)
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
	techLevel = unitini->readInt(tname, "TechLevel", -1);

	tmp = unitini->readString(tname, "owners");
	Split(owners, tmp, ',');

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


	string talkmode;
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
	talkback = p::uspool->getTalkback(talkmode.c_str());
	
    maxhealth = unitini->readInt(tname, "health", 50);

    setCost(unitini->readInt(tname, "cost", 0));
    if (0 == getCost())
    {
        logger->error("\"%s\" has no cost, setting to 1\n", tname.c_str());
	setCost(1);
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

    
    doubleowned = (unitini->readYesNo(tname, "DoubleOwned", 0) == 1);
	

    // Read primary weapon
    string priStr = unitini->readString(tname, "Primary", "");
    if (priStr == "")
    {
        this->setPrimaryWeapon(0);
    }
    else
    {
        this->setPrimaryWeapon(p::weappool->getWeapon(priStr.c_str()));
    }
        
    //
    string secStr = unitini->readString(tname, "Secondary", "");
    if (secStr == "")
    {
        this->setSecondaryWeapon(0);
    }
    else
    {
        this->setSecondaryWeapon(p::weappool->getWeapon(secStr.c_str()));
    }
    
    
    string deploytarget = unitini->readString(tname, "deploysto", "");
    if (deploytarget.size() > 0)
    {
        deployable = true;
        deploytype = p::uspool->getStructureTypeByName(deploytarget.c_str());
    }
    else
    {
        deployable = false;
        deploytype = 0;
    }
    pipcolour = unitini->readInt(tname, "pipcolour", 0);

    // Read the armor
    string charArmor = unitini->readString(tname, "Armour", "");
    if (charArmor == "")
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
    
    
	valid = true;

#ifdef LOOPEND_TURN
	animinfo.loopend = unitini->readInt(typeName, "loopend", 31);
	animinfo.loopend2 = unitini->readInt(typeName, "loopend2", 0);

	animinfo.animspeed = unitini->readInt(typeName, "animspeed", 3);
	animinfo.animspeed = abs(animinfo.animspeed);
	animinfo.animspeed = (animinfo.animspeed>1 ? animinfo.animspeed : 2);
	animinfo.animdelay = unitini->readInt(typeName, "delay", 0);

	animinfo.animtype = unitini->readInt(typeName, "animtype", 0);
	animinfo.sectype = unitini->readInt(typeName, "sectype", 0);

	animinfo.dmgoff = unitini->readInt(typeName, "dmgoff", ((shptnum[0]-1)>>1));
#endif

    // Read the C4 caract
    this->c4 = (unitini->readYesNo(typeName, "C4", 0) == 1);

    // Read the Infiltrate caracteristic
    this->infiltrate = (unitini->readYesNo(typeName, "Infiltrate", 0) == 1);
}

/**
 * Destructor
 */
UnitType::~UnitType()
{
   /* if (shpnums != 0)
        delete[] shpnums;

    if (shptnum != 0)
        delete[] shptnum;

    if (deploytarget != 0)
        delete[] deploytarget;

    for (unsigned int i = 0; i < owners.size(); ++i)
    {
        if (owners[i] != 0)
            delete[] owners[i];
    }

    for (unsigned int i = 0; i < prereqs.size(); ++i)
    {
        if (prereqs[i] != 0)
            delete[] prereqs[i];
    }*/
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

vector<string> UnitType::getOwners() const
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
    // Return the production type (infantry/ship/vehicule/...)
    return getPType();
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
