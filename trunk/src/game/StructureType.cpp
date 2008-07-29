// StructureType.cpp
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

#include "StructureType.h"

#include <cstring>

#include "SDL/SDL_types.h"

#include "include/common.h"
#include "include/Logger.h"
#include "weaponspool.h"
#include "video/ImageNotFound.h"
#include "video/SHPImage.h"
#include "Weapon.h"
#include "animinfo_t.h"
#include "misc/INIFile.h"
#include "video/ImageCache.h"
#include "video/ImageCacheEntry.h"

extern Logger *logger;
namespace pc
{
extern vector<SHPImage *> *imagepool;
extern ImageCache* imgcache;
}
namespace p
{
extern WeaponsPool* weappool;
}

StructureType::StructureType(const char* typeName, INIFile *structini,
		INIFile *artini, const char* thext) :
	UnitOrStructureType()
{
	SHPImage *shpimage;
	SHPImage *makeimage;
	Uint32 i = 0;
	char shpname[13];
	char imagename[8];
	char* tmpCharges;
	char* tmpWaterBound;
	char* tmp;
	char blocktest[128];
	Uint32 size;
	char* miscnames;
	INIFile* rulesIni = 0;

	// Load the rules.ini file
	rulesIni = new INIFile("rules.ini");

	// Check if the section exist
	if (structini->isSection(string(typeName)) == false)
	{
		logger->debug("Try to read a non existant ini section %s.\n", typeName);
		return;
	}

	// Ensure that there is a section in the ini file
	try
	{
		structini->readKeyValue(typeName,0);
	}
	catch (...)
	{
		// Log that section doesn't exist
		logger->debug("The section [%s] of the building no exists in  inifile (StructureType constructor).\n", typeName);

		shpnums = NULL;
		blocked = NULL;
		shptnum = NULL;
		name = NULL;
		return;
	}

	is_wall = false;

	memset(this->tname, 0x0, 8);
	strncpy(this->tname, typeName, 8);
	name = structini->readString(tname, "name");
	//prereqs = structini->splitList(tname,"prerequisites",',');
	tmp = structini->readString(tname, "prerequisites");
	//	printf ("%s line %i: Structure: %s --> prereqs = %s\n", __FILE__, __LINE__, tname, tmp);

	if (tmp != NULL)
	{
		prereqs = splitList(tmp, ',');
		delete[] tmp;
		tmp = NULL;
	}
#if 0
	printf ("%s line %i: Structure: %s \n", __FILE__, __LINE__, tname);
	for (unsigned int j = 0; j < prereqs.size(); j++)
	{
		printf ("prereq = %s\n", prereqs[j]);
	}
#endif
	tmp = structini->readString(tname, "deploywith");

	if (tmp != NULL)
	{
		deploywith = splitList(tmp, ',');
		delete[] tmp;
		tmp = NULL;
	}

	// Get the owners
	//owners = structini->splitList(tname,"owners",',');
	tmp = structini->readString(tname, "owners");
	if (tmp != NULL)
	{
		owners = splitList(tmp, ',');
		delete[] tmp;
		tmp = NULL;
	}
	if (owners.empty())
	{
		logger->warning("%s has no owners\n", tname);
	}

#if 0
	// DEBUG
	logger->debug("%s line %i: Structure: %s \n",__FILE__ , __LINE__, tname);
	for (unsigned int j = 0; j < owners.size(); j++)
	{
		logger->debug("owners = %s\n", owners[j]);
	}
#endif

	// Determine if it's a Wall with knowed Wall structure
	string strComp = string(tname);
	if (strComp == "BRIK" || strComp == "SBAG" || strComp == "BARB" || strComp
			== "CYCL" || strComp == "FENC")
	{
		is_wall = true;
	}
	else
	{
		is_wall = false;
	}

	// the size of the structure in tiles
	xsize = artini->readInt(tname, "xsize", 1);
	ysize = artini->readInt(tname, "ysize", 1);

	// Get the blockers
	blckoff = 0;
	blocked = new Uint8[xsize*ysize];
	for (i = 0; i < (Uint32)xsize*ysize; i++)
	{
		sprintf(blocktest, "notblocked%d", i);
		// @todo BUG : try catch
		try
		{
			size = artini->readInt(tname, blocktest);

			blocked[i] = 0;

		}
		catch (...)
		{
			sprintf(blocktest, "halfblocked%d", i);

			try
			{
				size = artini->readInt(tname, blocktest);

				// Oke, the tile is half blocked
				blocked[i] = 2;
				xoffset = -(i%xsize)*24;
				yoffset = -(i/xsize)*24;
				if (blocked[blckoff] == 0)
				{
					blckoff = i;
				}

			}
			catch (...)
			{
				blocked[i] = 1;
				xoffset = -(i%xsize)*24;
				yoffset = -(i/xsize)*24;
				if (blocked[blckoff] == 0)
				{
					blckoff = i;
				}
			}
		}
	}

	/*
	 //  Change per 02-02-2007 allow units to drive over buildings (example service depot (for repairing units))
	 //	printf ("%s line %i: BEFORE Struct = %s, width = %i, heigth = %i, xoffset = %i, yoffset = %i\n", __FILE__, __LINE__, tname, xsize, ysize, xoffset/24, yoffset/24);
	 xoffset = 0;
	 yoffset = 0;
	 */
	memset(shpname, 0, 13);
	numshps = structini->readInt(tname, "layers", 1);

	shpnums = new Uint16[(is_wall?numshps:numshps+1)];
	shptnum = new Uint16[numshps];

	// Read the Tech level
	// @todo : refactor this !!!!!!!
	//techLevel = structini->readInt(tname,"TechLevel", -1);
	techLevel = (Sint32)structini->readInt(tname, "techlevel", (Sint32)-1);

	powerinfo.power = structini->readInt(tname, "power", 0);
	powerinfo.drain = structini->readInt(tname, "drain", 0);
	powerinfo.powered = structini->readInt(tname, "powered", 0) != 0;
	maxhealth = structini->readInt(tname, "health", 100);

	// Read "Charges"
	tmpCharges = (structini->readString(tname, "Charges"));
	charges = false;
	if (tmpCharges != NULL)
	{
		if (strcmp(tmpCharges, "yes") == 0)
		{
			charges = true;
		}
		delete[] tmpCharges;
		tmpCharges = NULL;
	}

	// Read if it's a "waterbound" structure
	tmpWaterBound = (structini->readString(tname, "WaterBound"));
	WaterBound = false;
	if (tmpWaterBound != 0)
	{
		if (strcmp(tmpWaterBound, "yes") == 0)
		{
			WaterBound = true;
		}
		delete[] tmpWaterBound;
		tmpWaterBound = 0;
	}

	// @todo Read SHP ???????
	for (i = 0; i < numshps; i++)
	{
		// @todo TRY THIS (TEST !!!!!!!!!!!)
		sprintf(imagename, "image%d", i+1);
		tmp = structini->readString(tname, imagename);
		if (tmp == NULL)
		{
			strncpy(shpname, tname, 13);
			strncat(shpname, ".shp", 13);
		}
		else
		{
			strncpy(shpname, tmp, 13);
			delete[] tmp;
			tmp = NULL;
		}

		try
		{
			shpimage = new SHPImage(shpname, -1);
		}
		catch (ImageNotFound&)
		{
			strncpy(shpname, tname, 13);
			strncat(shpname, thext, 13);
			try
			{
				shpimage = new SHPImage(shpname, -1);
			}
			catch (ImageNotFound&)
			{
				logger->warning("Image not found: \"%s\"\n", shpname);
				numshps = 0;
				return;
			}
		}
		shpnums[i] = pc::imagepool->size();
		shptnum[i] = shpimage->getNumImg();
		pc::imagepool->push_back(shpimage);

		/*
		 string stringShpName = string(tname);
		 if (i>0){
		 stringShpName += i;
		 }
		 stringShpName += ".shp";
		 // Load images of the structure layer in da cache
		 shpnums[i] = pc::imgcache->loadImage(stringShpName.c_str());
		 // Set the number of images
		 shptnum[i] = pc::imgcache->getImage(shpnums[i]).NumbImages;
		 */
	}

	if (!is_wall)
	{
		numwalllevels = 0;
		animinfo.loopend = structini->readInt(tname, "loopend", 0);
		animinfo.loopend2 = structini->readInt(tname, "loopend2", 0);

		animinfo.animspeed = structini->readInt(tname, "animspeed", 3);
		animinfo.animspeed = abs(animinfo.animspeed);
		animinfo.animspeed = (animinfo.animspeed>1 ? animinfo.animspeed : 2);
		animinfo.animdelay = structini->readInt(tname, "delay", 0);

		animinfo.animtype = structini->readInt(tname, "animtype", 0);
		animinfo.sectype = structini->readInt(tname, "sectype", 0);

		animinfo.dmgoff
				= structini->readInt(tname, "dmgoff", ((shptnum[0])>>1));
		if (numshps == 2)
			animinfo.dmgoff2 = structini->readInt(tname, "dmgoff2", (shptnum[1]
					>>1));
		else
			animinfo.dmgoff2 = 0;

		defaultface = structini->readInt(tname, "defaultface", 0);

		// Check if name of the structure <= 4
		if (strlen(tname) <= 4)
		{
			strncpy(shpname, tname, 13);
			strncat(shpname, "make.shp", 13);
		}
		else
		{
			logger->warning("%s is nonstandard! (name lenght > 4)\n", tname);
		}
		// Load the MAKE anim and store anim info
		try
		{
			makeimage = new SHPImage(shpname, -1);
			animinfo.makenum = makeimage->getNumImg();

			makeimg = pc::imagepool->size();
			pc::imagepool->push_back(makeimage); // Store make image			
		}
		catch (ImageNotFound&)
		{
			makeimg = 0;
			animinfo.makenum = 0;
		}

		miscnames = structini->readString(tname, "primary_weapon");
		if (miscnames == NULL)
		{
			primary_weapon = NULL;
		}
		else
		{
			primary_weapon = p::weappool->getWeapon(miscnames);
			delete[] miscnames;
			miscnames = NULL;
		}
		miscnames = structini->readString(tname, "secondary_weapon");
		if (miscnames == NULL)
		{
			secondary_weapon = NULL;
		}
		else
		{
			secondary_weapon = p::weappool->getWeapon(miscnames);
			delete[] miscnames;
			miscnames = NULL;
		}
		turret = (structini->readInt(tname, "turret", 0) != 0);
		if (turret)
		{
			turnspeed = structini->readInt(tname, "rot", 3);
		}
	}
	else
	{
		numwalllevels = structini->readInt(tname, "levels", 1);
		turret = 0;
		primary_weapon = NULL;
		secondary_weapon = NULL;
	}

	// Read the Cost of the Structure Type
	cost = structini->readInt(tname, "cost", 0);

	// Reading of the armor
	miscnames = structini->readString(tname, "armour", "none");
	if (miscnames == 0)
	{
		armour = AC_none;
	}
	else
	{
		if (strncmp((miscnames), ("none"), (4)) == 0)
			armour = AC_none;
		else if (strncmp((miscnames), ("wood"), (4)) == 0)
			armour = AC_wood;
		else if (strncmp((miscnames), ("light"), (5)) == 0)
			armour = AC_light;
		else if (strncmp((miscnames), ("heavy"), (5)) == 0)
			armour = AC_heavy;
		else if (strncmp((miscnames), ("concrete"), (8)) == 0)
			armour = AC_concrete;

		delete[] miscnames;
		miscnames = 0;
	}

	// Check if this structure is primary setable
	if (structini->readInt(tname, "primary", 0) == 1)
	{
		primarysettable = true;
	}

	valid = true;

	// This is a hack :(
	AirBoundUnits = false;
	if (strcmp((char*)tname, "AFLD") == 0 || strcmp((char*)tname, "HPAD") == 0)
	{
		AirBoundUnits = true;
	}

	// Read the Adjacent
	this->adjacent = rulesIni->readInt(tname, "Adjacent", 1);

	// Read the Sight	
	this->sight = rulesIni->readInt(tname, "Sight", 1);

	// Free rules.ini
	delete rulesIni;
}

StructureType::~StructureType()
{
	Uint16 i;
	for (i=0; i<owners.size(); ++i)
	{
		if (owners[i] != NULL)
			delete[] owners[i];
		owners[i] = NULL;
	}
	for (i=0; i<prereqs.size(); ++i)
	{
		if (prereqs[i] != NULL)
			delete[] prereqs[i];
		prereqs[i] = NULL;
	}
	if (shpnums != NULL)
		delete[] shpnums;
	shpnums = NULL;
	if (blocked != NULL)
		delete[] blocked;
	blocked = NULL;
	if (shptnum != NULL)
		delete[] shptnum;
	shptnum = NULL;
	if (name != NULL)
		delete[] name;
	name = NULL;
}

Uint16 * StructureType::getSHPNums()
{
	return shpnums;
}

Uint16 * StructureType::getSHPTNum()
{
	return shptnum;
}

const char * StructureType::getTName() const
{
	return tname;
}
const char * StructureType::getName() const
{
	return name;
}

vector < char *> StructureType::getDeployWith() const
{
	return deploywith;
}

vector < char *> StructureType::getOwners() const
{
	return owners;
}

Uint8 StructureType::getNumLayers() const
{
	return numshps;
}

Uint16 StructureType::getMakeImg() const
{
	return makeimg;
}

bool StructureType::isWaterBound() const
{
	return WaterBound;
}
bool StructureType::hasAirBoundUnits() const
{
	return AirBoundUnits;
}
Uint8 StructureType::getXsize() const
{
	return xsize;
}
Uint8 StructureType::getYsize() const
{
	return ysize;
}
Uint8 StructureType::isBlocked(Uint16 tile) const
{
	return blocked[tile];
}
Sint8 StructureType::getXoffset() const
{
	return xoffset;
}
Sint8 StructureType::getYoffset() const
{
	return yoffset;
}
Uint8 StructureType::getOffset() const
{
	return 0;
}

Uint8 StructureType::getTurnspeed() const
{
	return turnspeed;
}

armor_t StructureType::getArmor() const
{
	return armour;
}

Uint32 StructureType::getAdjacent() const
{
	return adjacent;
}

animinfo_t StructureType::getAnimInfo() const
{
	return animinfo;
}

PowerInfo StructureType::getPowerInfo() const
{
	return powerinfo;
}

bool StructureType::isPowered()
{
	if (powerinfo.powered)
		return true;
	return false;
}

/**
 * Return the Primary weapon
 * 
 * @return Reference to the primary weapon
 * @see Weapon
 */
Weapon * StructureType::getWeapon() const
{
	// Return Reference to the Primary weapon
	return getWeapon(true);
}

/**
 * Return the weapon of the structure wanted
 * 
 * @param primary if true select the primary weapon else return the secondary
 * @return Reference to the selected weapon
 * @see Weapon
 */
Weapon * StructureType::getWeapon(bool primary) const
{
	if (primary)
	{
		return primary_weapon;
	}
	else
	{
		return secondary_weapon;
	}
}

bool StructureType::hasTurret() const
{
	return turret;
}

Uint16 StructureType::getBlckOff() const
{
	return blckoff;
}

bool StructureType::isInfantry() const
{
	return false;
}

Uint8 StructureType::getNumWallLevels() const
{
	return numwalllevels;
}

Uint8 StructureType::getDefaultFace() const
{
	return defaultface;
}

Uint8 StructureType::getBuildlevel() const
{
	return buildlevel;
}

bool StructureType::primarySettable() const
{
	return primarysettable;
}

bool StructureType::Charges()
{
	return charges;
}

Uint8 StructureType::getPQueue() const
{
	return 0;
}

bool StructureType::isStructure() const
{
	return true;
}

bool StructureType::isWall() const
{
	return is_wall;
}

Uint8 StructureType::getSpeed() const
{
	return 0;
}
