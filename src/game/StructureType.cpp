
#include "UnitOrStructureType.h"

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

#include "misc/common.h"
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

/**
 * @param tname Basic name of the StructureType (ex: FENC, WEAP ...)
 * @param structini 
 * @param artini
 * @param thext
 */
StructureType::StructureType(const string& typeName, INIFile* structini, INIFile* artini, 
	const string& thext) :
	UnitOrStructureType()
{
	SHPImage *shpimage;
	SHPImage *makeimage;
	Uint32 i = 0;
	char imagename[8];
	//char* tmpCharges;
	//char* tmpWaterBound;
	char blocktest[128];
	Uint32 size;
	//char* miscnames;
	

	// Load the rules.ini file
	INIFile* rulesIni = new INIFile("rules.ini");

	// Check if the section exist
	if (structini->isSection(typeName) == false)
	{
		logger->debug("Try to read a non existant ini section %s.\n", typeName.c_str());
		return;
	}

	is_wall = false;

	//name = structini->readString(typeName, "name");

	this->setName(typeName);
    
    // DEBUG
    //if (typeName == "WEAP")
    //{
    //    int a = 0;
    //}

    {
        // Read prerequiste
        string tmp = structini->readString(typeName, "prerequisites", "");
        //	printf ("%s line %i: Structure: %s --> prereqs = %s\n", __FILE__, __LINE__, tname, tmp);
        if (tmp.size() > 0)
        {
            //prereqs = splitList(tmp..c_str(), ',');
            Split(prereqs, tmp, ',');
        }
    }
    
#if 0
        // FOR DEBUG ONLY
	printf ("%s line %i: Structure: %s \n", __FILE__, __LINE__, tname);
	for (unsigned int j = 0; j < prereqs.size(); j++)
	{
		printf ("prereq = %s\n", prereqs[j]);
	}
#endif
        
	string tmp = structini->readString(typeName, "deploywith", "");
	if (tmp.size() > 0)
	{
        Split(deploywith, tmp, ',');
	}

	// Get the owners
	string ownersString = structini->readString(typeName, "owners", "");
    // Split the List of owners
	Split(owners, ownersString, ',');        
    // Custom warning
	if (owners.empty())
	{
		logger->warning("%s has no owners\n", typeName.c_str());
	}
    

#if 0
	// DEBUG
	logger->debug("%s line %i: Structure: %s \n",__FILE__ , __LINE__, tname.c_str());
	for (unsigned int j = 0; j < owners.size(); j++)
	{
		logger->debug("owners = %s\n", owners[j]);
	}
#endif

    // Determine if it's a Wall with knowed Wall structure
    if (typeName == "BRIK" || typeName == "SBAG" || typeName == "BARB" || typeName == "CYCL" || typeName == "FENC")
    {
        is_wall = true;
    }
    else
    {
        is_wall = false;
    }

	// the size of the structure in tiles
	xsize = artini->readInt(typeName, "xsize", 1);
	ysize = artini->readInt(typeName, "ysize", 1);

	// Get the blockers
	blckoff = 0;
	blocked = new Uint8[xsize*ysize];
	for (i = 0; i < (Uint32)xsize*ysize; i++)
	{
		sprintf(blocktest, "notblocked%d", i);
		// @todo BUG : try catch
		try
		{
			size = artini->readInt(typeName, blocktest);

			blocked[i] = 0;

		}
		catch (...)
		{
			sprintf(blocktest, "halfblocked%d", i);

			try
			{
				size = artini->readInt(typeName, blocktest);

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
	//memset(shpname, 0, 13);
	numshps = structini->readInt(typeName, "layers", 1);

	shpnums = new Uint16[(is_wall?numshps:numshps+1)];
	shptnum = new Uint16[numshps];

	// Read the Tech level
	techLevel = structini->readInt(typeName, "techlevel", -1);

	powerinfo.power = structini->readInt(typeName, "power", 0);
	powerinfo.drain = structini->readInt(typeName, "drain", 0);
	powerinfo.powered = structini->readInt(typeName, "powered", 0) != 0;
	maxhealth = structini->readInt(typeName, "health", 100);

	// Read "Charges"
        charges = (structini->readYesNo(string(typeName), "Charges", 0) == 1);
	
        

	// Read if it's a "waterbound" structure	
	WaterBound = (structini->readYesNo(typeName, "WaterBound", 0) == 1);


	// @todo Read SHP ???????
	for (i = 0; i < numshps; i++)
	{
		// @todo TRY THIS (TEST !!!!!!!!!!!)
		sprintf(imagename, "image%d", i+1);
		tmp = structini->readString(typeName, imagename, "");
                string shpname;
		if (tmp.size() == 0)
		{
			shpname = string(typeName) + ".shp";
		}
		else
		{
                    shpname = tmp;// + ".shp";
		}

		try
		{
			shpimage = new SHPImage(shpname.c_str(), -1);
		}
		catch (ImageNotFound&)
		{
			shpname = string(typeName) + string(thext);
			try
			{
				shpimage = new SHPImage(shpname.c_str(), -1);
			}
			catch (ImageNotFound&)
			{
				logger->warning("Image not found: \"%s\"\n", shpname.c_str());
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
		animinfo.loopend = structini->readInt(typeName, "loopend", 0);
		animinfo.loopend2 = structini->readInt(typeName, "loopend2", 0);

		animinfo.animspeed = structini->readInt(typeName, "animspeed", 3);
		animinfo.animspeed = abs(animinfo.animspeed);
		animinfo.animspeed = (animinfo.animspeed>1 ? animinfo.animspeed : 2);
		animinfo.animdelay = structini->readInt(typeName, "delay", 0);

		animinfo.animtype = structini->readInt(typeName, "animtype", 0);
		animinfo.sectype = structini->readInt(typeName, "sectype", 0);

		animinfo.dmgoff	= structini->readInt(typeName, "dmgoff", ((shptnum[0])>>1));
		if (numshps == 2)
			animinfo.dmgoff2 = structini->readInt(typeName, "dmgoff2", (shptnum[1]
					>>1));
		else
			animinfo.dmgoff2 = 0;

		defaultface = structini->readInt(typeName, "defaultface", 0);

                string shpMakeName;
		// Check if name of the structure <= 4
		if (typeName.size() <= 4)
		{
			shpMakeName = string(typeName) + "make.shp";
		}
		else
		{
			logger->warning("%s is nonstandard! (name lenght > 4)\n", typeName.c_str());
		}
		// Load the MAKE anim and store anim info
		try
		{
			makeimage = new SHPImage(shpMakeName.c_str(), -1);
			animinfo.makenum = makeimage->getNumImg();

			makeimg = pc::imagepool->size();
			pc::imagepool->push_back(makeimage); // Store make image			
		}
		catch (ImageNotFound&)
		{
                    // @todo try to load SHPNAME.XXX where XXX is 3 letters of theater
			makeimg = 0;
			animinfo.makenum = 0;
		}

        string strPri = structini->readString(typeName, "primary_weapon", "");
        if (strPri == "")
        {
            this->setPrimaryWeapon(0);
        }
        else
        {
            this->setPrimaryWeapon(p::weappool->getWeapon(strPri.c_str()));
        }
		string strSec = structini->readString(typeName, "secondary_weapon", "");
		if (strSec == "")
		{
			this->setSecondaryWeapon(0);
		}
		else
		{
			this->setSecondaryWeapon(p::weappool->getWeapon(strSec.c_str()));
		}
		turret = (structini->readInt(typeName, "turret", 0) != 0);
		if (turret)
		{
			turnspeed = structini->readInt(typeName, "rot", 3);
		}
	}
	else
	{
		numwalllevels = structini->readInt(typeName, "levels", 1);
		turret = 0;
                this->setPrimaryWeapon(0);
		this->setSecondaryWeapon(0);
	}

    // Read the Cost of the Structure Type
    setCost(structini->readInt(typeName, "cost", 0));

    // Reading of the armor
    string armStr = structini->readString(typeName, "armour", "none");
    if (armStr == "none")
    {
        armour = AC_none;
    }
    else
    {
        if (armStr == "none")
            armour = AC_none;
        else if (armStr == "wood")
            armour = AC_wood;
        else if (armStr == "light")
            armour = AC_light;
        else if (armStr == "heavy")
            armour = AC_heavy;
        else if (armStr == "concrete")
            armour = AC_concrete;
    }

	// Check if this structure is primary setable
	if (structini->readInt(typeName, "primary", 0) == 1)
	{
		primarysettable = true;
	}

	valid = true;

	// This is a hack :(
	AirBoundUnits = false;
	if (typeName == "AFLD" || typeName == "HPAD")
	{
		AirBoundUnits = true;
	}

	// Read the Adjacent
	this->adjacent = rulesIni->readInt(typeName, "Adjacent", 1);

	// Read the Sight	
	this->sight = rulesIni->readInt(typeName, "Sight", 1);

	// Free rules.ini
	delete rulesIni;
}

/**
 * Copy constructor
 */
StructureType::StructureType(const StructureType& orig)
{
    // Do Nothing
}

/**
 * Destructor
 */
StructureType::~StructureType()
{
	/*Uint16 i;
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
	shptnum = NULL;*/
}

Uint16 * StructureType::getSHPNums()
{
	return shpnums;
}

Uint16 * StructureType::getSHPTNum()
{
	return shptnum;
}

vector<string> StructureType::getDeployWith() const
{
	return deploywith;
}

vector<string> StructureType::getOwners() const
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
