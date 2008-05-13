#include "StructureType.h"
#include <cstdlib>
#include <cstring>
#include "include/Logger.h"
#include "video/ImageNotFound.h"
#include "video/SHPImage.h"
#include "Weapon.h"
#include "animinfo_t.h"
#include "misc/INIFile.h"

extern Logger *logger;

StructureType::StructureType(const char* typeName, INIFile *structini,
		INIFile *artini, const char* thext) :
	UnitOrStructureType() 
{
	SHPImage *shpimage;
	SHPImage *makeimage;
	Uint32 i = 0;
	char shpname[13], imagename[8];
	char *tmpCharges, *tmpWaterBound;
	char* tmp;
	char blocktest[128];
	Uint32 size;
	char* miscnames;


	// Check if the section exist
	//if (structini->isSection(string("65dg46dgg6")) == false){
	if (structini->isSection(string(typeName)) == false){
		logger->debug("Try to read a non existant ini section %s.\n", typeName);
		return;
	}
	
	// Ensure that there is a section in the ini file
	try {
		structini->readKeyValue(typeName,0);
	} catch (...) {
		
		logger->debug("Try to read a non existant ini section %s in StructureType constructor.\n", typeName);
		
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

	if (tmp != NULL) {
		prereqs = splitList(tmp, ',');
		delete[] tmp;
		tmp = NULL;
	}
#if 0
	printf ("%s line %i: Structure: %s \n", __FILE__, __LINE__, tname);
	for (unsigned int j = 0; j < prereqs.size(); j++) {
		printf ("prereq = %s\n", prereqs[j]);
	}
#endif
	tmp = structini->readString(tname, "deploywith");

	if (tmp != NULL) {
		deploywith = splitList(tmp, ',');
		delete[] tmp;
		tmp = NULL;
	}

	// Get the owners
	//owners = structini->splitList(tname,"owners",',');
	tmp = structini->readString(tname, "owners");
	if (tmp != NULL) {
		owners = splitList(tmp, ',');
		delete[] tmp;
		tmp = NULL;
	}
	if (owners.empty()) {
		logger->warning("%s has no owners\n", tname);
	}
#if 1
	logger->debug("%s line %i: Structure: %s \n",__FILE__ , __LINE__, tname);
	for (unsigned int j = 0; j < owners.size(); j++) {
		logger->debug("owners = %s\n", owners[j]);
	}
#endif

	// Determine if it's a Wall with knowed Wall structure
	// TODO DEBUT
	/*
	if( !_stricmp((tname), ("BRIK")) || !_stricmp((tname), ("SBAG")) ||
	!_stricmp((tname), ("BARB")) || !_stricmp((tname), ("WOOD")) ||
	!_stricmp((tname), ("CYCL")) || !_stricmp((tname), ("FENC")) )
	is_wall = true;
	*/	
	if( !strcmp((tname), ("BRIK")) || !strcmp((tname), ("SBAG")) ||
	!strcmp((tname), ("BARB")) || !strcmp((tname), ("WOOD")) ||
	!strcmp((tname), ("CYCL")) || !strcmp((tname), ("FENC")) ){
		is_wall = true;
	}
	
	// the size of the structure in tiles
	xsize = artini->readInt(tname, "xsize",1);
	ysize = artini->readInt(tname, "ysize",1);

	// Get the blockers
	blckoff = 0;
	blocked = new Uint8[xsize*ysize];
	for( i = 0; i < (Uint32)xsize*ysize; i++ ) {
		sprintf(blocktest, "notblocked%d", i);
		// TODO BUG : try catch
		try 
		{
			size = artini->readInt(tname, blocktest);
			
			blocked[i] = 0;
			
		} catch (...)
		{
			sprintf(blocktest, "halfblocked%d", i);
			
			try 
			{
				size = artini->readInt(tname, blocktest);
				
				// Oke, the tile is half blocked
				blocked[i] = 2;
				xoffset = -(i%xsize)*24;
				yoffset = -(i/xsize)*24;
				if (blocked[blckoff] == 0) {
					blckoff = i;
				}
				
			} catch (...)
			{
				blocked[i] = 1;
				xoffset = -(i%xsize)*24;
				yoffset = -(i/xsize)*24;
				if (blocked[blckoff] == 0) {
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
	memset(shpname,0,13);
	numshps = structini->readInt(tname, "layers",1);

	shpnums = new Uint16[(is_wall?numshps:numshps+1)];
	shptnum = new Uint16[numshps];

	buildlevel = structini->readInt(tname,"buildlevel",100);
	techlevel = structini->readInt(tname,"techlevel",99);
	if (buildlevel == 100) {
		logger->warning("%s does not have a buildlevel\n",tname);
	}

	powerinfo.power = structini->readInt(tname, "power", 0);
	powerinfo.drain = structini->readInt(tname, "drain", 0);
	powerinfo.powered = structini->readInt(tname, "powered", 0) != 0;
	maxhealth = structini->readInt(tname, "health", 100);

	// Read "Charges"
	tmpCharges = (structini->readString(tname, "Charges"));
	charges = false;
	if (tmpCharges != NULL) {
		if (strcmp (tmpCharges, "yes") == 0) {
			charges = true;
		}
		delete[] tmpCharges;
		tmpCharges = NULL;
	}

	// Read if it's a "waterbound" structure
	tmpWaterBound = (structini->readString(tname, "WaterBound"));
	WaterBound = false;
	if (tmpWaterBound != NULL) {
		if (strcmp (tmpWaterBound, "yes") == 0) {
			WaterBound = true;
		}
		delete[] tmpWaterBound;
		tmpWaterBound = NULL;
	}

	for( i = 0; i < numshps; i++ ) {
		sprintf(imagename, "image%d", i+1);
		tmp = structini->readString(tname, imagename);
		if( tmp == NULL ) {
			strncpy(shpname, tname, 13);
			strncat(shpname, ".SHP", 13);
		} else {
			strncpy(shpname, tmp, 13);
			delete[] tmp;
			tmp = NULL;
		}
		try {
			shpimage = new SHPImage(shpname, mapscaleq);
		} catch (ImageNotFound&) {
			strncpy(shpname, tname, 13);
			strncat(shpname, thext, 13);
			try {
				shpimage = new SHPImage(shpname, mapscaleq);
			} catch (ImageNotFound&) {
				logger->warning("Image not found: \"%s\"\n", shpname);
				numshps = 0;
				return;
			}
		}
		shpnums[i] = pc::imagepool->size();
		shptnum[i] = shpimage->getNumImg();
		pc::imagepool->push_back(shpimage);
	}
	if (!is_wall) {
		numwalllevels = 0;
		animinfo.loopend = structini->readInt(tname,"loopend",0);
		animinfo.loopend2 = structini->readInt(tname,"loopend2",0);

		animinfo.animspeed = structini->readInt(tname,"animspeed", 3);
		animinfo.animspeed = abs(animinfo.animspeed);
		animinfo.animspeed = (animinfo.animspeed>1?animinfo.animspeed:2);
		animinfo.animdelay = structini->readInt(tname,"delay",0);

		animinfo.animtype = structini->readInt(tname, "animtype", 0);
		animinfo.sectype = structini->readInt(tname, "sectype", 0);

		animinfo.dmgoff = structini->readInt(tname, "dmgoff", ((shptnum[0])>>1));
		if (numshps == 2)
		animinfo.dmgoff2 = structini->readInt(tname, "dmgoff2", (shptnum[1]>>1));
		else
		animinfo.dmgoff2 = 0;

		defaultface = structini->readInt(tname, "defaultface", 0);

		if (strlen(tname) <= 4) {
			strncpy(shpname, tname, 13);
			strncat(shpname, "make.shp", 13);
		} else
		logger->warning("%s is nonstandard!\n",tname);
		try {
			makeimage = new SHPImage(shpname, mapscaleq);
			makeimg = pc::imagepool->size();
			animinfo.makenum = makeimage->getNumImg();
			pc::imagepool->push_back(makeimage);
		} catch (ImageNotFound&) {
			makeimg = 0;
			animinfo.makenum = 0;
		}

		miscnames = structini->readString(tname, "primary_weapon");
		if( miscnames == NULL ) {
			primary_weapon = NULL;
		} else {
			primary_weapon = p::weappool->getWeapon(miscnames);
			delete[] miscnames;
			miscnames = NULL;
		}
		miscnames = structini->readString(tname, "secondary_weapon");
		if( miscnames == NULL ) {
			secondary_weapon = NULL;
		} else {
			secondary_weapon = p::weappool->getWeapon(miscnames);
			delete[] miscnames;
			miscnames = NULL;
		}
		turret = (structini->readInt(tname,"turret",0) != 0);
		if (turret) {
			turnspeed = structini->readInt(tname,"rot",3);
		}
	} else {
		numwalllevels = structini->readInt(tname,"levels",1);
		turret = 0;
		primary_weapon = NULL;
		secondary_weapon = NULL;
	}
	cost = structini->readInt(tname, "cost", 0);
	if (0 == cost) {
		logger->error("\"%s\" has no cost, resetting to 1\n", tname);
		cost = 1;
	}

	// Reading of the armor
	miscnames = structini->readString(tname,"armour","none");
	if (miscnames == NULL) {
		armour = AC_none;
	} else {
		// TODO DEBUG _stricmp(
		/*
		if (_strnicmp((miscnames), ("none"), (4)) == 0)
		armour = AC_none;
		else if (_strnicmp((miscnames), ("wood"), (4)) == 0)
		armour = AC_wood;
		else if (_strnicmp((miscnames), ("light"), (5)) == 0)
		armour = AC_light;
		else if (_strnicmp((miscnames), ("heavy"), (5)) == 0)
		armour = AC_heavy;
		else if (_strnicmp((miscnames), ("concrete"), (8)) == 0)
		armour = AC_concrete;
		*/
		if (strncmp((miscnames), ("none"), (4)) == 0)
			armour = AC_none;
		else {
			if (strncmp((miscnames), ("wood"), (4)) == 0)
				armour = AC_wood;
			else if (strncmp((miscnames), ("light"), (5)) == 0)
					armour = AC_light;
				 else if (strncmp((miscnames), ("heavy"), (5)) == 0)
				 		armour = AC_heavy;
				 	else if (strncmp((miscnames), ("concrete"), (8)) == 0)
				 		armour = AC_concrete;
		}
		
		delete[] miscnames;
		miscnames = NULL;
	}

	// Check if this structure is primary setable
	if (structini->readInt(tname, "primary", 0) == 1){
		primarysettable = true;
	}
	
	
	valid = true;

	// This is a hack :(
	AirBoundUnits = false;
	if (strcmp ((char*)tname, "AFLD") == 0 || strcmp ((char*)tname, "HPAD") == 0) {
		AirBoundUnits = true;
	}
}

Uint16 * StructureType::getSHPNums() {
	return shpnums;
}
Uint16 * StructureType::getSHPTNum() {
	return shptnum;
}
const char * StructureType::getTName() const {
	return tname;
}
const char * StructureType::getName() const {
	return name;
}

std::vector < char *> StructureType::getDeployWith() const {
	return deploywith;
}
std::vector < char *> StructureType::getOwners() const {
	return owners;
}
Uint8 StructureType::getNumLayers() const {
	return numshps;
}
Uint16 StructureType::getMakeImg() const {
	return makeimg;
}

bool StructureType::isWaterBound() const {
	return WaterBound;
}
bool StructureType::hasAirBoundUnits() const {
	return AirBoundUnits;
}
Uint8 StructureType::getXsize() const {
	return xsize;
}
Uint8 StructureType::getYsize() const {
	return ysize;
}
Uint8 StructureType::isBlocked(Uint16 tile) const {
	return blocked[tile];
}
Sint8 StructureType::getXoffset() const {
	return xoffset;
}
Sint8 StructureType::getYoffset() const {
	return yoffset;
}
Uint8 StructureType::getOffset() const {
	return 0;
}

Uint8 StructureType::getTurnspeed() const {
	return turnspeed;
}

armour_t StructureType::getArmour() const {
	return armour;
}
animinfo_t StructureType::getAnimInfo() const {
	return animinfo;
}
powerinfo_t StructureType::getPowerInfo() const {
	return powerinfo;
}
bool StructureType::isPowered() {
	if (powerinfo.powered)
		return true;
	return false;
}

Weapon * StructureType::getWeapon(bool primary) const {
	return (primary ? primary_weapon : secondary_weapon);
}

bool StructureType::hasTurret() const {
	return turret;
}
Uint16 StructureType::getBlckOff() const {
	return blckoff;
}
bool StructureType::isInfantry() const {
	return false;
}
Uint8 StructureType::getNumWallLevels() const {
	return numwalllevels;
}
Uint8 StructureType::getDefaultFace() const {
	return defaultface;
}
Uint8 StructureType::getBuildlevel() const {
	return buildlevel;
}

bool StructureType::primarySettable() const {
	return primarysettable;
}
bool StructureType::Charges() {
	return charges;
}
Uint8 StructureType::getPQueue() const {
	return 0;
}
bool StructureType::isStructure() const {
	return true;
}
bool StructureType::isWall() const{
	return is_wall;
}
Uint8 StructureType::getSpeed() const{
	return 0;
}
