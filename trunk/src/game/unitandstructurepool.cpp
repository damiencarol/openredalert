// UnitAndStructurePool.cpp
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

#include "UnitAndStructurePool.h"

#include <string>
#include <algorithm>
#include <vector>

#include "SDL/SDL_types.h"

#include "include/config.h"
#include "CnCMap.h"
#include "include/common.h"
#include "game/Game.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "PlayerPool.h"
#include "game/Unit.h"
#include "weaponspool.h"
#include "video/ImageCache.h"
#include "InfantryGroup.h"
#include "StructureType.h"
#include "UnitType.h"
#include "UnitOrStructureType.h"
#include "MissionData.h"
#include "L2Overlay.h"
#include "movement.h"
#include "Structure.h"
#include "pside.h"
#include "Player.h"
#include "Unit.h"
#include "GameMode.h"
#include "misc/INIFile.h"
#include "UnitAndStructureMat.h"
#include "Talkback.h"
#include "AiCommand.h"

using std::make_pair;
using std::map;
using std::multimap;
using std::pair;
using std::string;
using std::vector;

namespace pc {
    extern ConfigType Config;
    extern ImageCache* imgcache;
}
namespace p {
    extern CnCMap* ccmap;
	extern PlayerPool* ppool;
}
extern Logger * logger;

/**
 * Constructor, loads all the units from the inifile and create
 * those units/structures in the unit/structure pool
 */
UnitAndStructurePool::UnitAndStructurePool(const char* theTheater) :
	deleted_unitorstruct(false),
	numdeletedunit(0),
	numdeletedstruct(0)
{
	// CERTAIN STRUCTURE ARE VARIABLE WITH TEATHER
	strcpy(this->theaterext, ".");
	strncat(this->theaterext, theTheater, 3);

	// needed for populate by structures
	unitandstructmat.resize((p::ccmap->getWidth()+10) * (p::ccmap->getHeight()+10));
	//unitandstructmat.resize(100000);

	structini	= GetConfig("structure.ini");
	unitini		= GetConfig("unit.ini");
	tbackini	= GetConfig("talkback.ini");
	artini		= GetConfig("art.ini");

	// Create Weapons Pool
    p::weappool	= new WeaponsPool();

//	bib1 = pc::imgcache->loadImage("bib1.tem", -1);
//	bib2 = pc::imgcache->loadImage("bib2.tem", -1);
//	bib3 = pc::imgcache->loadImage("bib3.tem", -1);

	bib1 = 0;
	bib2 = 0;
	bib3 = 0;
}

/**
 * Destructor, empties the various matricies and deletes the WeaponsPool
 * and inifile members.
 */
UnitAndStructurePool::~UnitAndStructurePool()
{
    Uint32 i = 0;

    // this is for cleaning up the multimaps
    typedef multimap<UnitType*, vector<StructureType*>* >::const_iterator Iu;
    typedef multimap<StructureType*, vector<StructureType*>* >::const_iterator Is;
    std::pair<Is,Is> structpair;
    std::pair<Iu,Iu> unitpair;

    for( i = 0; i < unitpool.size(); i++ ) {
        delete unitpool[i];
    }

    for (i = 0; i < unittypepool.size(); i++)
    {
    	unitpair = unit_prereqs.equal_range(unittypepool[i]);

        for (Iu l = unitpair.first; l != unitpair.second ; ++l) {
            delete l->second;
        }
        delete unittypepool[i];
    }

    for( i = 0; i < structurepool.size(); i++ ) {
        //structurepool[i]->unrefer();
		delete structurepool[i];
    }

    for( i = 0; i < structuretypepool.size(); i++ ) {
        structpair = struct_prereqs.equal_range(structuretypepool[i]);
        for (Is l = structpair.first; l != structpair.second ; ++l) {
            delete l->second;
        }
        delete structuretypepool[i];
    }

    delete p::weappool;
}

/**
 * Retrieve the imagenumber for the Structure at the specified position
 *
 * @param cellpos the position we want to know the structure of.
 * @param inumbers pointer to array of image numbers
 * @param xoffsets pointer to array of x offsets
 * @param yoffsets pointer to array of y offsets
 * @returns the imagenumber.
 */
Uint8 UnitAndStructurePool::getStructureNum(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoffsets, Sint8 **yoffsets)
{
	Structure *st = 0;
	Uint8 layers = 0;

	// Check if the pos is correct
	if (cellpos >= p::ccmap->getSize()){
		return 0;
	}

	//
	if ( !(unitandstructmat[cellpos].flags & US_STR_LOWER_RIGHT)) {
		return 0;
	}

	if (unitandstructmat[cellpos].flags & (US_IS_STRUCTURE|US_IS_WALL))
    {
		st = getStructure((unitandstructmat[cellpos].structurenumb)); //structurepool[(unitandstructmat[cellpos])&0xffff];
		if (st == 0){
			// hmmm some error --> remove the structure from the unitandstructmat
			logger->error ("%s line %i: UnitandStructuremat error --> correcting\n", __FILE__, __LINE__);
			unitandstructmat[cellpos].flags &= ~(US_IS_STRUCTURE|US_IS_WALL|US_STR_LOWER_RIGHT);
			return 0;
		}
		layers = st->getImageNums(inumbers, xoffsets, yoffsets);
		return layers;
	}
	return 0;
}

/**
 * Retrieve the imagenumber for the Unit at the specified position
 *
 * @param cellpos the position we want to know the structure of.
 * @param inumbers pointer to array of image numbers
 * @param xoffsets pointer to array of x offsets
 * @param yoffsets pointer to array of y offsets
 * @returns the imagenumber.
 */
Uint8 UnitAndStructurePool::getUnitNum(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoffsets, Sint8 **yoffsets)
{
	Unit *un = 0;

	if (cellpos >= p::ccmap->getSize()){
		return 0;
	}

	if( !(unitandstructmat[cellpos].flags & US_UNIT_LOWER_RIGHT) ){
		return 0;
	}

	if( unitandstructmat[cellpos].flags & US_IS_UNIT ) {
		un = getUnit ((unitandstructmat[cellpos].unitnumb)); //unitpool[(unitandstructmat[cellpos])&0xffff];
		if (un == 0){
			// hmmm some error --> remove the unit from the unitandstructmat
			logger->error ("%s line %i: UnitandStructuremat error --> correcting\n", __FILE__, __LINE__);
			unitandstructmat[cellpos].flags &= ~(US_IS_UNIT|US_UNIT_LOWER_RIGHT);
			return 0;
		}
		if( ((UnitType *)un->getType())->isInfantry() ) {
			return un->getInfantryGroup()->GetImageNums(inumbers, xoffsets, yoffsets);
		}
		return un->getImageNums(inumbers, xoffsets, yoffsets);
	}
	return 0;
}

/**
 * Retrieve the imagenumber for the FlyingUnit at the specified position
 *
 * @param cellpos the position we want to know the structure of.
 * @param inumbers pointer to array of image numbers
 * @param xoffsets pointer to array of x offsets
 * @param yoffsets pointer to array of y offsets
 * @returns the imagenumber.
 */
Uint8 UnitAndStructurePool::getFlyingUnitNum(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoffsets, Sint8 **yoffsets)
{
	Unit *un = 0;

	if (cellpos >= p::ccmap->getSize())
		return 0;

	if( !(unitandstructmat[cellpos].flags&US_AIRUNIT_LOWER_RIGHT) )
		return 0;

	if( unitandstructmat[cellpos].flags & US_IS_AIRUNIT ) {
		un = getUnit ((unitandstructmat[cellpos].airunitnumb)); //unitpool[(unitandstructmat[cellpos])&0xffff];
		if (un == 0){
			// hmmm some error --> remove the unit from the unitandstructmat
			logger->error ("%s line %i: UnitandStructuremat error --> correcting\n", __FILE__, __LINE__);
			unitandstructmat[cellpos].flags &= ~(US_IS_AIRUNIT|US_AIRUNIT_LOWER_RIGHT);
			return 0;
		}
/*
		if( ((UnitType *)un->getType())->isInfantry() ) {
			return un->getInfantryGroup()->GetImageNums(inumbers, xoffsets, yoffsets);
		}
*/
		return un->getImageNums(inumbers, xoffsets, yoffsets);
	}
	return 0;
}

/**
 *
 */
bool UnitAndStructurePool::getUnitOrStructureLimAt(Uint32 curpos, float* width,
        float* height, Uint32* cellpos, Uint8* igroup, Uint8* owner,
        Uint8* pcol, bool* blocked)
{

	if (curpos >= p::ccmap->getSize())
		return false;

    if (curpos >= unitandstructmat.size() ){
		logger->error ("%s line %i: Failure: curpos:%i >= size:%i\n", __FILE__, __LINE__, curpos, unitandstructmat.size());
		return false;
    }


 //   Uint32 cval = unitandstructmat[curpos];
    if (unitandstructmat[curpos].flags & US_IS_UNIT) {
		Unit* un = getUnit (unitandstructmat[curpos].unitnumb);
		if (un == 0){
			logger->error ("%s line %i: UnitandStructuremat error --> correcting\n", __FILE__, __LINE__);
			unitandstructmat[curpos].flags &= ~(US_IS_UNIT|US_UNIT_LOWER_RIGHT);
			return false;
		}
        //Unit* un = unitpool[cval&0xffff];
        *width   = 0.75f;
        *height  = 0.75f;
        *owner   = un->getOwner();
        *pcol    = p::ppool->getPlayer(*owner)->getStructpalNum();
        *igroup  = 0;
        *cellpos = un->getPos();
        *blocked = true;
        if (un->getType()->isInfantry()) {
            InfantryGroup* igrp = un->getInfantryGroup();
			if (igrp != 0){
				for (int i=0;i<5;++i)
					if (igrp->UnitAt(i) != 0)
						*igroup |= 1<<i;
			}
        }
        return true;
    } else if ((unitandstructmat[curpos].flags & US_IS_STRUCTURE) || (unitandstructmat[curpos].flags & US_IS_WALL)) {
        Structure* st = getStructure (unitandstructmat[curpos].structurenumb); //structurepool[cval&0xffff];
		if (st == 0){
			logger->error ("%s line %i: UnitandStructuremat error --> correcting\n", __FILE__, __LINE__);
			unitandstructmat[curpos].flags &= ~(US_IS_STRUCTURE|US_IS_WALL|US_STR_LOWER_RIGHT);
			return false;
		}
        *width   = 1.0f;
        *height  = 1.0f;
        *owner   = st->getOwner();
        *pcol    = p::ppool->getPlayer(*owner)->getStructpalNum();
        *igroup  = 0;
        *cellpos = st->getBPos(curpos);
        *blocked = true;
        return true;
    }
    if (unitandstructmat[curpos].flags & US_IS_AIRUNIT) {
		Unit* un = getUnit (unitandstructmat[curpos].airunitnumb);
		if (un == 0){
			logger->error ("%s line %i: UnitandStructuremat error --> correcting\n", __FILE__, __LINE__);
			unitandstructmat[curpos].flags &= ~(US_IS_AIRUNIT|US_AIRUNIT_LOWER_RIGHT);
			return false;
		}
        //Unit* un = unitpool[cval&0xffff];
        *width   = 0.75f;
        *height  = 0.75f;
        *owner   = un->getOwner();
        *pcol    = p::ppool->getPlayer(*owner)->getStructpalNum();
        *igroup  = 0;
        *cellpos = un->getPos();
        *blocked = true;
        if (un->getType()->isInfantry()) {
            InfantryGroup* igrp = un->getInfantryGroup();
			if (igrp != 0){
				for (int i=0;i<5;++i)
					if (igrp->UnitAt(i) != 0){
						*igroup |= 1<<i;
                    }
			}
        }
        return true;
    }
    return false;
}

/**
 */
bool UnitAndStructurePool::hasL2overlay(Uint16 cellpos) const
{
	return (unitandstructmat[cellpos].flags&US_HAS_L2OVERLAY)!=0;
}

/**
 * Gets a list of all flying stuff in the current tile
 */
Uint8 UnitAndStructurePool::getL2overlays(Uint16 pos, Uint32 **inumbers, Sint8 **xoffset, Sint8 **yoffset)
{
	multimap<Uint16, L2Overlay*>::iterator entry;
    L2Overlay* curl2;
    Uint8 numentries;
    Uint8 i;
    Uint8 j;

	// Check that position is in the map
	if (pos >= p::ccmap->getSize()){
		return 0;
	}

    // If their are no overlay return 0
    if( !(unitandstructmat[pos].flags&US_HAS_L2OVERLAY) ) {
        return 0;
    }

    // find in the l2 OVERLAY pool
    entry = l2pool.find(pos);
    // get the number of overlay saved for this position
    numentries = numl2images[pos];

    // Alocate tables
    *inumbers = new Uint32[numentries];
    *xoffset = new Sint8[numentries];
    *yoffset = new Sint8[numentries];
    for( i = 0; i < numentries; i++ )
    {
        curl2 = entry->second;
        for (j=0; j<(curl2->numimages); ++j)
        {
            (*inumbers)[i+j] = curl2->imagenums[j];
            (*xoffset)[i+j] = curl2->xoffsets[j];
            (*yoffset)[i+j] = curl2->yoffsets[j];
        }
        entry++;
    }
    // return number of entries in the overlay
    return numentries;
}

multimap<Uint16, L2Overlay*>::iterator UnitAndStructurePool::addL2overlay(Uint16 cellpos, L2Overlay *ov)
{
    multimap<Uint16, L2Overlay*>::iterator entry;
    l2pool.insert(make_pair(cellpos, ov));
    unitandstructmat[cellpos].flags|=US_HAS_L2OVERLAY;
    numl2images[cellpos] += ov->numimages;
    entry = l2pool.find(cellpos);
    while(entry->second != ov) {
        entry++;
    }
    return entry;
}

void UnitAndStructurePool::removeL2overlay(multimap<Uint16, L2Overlay*>::iterator entry)
{
    Uint32 cellpos;

    cellpos = entry->first;
    numl2images[cellpos] -= entry->second->numimages;
    l2pool.erase(entry);
    if( l2pool.find(cellpos) == l2pool.end() ) {
        unitandstructmat[cellpos].flags&=(~US_HAS_L2OVERLAY);
    }
}

/**
 * Create a renforcement Team with type defined in maps data
 *
 * @todo check if the movement order list only apply to the last unit
 */
bool UnitAndStructurePool::createReinforcements(RA_Teamtype* Team)
{
	RA_TeamUnits RA_Unit;
	Uint16 cellpos = 0; //,xpos, ypos;
	Uint8 owner = 0;
	string name_trigger; // name of the trigger of all unit in team

	// get the number of player by the num of House in Team data
	owner = p::ppool->getPlayerNumByHouseNum(Team->country);

	//logger->debug("%s line %i: Team size = %i, Owner = %i\n", __FILE__, __LINE__, Team->Units.size(), (int)owner);

	// Get the trigger
	name_trigger = p::ccmap->getTriggerByNumb(Team->trigger)->name;

	// For each unit type
	for (unsigned int i = 0; i < Team->Units.size(); i++)
    {
        UnitType* type = getUnitTypeByName(Team->Units[i].tname.c_str());
        cellpos = p::ccmap->getWaypoint(Team->waypoint);
        if (0 == type) {
            logger->error("Invalid type \"%s\"\n",Team->Units[i].tname.c_str());
            return false;
        }

        Uint16 x = 0;
        Uint16 y = 0;
        p::ccmap->translateCoord(cellpos, &x, &y);

        logger->debug("SPAWN AT = %d %d       %d\n", x, y, cellpos);

        // For each unit
        for (int j = 0; j < Team->Units[i].numb; j++)
        {
			while (!p::ccmap->canSpawnAt(cellpos)){
				logger->debug("try SPAWN AT = %d %d    cell= %d\n", x, y, cellpos);
				cellpos++;
			}
			Unit* uni = 0;
			if (p::ccmap->canSpawnAt(cellpos))
			{
				if (type->isInfantry()){
					uni = createUnit(type, cellpos, 0, owner, 255, 0, COMMAND_GUARD, name_trigger);
				}else{
					uni = createUnit(type, cellpos, 1, owner, 255, 0, COMMAND_GUARD, name_trigger);
				}
			}else{
				printf ("%s line %i: Can't spawn at this position\n", __FILE__, __LINE__);
			}

			if (uni != 0)
			{
			    // Create aicommands
			    for (unsigned int k=0; k<Team->aiCommandList.size(); k++)
			    {
			    	AiCommand* newCommand = new AiCommand();
			    	newCommand->setId(Team->aiCommandList[k]->getId());
			    	newCommand->setWaypoint(Team->aiCommandList[k]->getWaypoint());

			    	uni->aiCommandList.push_back(newCommand);
			    	logger->debug(" (%d)--> [%d:%d]\n", k, uni->aiCommandList[k]->getId(),
			    			uni->aiCommandList[k]->getWaypoint());
			    }
			}
        }


    }
    return true;
}

/**
 * Creates a structure
 */
bool UnitAndStructurePool::createStructure(const char* typen, Uint16 cellpos,
        Uint8 owner, Uint16 health, Uint8 facing, bool makeanim, string trigger_name)
{
	// If the pos is not in the size
	if (cellpos >= p::ccmap->getSize()){
		return false;
	}

	// Get the type of the structure with his name
    StructureType* type = getStructureTypeByName(typen);
    if (0 == type) {
        logger->error("Invalid type \"%s\"\n",typen);
        return false;
    }

    // Create the structure
    bool res = createStructure(type, cellpos, owner, health, facing, makeanim, trigger_name);

    // Return success
    return res;
}

/**
 *
 */
bool UnitAndStructurePool::createStructure(StructureType* type, Uint16 cellpos,
        Uint8 owner, Uint16 health, Uint8 facing, bool makeanim, string trigger_name)
{
	Uint16 frame;
	Uint32 pos;

	if (cellpos >= p::ccmap->getSize())
		return false;

    int x = 0, y = 0, curpos = 0;
    Uint32 structnum = 0;
    Structure* st = 0;

    Uint32 br = cellpos + p::ccmap->getWidth()*(type->getYsize()-1);
    if (cellpos > p::ccmap->getSize() || (br > p::ccmap->getSize() && 0)) {
        logger->error("%s line %i: Attempted to create a \"%s\" at %i, outside map (%i)\n", __FILE__, __LINE__, type->getTName(), br, p::ccmap->getSize());
        return false;
    }

    // Reuse an expired structnum
    if (numdeletedstruct > 0) {
        for (structnum = 0; structnum < structurepool.size(); ++structnum) {
            if (0 == structurepool[structnum]) {break;}
        }
    } else {
        structnum = structurepool.size();
    }

    if (type->isWall())
    {
        // walls will always be one cell
        if (0 != getStructureAt(cellpos) ||
        	0 != (unitandstructmat[cellpos].flags & US_IS_UNIT)||
        	0 != (unitandstructmat[cellpos].flags & US_IS_AIRUNIT))
        {
            return false;
        }

		if (structnum > structurepool.size()){
			logger->error ("%s line %i: ERROR create structure, structnum = %i, size = %i\n", __FILE__, __LINE__, structnum, structurepool.size());
			structnum = structurepool.size();
		}

        unitandstructmat[cellpos].flags = (US_STR_LOWER_RIGHT|US_IS_WALL);
		unitandstructmat[cellpos].structurenumb = structnum;


    }
    else
    {
        /// @todo Rewrite this to use curpos in a more straightforward way.
        curpos = cellpos+p::ccmap->getWidth()*(type->getYsize());
        for (y = type->getYsize()-1; y>=0; --y) {
            curpos -= p::ccmap->getWidth();
            for (x = type->getXsize()-1; x>=0; --x) {
                if (type->isBlocked(y*type->getXsize()+x)) {
                    if (getStructureAt(curpos+x) != 0) {
                        Uint16 tx, ty;
                        p::ccmap->translateFromPos(curpos+x, &tx, &ty);
                        logger->error("\"%s\" already exists at (%i, %i) [%i]\n", getStructureAt(curpos+x)->getType()->getTName(), tx, ty, curpos+x);
                        return false;
                    }
                    if (0 != (unitandstructmat[curpos+x].flags & US_IS_UNIT) || 0 != (unitandstructmat[curpos+x].flags & US_IS_AIRUNIT)) {
                        Uint16 tx, ty;
                        p::ccmap->translateFromPos(curpos+x, &tx, &ty);
                        logger->error("Unit(s) already exists at cell (%i, %i) %i\n", tx, ty, curpos+x);
                        return false;
                    }
                }
            }
        }

        // Redo the loop knowing that it's clear.  Saves having to backtrack if
        // we have to abort the placing.
        bool setlr = false;
        curpos = cellpos + p::ccmap->getWidth()*(type->getYsize());
        for (y = type->getYsize()-1; y>=0; --y) {
            curpos -= p::ccmap->getWidth();
            for (x = type->getXsize()-1; x>=0; --x) {
                if (type->isBlocked(y*type->getXsize()+x)) {

					if (structnum > structurepool.size()){
						logger->error ("%s line %i: ERROR create structure, structnum = %i, size = %i\n", __FILE__, __LINE__, structnum, structurepool.size());
						structnum = structurepool.size();
					}
//					unitandstructmat[curpos+x].flags = US_IS_STRUCTURE;
//					unitandstructmat[curpos+x].structurenumb = structnum;
					if (type->isBlocked(y*type->getXsize()+x) == 2) {
						unitandstructmat[curpos+x].flags = US_IS_STRUCTURE|US_HIGH_COST;
						unitandstructmat[curpos+x].structurenumb = structnum;
					}else{
						//unitandstructmat[curpos+x] = US_IS_STRUCTURE|structnum;
						unitandstructmat[curpos+x].flags = US_IS_STRUCTURE;
						unitandstructmat[curpos+x].structurenumb = structnum;
					}

                    if (!setlr) {
                        unitandstructmat[curpos+x].flags |= US_STR_LOWER_RIGHT;
                        setlr = true;
                    }
                }
#if 1
		// Don't place worn down ground under a naval yard!!
		if (!type->isWaterBound())
		{
			// Start of new worn down ground code
			if (bib1 == 0)
			{
				if (p::ccmap->SnowTheme ())
					bib1 = pc::imgcache->loadImage("bib1.sno", -1);
				else
					bib1 = pc::imgcache->loadImage("bib1.tem", -1);
			}
			if (bib2 == 0){
				if (p::ccmap->SnowTheme ())
					bib2 = pc::imgcache->loadImage("bib2.sno", -1);
						else
							bib2 = pc::imgcache->loadImage("bib2.tem", -1);
					}

			if (bib3 == 0){
						if (p::ccmap->SnowTheme ())
							bib3 = pc::imgcache->loadImage("bib3.sno", -1);
						else
							bib3 = pc::imgcache->loadImage("bib3.tem", -1);
			}

			if (y < 2){
				switch (type->getXsize()){
					case 4:
						frame = x+y*4;
						pos = curpos+x+(type->getYsize()-1)*p::ccmap->getWidth();
						//printf ("%s line %i: Case %i, frame = %i\n", __FILE__, __LINE__, type->getYsize(), frame);
						p::ccmap->setTerrainOverlay( pos, bib1, frame );
						break;
					case 3:
						frame = x+y*3;
						pos = curpos+x+(type->getYsize()-1)*p::ccmap->getWidth();
						//printf ("%s line %i: Case %i, frame = %i\n", __FILE__, __LINE__, type->getYsize(), frame);
						p::ccmap->setTerrainOverlay( pos, bib2, frame );
						break;
					case 2:
						frame = x+y*2;
						pos = curpos+x+(type->getYsize()-1)*p::ccmap->getWidth();
						//printf ("%s line %i: Case %i, frame = %i\n", __FILE__, __LINE__, type->getYsize(), frame);
						p::ccmap->setTerrainOverlay( pos, bib3, frame );
						break;
				}
			}

			}
#endif
            }
        }
    } // end if (wall)

    // Create a new one
    st = new Structure(type, cellpos, owner, health, facing, trigger_name);
    st->referTo();
    st->setStructnum(structnum);
    if (structnum == structurepool.size()) {
        structurepool.push_back(st);
    } else {
        structurepool[structnum] = st;
        --numdeletedstruct;
    }

    // Check if it's wall
    if(type->isWall())
    {
        // update the wall-images
    	updateWalls(st, true, p::ccmap);
    } else {
        if (makeanim) {
        	// run "build" anim at const speed
            st->runAnim(0);
        } else {
            if( (type->getAnimInfo().animtype == 1) || (type->getAnimInfo().animtype == 4) ) {
            	// looped anim building ???
                st->runAnim(1);
            }
        }
    }
    return true;
}

/**
 * Creates a unit
 */
Unit* UnitAndStructurePool::createUnit(const char *typen, Uint16 cellpos, Uint8 subpos, Uint8 owner, Uint16 health, Uint8 facing, Uint8 action, string trigger_name)
{
	// Check that the unit is in the map
	if (cellpos >= p::ccmap->getSize()){
		return 0;
	}

	// Get the type of the Unit
    UnitType* type = getUnitTypeByName(typen);
    if (0 == type) {
        logger->error("Invalid type name: \"%s\"\n", typen);
        return 0;
    }

    // Return the Unit if the unit creation is ok
    // if no then return NULL (0)
    return createUnit(type, cellpos, subpos, owner, health, facing, action, trigger_name);

}

/**
 * Create a unit in the map
 */
Unit* UnitAndStructurePool::createUnit(UnitType* type, Uint16 cellpos, Uint8 subpos, Uint8 owner, Uint16 health, Uint8 facing, Uint8 action, string trigger_name)
{
	if (cellpos >= p::ccmap->getSize()){
		return false;
	}

    Uint32 unitnum = 0;

    if (cellpos > (p::ccmap->getWidth() * p::ccmap->getHeight())) {
        logger->error("Attempted to create a %s at %i, outside map.\n",
                type->getTName(), cellpos);
        return false;
    }
    if (getStructureAt(cellpos) != NULL && (unitandstructmat[cellpos].flags&(US_HIGH_COST) == 0)) {
        logger->error("Cell %i already occupied by structure (%s).\n", cellpos,
                getStructureAt(cellpos)->getType()->getTName());
        return false;
    }
    if (getUnitAt(cellpos,subpos) != 0) {
    	// @todo appear next !!! (next subpos)
        logger->error("Cell/subpos already occupied by %s\n", getUnitAt(cellpos,
                    subpos)->getType()->getTName());
        return false;
    }

    // Reuse an expired unitnum.
    if (numdeletedunit > 0)  {
        for (unitnum = 0; unitnum < unitpool.size(); ++unitnum) {
            if(0 == unitpool[unitnum])
                break;
        }
    } else {
        unitnum = unitpool.size();
    }

    InfantryGroup *group = 0;

    if (type->isInfantry()) {
        if (unitandstructmat[cellpos].flags&US_IS_UNIT) {
            group = unitpool[unitandstructmat[cellpos].unitnumb]->getInfantryGroup();
        } else {
            group = new InfantryGroup;
        }
    }

    Unit* un = new Unit(type, cellpos, subpos, group, owner, health, facing, action, trigger_name);

	if (unitnum > unitpool.size()){
		logger->error ("%s line %i: CRASHBUG, hmm this should not be possible, unit numb overflow, unitnum = %i, size = %i\n", __FILE__, __LINE__, unitnum, unitpool.size());
		unitnum = unitpool.size();
	}

	if (!un->IsAirBound()){
		unitandstructmat[cellpos].unitnumb = unitnum;
		unitandstructmat[cellpos].flags |= US_UNIT_LOWER_RIGHT|US_IS_UNIT;
		// Check if we need to handle a trigger
		if (unitandstructmat[cellpos].flags & US_CELL_HAS_TRIGGER){
			HandleGlobalTrigger(TRIGGER_EVENT_ZONE_ENTRY, cellpos);
		}
	}else{
		unitandstructmat[cellpos].airunitnumb = unitnum;
		unitandstructmat[cellpos].flags |= US_AIRUNIT_LOWER_RIGHT|US_IS_AIRUNIT;

		// Check if we need to handle a trigger
		if (unitandstructmat[cellpos].flags & US_CELL_HAS_TRIGGER)
			HandleGlobalTrigger(TRIGGER_EVENT_ZONE_ENTRY, cellpos);
		printf ("%s line %i: Air born unit created :)\n", __FILE__, __LINE__);
	}

    /* curpos = cellpos;
     for( y = 0; y < type->getSize(); y++ ){
      for(x = 0; x < type->getSize(); x++){
       unitandstructmat[curpos+x] |= US_IS_UNIT;
      }
      curpos += mapwidth;
     }*/
    un->setUnitnum(unitnum);
    if (unitnum == unitpool.size()) {
        unitpool.push_back(un);
    } else {
        unitpool[unitnum] = un;
        numdeletedunit--;
    }

    return un;
}

/**
 * Place the flag of a Cell Trigger in a cell
 */
bool UnitAndStructurePool::createCellTrigger(Uint32 cellpos)
{
	if (cellpos >= p::ccmap->getSize()){
		return false;
	}

	unitandstructmat[cellpos].flags |= US_CELL_HAS_TRIGGER;

	return true;
}

/**
 *
 */
bool UnitAndStructurePool::spawnUnit(const char* typen, Uint8 owner) {

    UnitType* type = getUnitTypeByName(typen);
    if (0 == type) {
        logger->error("%s line %i: Invalid type name: \"%s\"\n", __FILE__, __LINE__, typen);
        return false;
    }
    return spawnUnit(type, owner);
}

/**
 * First part of spawn unit (this starts the door animation if needed)
 */
bool UnitAndStructurePool::spawnUnit(UnitType* type, Uint8 owner)
{
	bool returnval = true;
    Player* player = p::ppool->getPlayer(owner);
    assert(player != 0);
    Structure* tmpstruct = player->getPrimary(type);
    Uint16 pos = 0xffff;
    Uint8 subpos = 0;

    if (0 != tmpstruct) {
        pos = tmpstruct->getFreePos(&subpos, type->isInfantry());
    } else {
        logger->error("No primary building set for %s\n", type->getTName());
        return false;
    }

    if (pos != 0xffff) {
        /// @todo run weap animation (let unit exit weap)
 		if(!type->isInfantry() && !tmpstruct->getType()->hasAirBoundUnits() && !tmpstruct->getType()->isWaterBound()){
			/// CreateUnitAnimation eventually calls createUnit
			returnval = tmpstruct->CreateUnitAnimation(type, owner);
		}else{
			tmpstruct->runAnim(1);
			Unit* unit = createUnit(type, pos, subpos, owner, FULLHEALTH, 0, 0, "None");
			if (unit != 0)
				returnval = true;
		}

		return returnval;
    } else {
        logger->error("%s line %i: No free position for %s\n", __FILE__, __LINE__, type->getTName());
    }
    return false;
}

Unit* UnitAndStructurePool::getUnitAt(Uint32 cell, Uint8 subcell)
{
	Unit *un = 0;
	if (cell >= p::ccmap->getSize()){
		return 0;
	}
    if (unitandstructmat.size() <= cell){
		return 0;
	}
    if( !(unitandstructmat[cell].flags & (US_IS_UNIT|US_IS_AIRUNIT))){
        return 0;
    }
	if (unitandstructmat[cell].flags & US_IS_AIRUNIT){
    	un = getUnit(unitandstructmat[cell].airunitnumb); //unitpool[unitandstructmat[cell]&0xffff];
	}
	if (unitandstructmat[cell].flags & US_IS_UNIT){
    	un = getUnit(unitandstructmat[cell].unitnumb); //unitpool[unitandstructmat[cell]&0xffff];
	}
	if (un == 0){
		return 0;
	}
    if (((UnitType *)un->getType())->isInfantry() ){
        return un->getInfantryGroup()->UnitAt(subcell);
    }
    // Return the ref of the Unit
    return un;
}

/*
 * This func is not exact for infantry!!
 */
Unit* UnitAndStructurePool::getUnitAt(Uint32 cell)
{
	if (cell >= p::ccmap->getSize()){
		return 0;
	}

    Unit *un = 0;
    if( !(unitandstructmat[cell].flags & (US_IS_UNIT|US_IS_AIRUNIT))){
        return 0;
    }
	if (unitandstructmat[cell].flags & US_IS_UNIT){
    	un = getUnit(unitandstructmat[cell].unitnumb); //unitpool[unitandstructmat[cell]&0xffff];
	}

	if (unitandstructmat[cell].flags & US_IS_AIRUNIT){
    	un = getUnit(unitandstructmat[cell].airunitnumb); //unitpool[unitandstructmat[cell]&0xffff];
	}

    return un;
}

Uint32 UnitAndStructurePool::getNumbUnits()
{
	return unitpool.size();
}

Unit* UnitAndStructurePool::getUnit(Uint32 num)
{
	if (num < unitpool.size())
	{
		return unitpool[num];
	}
	return 0;
}

/**
 * Retrieves the structure at a given cell (NULL if there is no structure).
 *
 * @param cell the cell to be examined for structures
 * @param wall if false, will return NULL if a wall is found in the cell
 * @returns a pointer to the Structure if found
 */
Structure *UnitAndStructurePool::getStructureAt(Uint32 cell, bool wall)
{
	if (cell >= p::ccmap->getSize()){
		return 0;
	}

    if( !(unitandstructmat[cell].flags & US_IS_STRUCTURE) &&
            !(wall && (unitandstructmat[cell].flags&US_IS_WALL)))
    {
        return 0;
    }

    return structurepool[unitandstructmat[cell].structurenumb];
}

Structure* UnitAndStructurePool::getStructure(Uint32 num)
{
	if (num < structurepool.size()){
		return structurepool[num];
	}
	return 0;
}

Uint32 UnitAndStructurePool::getNumbStructures()
{
	return structurepool.size();
}

bool UnitAndStructurePool::cellOccupied (Uint32 cell)
{
	if (cell >= p::ccmap->getSize())
		return true;

	if (unitandstructmat[cell].flags & (US_IS_STRUCTURE|US_IS_WALL|US_IS_UNIT|US_IS_AIRUNIT))
		return true;

	return false;
}

/**
 * Retrieves ground units from a given cell like getUnitAt and
 * getStructureAt except works for either.
 *
 * @returns a pointer to the UnitOrStructure at cell.
 * @param cell The cell to be examined.
 * @param subcell The subposition of the cell to be examined (only valid for infantry).  Bitwise or with 128 to get the nearest infantry unit to that subpos.
 * @return the Unit at the position
 */
Unit* UnitAndStructurePool::getGroundUnitAt( Uint32 cell, Uint8 subcell)
{
	Unit* un = 0;

	if (cell >= p::ccmap->getSize())
	{
		// Return NULL
		return 0;
	}

	if (unitandstructmat[cell].flags & US_IS_UNIT)
	{
		un = getUnit(unitandstructmat[cell].unitnumb);
		if (un != 0)
		{
			if (dynamic_cast<UnitType *>(un->getType())->isInfantry() )
			{
				if (subcell == 0x80){
					return un;
				} else {
					return un->getInfantryGroup()->UnitAt(subcell);
				}
			}
        	return un;
		}
	}
	return 0;
}

/**
 * Retrieves units or structures from a given cell like getUnitAt and
 * getStructureAt except works for either.
 *
 * @returns a pointer to the UnitOrStructure at cell.
 * @param cell The cell to be examined.
 * @param subcell The subposition of the cell to be examined (only valid for infantry).  Bitwise or with 128 to get the nearest infantry unit to that subpos.
 * @return pointer to the Unit which is at the cell position
 */
Unit* UnitAndStructurePool::getFlyingAt(Uint32 cell, Uint8 subcell)
{
	// Test if the cell is in the map
	if (cell >= p::ccmap->getSize())
	{
		// If not return NULL
		return 0;
	}

	if (unitandstructmat[cell].flags & US_IS_AIRUNIT)
	{
		Unit* un = getUnit(unitandstructmat[cell].airunitnumb);
		if (un != 0)
		{
			// If it's infantry return the Unit at the sub position
    		if(dynamic_cast<UnitType *>(un->getType())->isInfantry())
        		return un->getInfantryGroup()->UnitAt(subcell);
			return (un);
		}
	}

	// Return NULL if not found
	return 0;
}

/**
 * Retrieves units or structures from a given cell like getUnitAt and
 * getStructureAt except works for either.
 *
 * @returns a pointer to the UnitOrStructure at cell.
 * @param cell The cell to be examined.
 * @param subcell The subposition of the cell to be examined (only valid for infantry).  Bitwise or with 128 to get the nearest infantry unit to that subpos.
 * @param wall Whether or not to check for walls as well.
 */
Structure* UnitAndStructurePool::getStructureAt(Uint32 cell, Uint8 subcell, bool wall)
{
	Structure *str = 0;

	if (cell >= p::ccmap->getSize())
		return 0;

	if (unitandstructmat[cell].flags & (US_IS_STRUCTURE|(wall?US_IS_WALL:0)) ) {
		str = getStructure(unitandstructmat[cell].structurenumb);
			return (str);
	}
	return 0;
}

/**
 * Retrieves infantry group from a given cell
 */
InfantryGroup* UnitAndStructurePool::getInfantryGroupAt(Uint32 cell)
{
    if (unitandstructmat[cell].flags & US_IS_UNIT && cell < p::ccmap->getSize()) {
        Unit* un = getUnit(unitandstructmat[cell].unitnumb);
        if( ((UnitType *)un->getType())->isInfantry() ) {
            return un->getInfantryGroup();
        }
    }
    return 0;
}

Uint16 UnitAndStructurePool::getSelected(Uint32 pos)
{
    int i = 0;
    Uint16 selected = 0;

	if (pos >= p::ccmap->getSize())
		return 0;

    if( !(unitandstructmat[pos].flags&(US_UNIT_LOWER_RIGHT |US_STR_LOWER_RIGHT|US_AIRUNIT_LOWER_RIGHT)) )
        return 0;

    if( unitandstructmat[pos].flags&US_IS_UNIT ) {
		if ( unitpool.size () > (unitandstructmat[pos].unitnumb)){
			if(((UnitType *)getUnit (unitandstructmat[pos].unitnumb)->getType())->isInfantry()) {  //unitpool[unitandstructmat[pos]&0xffff]->getType())->isInfantry()) {
				selected = 0xff00;
				for( i = 0; i < 5; i++ )
					if( !unitpool[unitandstructmat[pos].unitnumb]->getInfantryGroup()->IsClear(i) )
						if(unitpool[unitandstructmat[pos].unitnumb]->getInfantryGroup()->UnitAt(i)->isSelected() )
							selected |= 1<<i;
				return selected;

			} else if(unitpool[unitandstructmat[pos].unitnumb]->isSelected())
				return (1<<8) | 1;
		}else{
			logger->error ("%s line %i: Tried to get unit outside of unit-pool, up size = %i, pos = %i\n", __FILE__, __LINE__, unitpool.size (), unitandstructmat[pos].unitnumb);
		}
    }


    if( unitandstructmat[pos].flags&US_IS_AIRUNIT ) {
		if ( unitpool.size () > (unitandstructmat[pos].airunitnumb)){
			if(((UnitType *)getUnit (unitandstructmat[pos].airunitnumb)->getType())->isInfantry()) {  //unitpool[unitandstructmat[pos]&0xffff]->getType())->isInfantry()) {
				selected = 0xff00;
				for( i = 0; i < 5; i++ )
					if( !unitpool[unitandstructmat[pos].airunitnumb]->getInfantryGroup()->IsClear(i) )
						if(unitpool[unitandstructmat[pos].airunitnumb]->getInfantryGroup()->UnitAt(i)->isSelected() )
							selected |= 1<<i;
				return selected;

			} else if(unitpool[unitandstructmat[pos].airunitnumb]->isSelected())
				return (1<<8) | 1;
		}else{
			logger->error ("%s line %i: Tried to get unit outside of unit-pool, up size = %i, pos = %i\n", __FILE__, __LINE__, unitpool.size (), unitandstructmat[pos].airunitnumb);
		}
    }


    if( unitandstructmat[pos].flags&US_IS_STRUCTURE ) {
		if (structurepool.size() > (unitandstructmat[pos].structurenumb) ){
			if(structurepool[unitandstructmat[pos].structurenumb]->isSelected())
				return (((StructureType *)structurepool[unitandstructmat[pos].structurenumb]->getType())->getXsize()<<8) | 1;
		}else{
			logger->error ("%s line %i: Tried to get structure outside of structure-pool, sp size = %i, pos = %i\n", __FILE__, __LINE__, structurepool.size(), unitandstructmat[pos].structurenumb);
		}
	}
    return 0;
}

/**
 * Called by MoveAnimEvent before moving to set up the unitandstructure matrix
 *
 * @note If there are no Unit in the destination cell the BlockingUnit pointer is NULL
 *
 * @param un the unit about to move
 * @param dir the direction in which the unit is to move
 * @param xmod the modifier used to adjust the xoffset (set in this function)
 * @param ymod the modifier used to adjust the yoffset (set in this function)
 * @param BlockingUnit pointer to the blocking unit (The methods update the pointer to the Unit which is in the destination cell)
 * @return the cell of the new position
 */
Uint16 UnitAndStructurePool::preMove(Unit *un, Uint8 dir, Sint8 *xmod, Sint8 *ymod, Unit **BlockingUnit)
{
	Uint16 newpos = 0;
	Sint8 unitmod = ((UnitType *)un->getType())->getMoveMod();

	*BlockingUnit = 0;

	switch(dir) {
		case m_up:
			newpos = un->getPos()-p::ccmap->getWidth();
			*xmod = 0;
			*ymod = -unitmod;
			break;
		case m_upright:
			newpos = un->getPos()-p::ccmap->getWidth()+1;
			*xmod = unitmod;
			*ymod = -unitmod;
			break;
		case m_right:
			newpos = un->getPos()+1;
			*xmod = unitmod;
			*ymod = 0;
			break;
		case m_downright:
			newpos = un->getPos()+p::ccmap->getWidth()+1;
			*xmod = unitmod;
			*ymod = unitmod;
			break;
		case m_down:
			newpos = un->getPos()+p::ccmap->getWidth();
			*xmod = 0;
			*ymod = unitmod;
			break;
		case m_downleft:
			newpos = un->getPos()+p::ccmap->getWidth()-1;
			*xmod = -unitmod;
			*ymod = unitmod;
			break;
		case m_left:
			newpos = un->getPos()-1;
			*xmod = -unitmod;
			*ymod = 0;
			break;
		case m_upleft:
			newpos = un->getPos()-p::ccmap->getWidth()-1;
			*xmod = -unitmod;
			*ymod = -unitmod;
			break;
		default:
			return 0xffff;
			break;
	}

	// this is needed since tiles in fog have cost 1 in the pathfinder
	if( p::ccmap->getCost(newpos, un) > 5) {
		return 0xffff;
	}

	if (unitandstructmat[newpos].flags&(US_HIGH_COST) != 0)
		printf ("%s line %i: strange tile with us high cost ??\n", __FILE__, __LINE__);

    /* if an infantry's position got updated */
	if( unitandstructmat[newpos].flags&(US_IS_WALL|US_IS_STRUCTURE|US_IS_UNIT|US_MOVING_HERE) && !un->IsAirBound ()){
//	if(( unitandstructmat[newpos].flags&(US_IS_WALL|US_IS_STRUCTURE|US_IS_UNIT|US_MOVING_HERE)) && ((unitandstructmat[newpos].flags&(US_HIGH_COST)) == 0 || ((UnitType *)un->getType())->isInfantry())) {
		if( !((UnitType *)un->getType())->isInfantry()  && (unitandstructmat[newpos].flags&(US_HIGH_COST)) == 0 ) {
			// remove this later when code for moving over walls and infantry is done
			*BlockingUnit = unitpool[unitandstructmat[newpos].unitnumb];
			return 0xffff;
		}

		if (unitandstructmat[newpos].flags&(US_IS_WALL))
		{
			// @todo check for tracked and wall type to allow over running some walls
			return 0xffff;
		}

		if (unitandstructmat[newpos].flags&(US_IS_STRUCTURE) && (unitandstructmat[newpos].flags&(US_IS_UNIT) == 0))
		{
			return 0xffff;
		}

		if( unitandstructmat[newpos].flags&US_IS_UNIT ) {

			if( !((UnitType *)unitpool[unitandstructmat[newpos].unitnumb]->getType())->isInfantry() )
			{
				// @todo infantry squishing, check moving unit's type for tracked and accept
				//  this cell as valid if so.
				*BlockingUnit = unitpool[unitandstructmat[newpos].unitnumb];
				return 0xffff;
			}

			if ( ((Unit* )unitpool[unitandstructmat[newpos].unitnumb])->getOwner() != un->getOwner()) {
				// do not allow units of different sides to occupy same cell
				// this is not allowed because apart from looking weird,
				// area of effect weapons (e.g. flame thrower) will take
				// themselves with their target(s).
				*BlockingUnit = unitpool[unitandstructmat[newpos].unitnumb];
				return 0xffff;
			}

			if( (((unitandstructmat[newpos].flags&US_MOVING_HERE)>>24) + unitpool[unitandstructmat[newpos].unitnumb]->getInfantryGroup()->GetNumInfantry()) >= 5){
				*BlockingUnit = unitpool[unitandstructmat[newpos].unitnumb];
				return 0xffff;
			}

		} else if(((unitandstructmat[newpos].flags&US_MOVING_HERE)>>24) >= 5){
			// more than five infantry in current cell
			if (unitandstructmat[newpos].unitnumb < unitpool.size())
				*BlockingUnit = unitpool[unitandstructmat[newpos].unitnumb];
			return 0xffff;
		}
	}

	if( unitandstructmat[newpos].flags&(US_IS_AIRUNIT|US_AIR_MOVING_HERE)){
		if (un->IsAirBound ()){
			if (unitandstructmat[newpos].flags&(US_IS_AIRUNIT))
				*BlockingUnit = unitpool[unitandstructmat[newpos].airunitnumb];
			return 0xffff;
		}
	}

	if( ((UnitType *)un->getType())->isInfantry() ){
		//logger->error ("%s line %i: Premove1 = %x\n", __FILE__, __LINE__, unitandstructmat[newpos]);
		unitandstructmat[newpos].flags += 0x1000000;
		//logger->error ("%s line %i: Premove2 = %x\n", __FILE__, __LINE__, unitandstructmat[newpos]);
	}
	else
	{
		if (!un->IsAirBound ())
		{
			unitandstructmat[newpos].flags |= US_MOVING_HERE;

			// Check if we need to handle a trigger
			if (unitandstructmat[newpos].flags & US_CELL_HAS_TRIGGER)
				HandleGlobalTrigger ( TRIGGER_EVENT_ZONE_ENTRY, newpos );
		} else {
			unitandstructmat[newpos].flags |= US_AIR_MOVING_HERE;

			// Check if we need to handle a trigger
			if (unitandstructmat[newpos].flags & US_CELL_HAS_TRIGGER)
				HandleGlobalTrigger ( TRIGGER_EVENT_ZONE_ENTRY, newpos );
		}
	}
	return newpos;
}

/**
 * Called when a unit has moved into a new cell
 *
 * @param un the unit
 * @param newpos the cell into which the unit has moved
 * @returns the new sub position for the unit (only needed for infantry)
 */
Uint8 UnitAndStructurePool::postMove(Unit *un, Uint16 newpos)
{
	Uint8 subpos = 0;

	if (newpos >= p::ccmap->getSize())
		return 0;

	subpos = unhideUnit(un,newpos,false);

	// Check if the move was oke
	if (subpos > 5)
		return subpos;

	p::ppool->getPlayer(un->getOwner())->movedUnit(un->getPos(), newpos, un->getType()->getSight());

	/** @todo: Airborne infantry not supported jet **/
	if( ((UnitType *)un->getType())->isInfantry() ) {
		if (unitandstructmat[newpos].flags>0x1000000)
			unitandstructmat[newpos].flags -= 0x1000000;
		else
			logger->error ("%s line %i: Post move error\n", __FILE__, __LINE__);
	} else {
		// clear values from old position
		if (!un->IsAirBound())
			unitandstructmat[un->getPos()].flags &= ~(US_UNIT_LOWER_RIGHT|US_IS_UNIT);
		else
			unitandstructmat[un->getPos()].flags &= ~(US_AIRUNIT_LOWER_RIGHT|US_IS_AIRUNIT);
	}
	return subpos;
}

/**
 * Resets the US_MOVING_HERE flag of a cell when the unit stops
 *  before it reaches its destination.
 *
 *  @param un the unit stopping.
 *  @param pos the position to which the unit was moving before stopping.
 */
void UnitAndStructurePool::abortMove(Unit* un, Uint32 pos)
{
    if( !(unitandstructmat[pos].flags & US_MOVING_HERE|US_AIR_MOVING_HERE) ) {
        return;
    }
    if (((UnitType *)un->getType())->isInfantry()) {
		if (unitandstructmat[pos].flags>=0x1000000)
        	unitandstructmat[pos].flags -= 0x1000000;
		else
			logger->error ("%s line %i: abortMove error, unitandstructmat[pos] = %x\n", __FILE__, __LINE__, unitandstructmat[pos].flags);
    } else {
		if (!un->IsAirBound())
			unitandstructmat[pos].flags &= ~(US_MOVING_HERE);
		else
			unitandstructmat[pos].flags &= ~(US_AIR_MOVING_HERE);
    }
}

/**
 * Searches the UnitType pool for a unit type with a given name.
 *  if the type can not be found, it is read in from units.ini
 *
 * @param unitname the name of the unit to retrieve
 * @returns pointer to the UnitType value
 */
UnitType* UnitAndStructurePool::getUnitTypeByName(const char* unitname)
{
    map<string, Uint16>::iterator typeentry;
    UnitType* type;
    Uint16 typenum;
    string uname = (string)unitname;

    //transform(uname.begin(),uname.end(), uname.begin(), toupper);

    typeentry = unitname2typenum.find(uname);

    if( typeentry != unitname2typenum.end() ) {
        typenum = typeentry->second;
        type = unittypepool[typenum];
    } else {
    	// Check that there are a section for the unitType
    	if (unitini->isSection(unitname) == true)
    	{
    		typenum = unittypepool.size();
    		type = new UnitType(uname.c_str(), unitini);
    		unittypepool.push_back(type);
    		unitname2typenum[uname] = typenum;
    	}
    	else
    	{
    		return 0;
    	}
    }
    if (type->isValid()) {
        return type;
    }
    return 0;
}

/**
 * Same as getUnitTypeByName but for structures (and the ini file is structure.ini)
 *
 * @param structname the name of the structure to retrieve (e.g. FACT or PROC)
 * @return pointer to the StructureType value
 */
StructureType* UnitAndStructurePool::getStructureTypeByName(const char* structname)
{
	StructureType* type; // Type to return
	Uint16 typenum;
    map<string, Uint16>::iterator typeentry;


    string sname = (string)structname;

    // UPPER sname
    transform(sname.begin(),sname.end(),sname.begin(),toupper);

    typeentry = structname2typenum.find(sname);

    if( typeentry != structname2typenum.end() ) {
        typenum = typeentry->second;
        type = structuretypepool[typenum];
    } else {
        typenum = structuretypepool.size();
        type = new StructureType(structname, structini, artini, theaterext);
        structuretypepool.push_back(type);
        structname2typenum[sname] = typenum;
    }
    if (type->isValid()) {
    	return type;
    }

    // Return error (NULL)
    return 0;
}

/**
 *
 */
UnitOrStructureType* UnitAndStructurePool::getTypeByName(const char* typen)
{
    UnitOrStructureType* retval;
    retval = getUnitTypeByName(typen);
    if (0 == retval) {
        return getStructureTypeByName(typen);
    }
    return retval;
}

/**
 *
 */
bool UnitAndStructurePool::freeTile(Uint16 pos) const
{
	return (unitandstructmat[pos].flags&0x70000000)==0;
}

/**
 *
 */
Uint16 UnitAndStructurePool::getTileCost(Uint16 pos, Unit* excpUn) const
{
    bool AirBound = false;
    bool WaterBound = false;
    Unit* un = 0;
    Structure* str = 0;

	// Don't allow excpUn to be NULL
    if (excpUn == 0)
    {
		//printf ("%s line %i: Warning excpUn == NULL is not allowed in this function anymore!!\n", __FILE__, __LINE__);
        return getTileCost(pos);
	}

    // Check if the position is in the map
	if (pos >= p::ccmap->getSize())
	{
		return 0xffff;
    }

	if (excpUn != 0)
	{
		WaterBound	= excpUn->IsWaterBound ();
		AirBound	= excpUn->IsAirBound();
	}

	if (unitandstructmat[pos].flags & US_IS_UNIT)
	{
		un = unitpool[unitandstructmat[pos].unitnumb];

		if (un == excpUn)
			return 0;

		if (AirBound)
		{
			if (un->IsAirBound())
			{
				return 0xffff;
			} else {
				return 0;
			}
		}

		/// Remove this to prevent getting other problems with movign units
		//if( un->getOwner() == costcalcowner )
		//	return 4;

		return 0xfff0;
	}

	if (unitandstructmat[pos].flags & US_IS_AIRUNIT)
	{
		un = unitpool[unitandstructmat[pos].airunitnumb];
		if (un == excpUn)
			return 0;


		if (un->IsAirBound() == excpUn->IsAirBound())
			return 0xffff;
		else
			return 0;

		if( un->getOwner() == costcalcowner )
			return 4;
		return 0xfff0;
	}


	if (tileAboutToBeUsed(pos))
	{
		if (excpUn->getType()->isInfantry())
			return 2;
		else
			return 0xffff;
	}

    if (unitandstructmat[pos].flags & (US_IS_WALL|US_IS_STRUCTURE))
    {

		str = structurepool[unitandstructmat[pos].structurenumb];

		if (str->getOwner() != excpUn->getOwner())
			return 0xfff0;

		// Don't allow driving on a unit on a structure
    	if( unitandstructmat[pos].flags & US_IS_UNIT ){
			logger->error ("%s line %i: already a unit here\n", __FILE__, __LINE__);
			return 0xfff0;
		}
		if (excpUn->IsAirBound())
			return 0;

		if (unitandstructmat[pos].flags & US_HIGH_COST && !((UnitType *)excpUn->getType())->isInfantry()){
			//printf ("Return half blocked\n");
        	return 0;
		}
		return 0xfff0;
	}

    return 0;
}

Uint16 UnitAndStructurePool::getTileCost(Uint16 pos) const
{
	Unit		*un = 0;
	Structure	*str = 0;

	// Check if the pos is in the map
	if (pos >= p::ccmap->getSize())
	{
		return 0xffff;
	}


	if (unitandstructmat[pos].flags & US_IS_UNIT)
	{

		un = unitpool[unitandstructmat[pos].unitnumb];

		/// Remove this to prevent getting other problems with movign units
		//if( un->getOwner() == costcalcowner )
		//	return 4;

		return 0xfff0;
	}

	if (unitandstructmat[pos].flags & US_IS_AIRUNIT)
	{

		un = unitpool[unitandstructmat[pos].airunitnumb];

		if( un->getOwner() == costcalcowner )
			return 4;

		return 0xffff;
	}


    if (tileAboutToBeUsed(pos))
    {
    	return 0xfff0;
    }

    if (unitandstructmat[pos].flags & (US_IS_WALL|US_IS_STRUCTURE))
    {

		str = structurepool[unitandstructmat[pos].structurenumb];

		// Don't allow driving on a unit on a structure
    	if( unitandstructmat[pos].flags & US_IS_UNIT ){
			logger->error ("%s line %i: already a unit here\n", __FILE__, __LINE__);
			return 0xfff0;
		}
		if (unitandstructmat[pos].flags & US_HIGH_COST){
			return 0;
		}else
			return 0xfff0;
		return 0xfff0;
	}
    return 0;
}

/**
 *
 */
bool UnitAndStructurePool::tileAboutToBeUsed(Uint16 pos) const
{
    return (unitandstructmat[pos].flags & (US_MOVING_HERE|US_AIR_MOVING_HERE))!=0;
}

/**
 *
 */
void UnitAndStructurePool::setCostCalcOwnerAndType(Uint8 owner, Uint8 type)
{
	costcalcowner = owner;
	costcalctype = type;
}

/**
 * Removes a unit from the map
 *
 * @param un The unit to remove
 */
void UnitAndStructurePool::removeUnit(Unit *un)
{
//	printf ("%s line %i: Remove unit\n", __FILE__, __LINE__);
    int i;
    unitpool[un->getNum()] = 0;
    if( ((UnitType *)un->getType())->isInfantry() ) {
//		printf ("%s line %i: Remove infantry!!\n", __FILE__, __LINE__);
        InfantryGroup *infgrp = un->getInfantryGroup();
        infgrp->RemoveInfantry(un->getSubpos());
        if (infgrp->GetNumInfantry() == 0) {
            //delete infgrp;
            unitandstructmat[un->getPos()].flags &= ~(US_UNIT_LOWER_RIGHT|US_IS_UNIT);
        } else if ((unitandstructmat[un->getPos()].unitnumb) == un->getNum()) {
            for (i = 0; i < 5; i++) {
                if(!infgrp->IsClear(i)) {
                    //unitandstructmat[un->getPos()]&=~0xffff;
                    unitandstructmat[un->getPos()].unitnumb = infgrp->UnitAt(i)->getNum();
                    break;
                }
            }
        }
        un->setInfantryGroup(0);
    } else {
//		printf ("%s line %i: Remove normal unit!!\n", __FILE__, __LINE__);
		if (!un->IsAirBound())
        	unitandstructmat[un->getPos()].flags &= ~(US_UNIT_LOWER_RIGHT|US_IS_UNIT);
		else
        	unitandstructmat[un->getPos()].flags &= ~(US_AIRUNIT_LOWER_RIGHT|US_IS_AIRUNIT);
    }
    numdeletedunit++;
    deleted_unitorstruct = true;
    un->remove();
    //if numdeletedunit > some_value then pack the unitpool
}

/**
 * Removes a structure from the map
 *
 * @param st Structure to remove
 */
void UnitAndStructurePool::removeStructure(Structure *st)
{
    Uint16 curpos, x, y;
	Uint32 pos;
	StructureType *type;

	type = st->getType();

    // unitandstructmat[st->getPos()] &= ~(US_UNIT_LOWER_RIGHT|US_IS_STRUCTURE);
    structurepool[st->getNum()] = NULL;
    curpos = st->getPos();
    if (((StructureType*)st->getType())->isWall()) {
        updateWalls(st, false, p::ccmap);
        unitandstructmat[curpos].flags &= ~(US_STR_LOWER_RIGHT|US_IS_WALL);
    } else {
        for( y = 0; y<((StructureType *)st->getType())->getYsize(); y++ ) {
            for(x = 0; x<((StructureType *)st->getType())->getXsize(); x++) {
                if( ((StructureType *)st->getType())->isBlocked(y*((StructureType *)st->getType())->getXsize()+x) ) {
                    unitandstructmat[curpos+x].flags &= ~(US_STR_LOWER_RIGHT|US_IS_STRUCTURE);
                }
				if (y < 2){
					switch (type->getYsize()){
						case 4:
							pos = curpos+x+(type->getYsize()-1)*p::ccmap->getWidth();
							p::ccmap->setTerrainOverlay( pos, 0, 0 );
							break;
						case 3:
							pos = curpos+x+(type->getYsize()-1)*p::ccmap->getWidth();
							p::ccmap->setTerrainOverlay( pos, 0, 0 );
							break;
						case 2:
							pos = curpos+x+(type->getYsize()-1)*p::ccmap->getWidth();
							p::ccmap->setTerrainOverlay( pos, 0, 0 );
							break;
					}
				}
            }
            curpos += p::ccmap->getWidth();
        }
        numdeletedstruct++; // don't count walls
    }
    deleted_unitorstruct = true;
    st->remove();
    //if numdeletedstruct > some_value then pack the structurepool
}

/**
 *
 */
bool UnitAndStructurePool::hasDeleted()
{
	bool retval = deleted_unitorstruct;
	deleted_unitorstruct = false;
	return retval;
}

/**
 * for debugging the movement code
 */
void UnitAndStructurePool::showMoves()
{
    Uint32 x;
    logger->note("Current cells have US_MOVING_HERE set:\n"
                 "cell\tvalue (US_MOVING_HERE == %u/%x)\n", US_MOVING_HERE, US_MOVING_HERE);
    for (x=0;x < (Uint32)p::ccmap->getWidth()*p::ccmap->getHeight();++x) {
        if (unitandstructmat[x].flags&US_MOVING_HERE){
			// Structures don't move :)
            logger->note("%i\t%u/%x\n",x,unitandstructmat[x].flags,unitandstructmat[x].unitnumb);
		}
    }
}

/**
 *
 */
void UnitAndStructurePool::addPrerequisites(UnitType* unittype)
{
    vector<StructureType*>* type_prereqs;
    if (unittype == 0)
        return;
    vector<char*> prereqs = unittype->getPrereqs();

    if (prereqs.empty()) {
        logger->warning("No prerequisites for unit \"%s\"\n",unittype->getTName());
        return;
    }
    if (string(prereqs[0]) == "none")
    {
        return;
    }
    for (Uint16 x=0;x<prereqs.size();++x) {
        type_prereqs = new vector<StructureType*>;
        splitORPreReqs(prereqs[x],type_prereqs);
        unit_prereqs.insert(make_pair(unittype,type_prereqs));
    }
}

/**
 * Populate prerequiste for structure
 */
void UnitAndStructurePool::addPrerequisites(StructureType* structtype)
{
    vector<StructureType*>* type_prereqs;
    if (structtype == 0)
        return;
    vector<char*> prereqs = structtype->getPrereqs();

    if (prereqs.empty()) {
        logger->warning("No prerequisites for structure \"%s\".\n"
                        "Use \"none\" if this intended.\n",structtype->getTName());
        return;
    }

    // If prerequiste are none we skip it
    if (string(prereqs[0]) == "none")
    {
        return;
    }

    for (Uint16 x=0;x<prereqs.size();++x) {
        type_prereqs = new vector<StructureType*>;
        splitORPreReqs(prereqs[x],type_prereqs);
        struct_prereqs.insert(make_pair(structtype,type_prereqs));
    }
}

/**
 *
 */
void UnitAndStructurePool::preloadUnitAndStructures(Uint8 techlevel)
{
    string secname;
    Uint8 ltech;
    Uint32 secnum;

    try {
        for(secnum = 0;;secnum++) {
            secname = unitini->readSection(secnum);
            ltech = unitini->readInt(secname.c_str(), "TechLevel", 100);
            if (ltech == 100) {
                logger->warning("No techlevel defined for unit \"%s\"\n",secname.c_str());
            } else {
                if (p::ccmap->getGameMode() == GAME_MODE_SINGLE_PLAYER) {
                    if (ltech <= techlevel) {
                        addPrerequisites(getUnitTypeByName(secname.c_str()));
                    }
                } else {
                    if (ltech < 99) {
                        addPrerequisites(getUnitTypeByName(secname.c_str()));
                    }
                }
            }
        }
    } catch(...) {}

    // Try to read the TechLevel
    try {
        for (secnum = 0;;secnum++) {
            secname = structini->readSection(secnum);
            ltech = structini->readInt(secname.c_str(),"techlevel",100);
            if (ltech == 100) {
                //logger->warning("No techlevel defined for structure \"%s\"\n",secname.c_str());
            } else {
                if (p::ccmap->getGameMode() == 0) {
                    if (ltech <= techlevel) {
                        addPrerequisites(getStructureTypeByName(secname.c_str()));
                    }
                } else {
                    if (ltech < 99) {
                        addPrerequisites(getStructureTypeByName(secname.c_str()));
                    }
                }
            }
        }
    } catch(...) {
    	logger->debug("unitAndStructurPool.preloadUnitAndStructures error in TechLevel reading [%s].\n", secname.c_str());
    }
}

/**
 *
 */
void UnitAndStructurePool::generateProductionGroups()
{
    for ( std::vector<UnitType*>::iterator ut = unittypepool.begin(); ut != unittypepool.end(); ++ut) {
        vector<StructureType*> options;
        vector<char*> nopts = (*ut)->getPrereqs();
        splitORPreReqs(nopts[0], &options);
        if (0 == options.size()) {
            continue;
        }
        Uint32 ptype = (*ut)->getType();
        (*ut)->setPType(ptype);
        for (vector<StructureType*>::iterator st = options.begin();
                st != options.end(); ++st) {
            (*st)->setPType(ptype);
        }
    }
}

/**
 * Return buildable unit from the unit and structurepool
 */
vector<const char*> UnitAndStructurePool::getBuildableUnits(Player* pl)
{
    vector<const char*> retval;
    vector<StructureType*> prereqs;
    typedef multimap<UnitType*, vector<StructureType*>* >::const_iterator I;
    pair<I,I> b;
    Uint32 x,y;
    UnitType* utype;
    bool ivalid;
    bool ovalid;
    bool buildall;

    // Get if the player can build all
    buildall = pl->canBuildAll();

    // Parse all the list
    for (x=0;x<unittypepool.size();++x) {
        utype = unittypepool[x];
        if (!utype->isValid())
            continue;
        b = unit_prereqs.equal_range(utype);
        ovalid = true;
        if (buildall) {
            if (strlen(utype->getTName()) < 5) {
                retval.push_back(utype->getTName());
            }
            continue;
        }

        //if ( ( (utype->getBuildlevel() < 99) && (p::ccmap->getGameMode() != 0)) ||
         //       (utype->getBuildlevel() <= p::ccmap->getMissionData()->buildlevel) ) {
        if (utype->getTechLevel() != -1 && utype->getTechLevel() < pl->getTechLevel())
        {

            for (I i = b.first; i != b.second; ++i) {
                // need all of these
                prereqs = *(i->second);
                ivalid = false;
                for (y = 0;y < prereqs.size() ; ++y) {
                    // need just one of these
                    if (pl->ownsStructure(prereqs[y])) {
                        ivalid = true;
                        break;
                    }
                }
                if (ovalid) {
                    ovalid = ivalid;
                }
            }

            // NEED TO REFACTOR THIS
//			if (strcmp ((char*)utype->getTName(), "E3") == 0)
//				printf ("%s line %i: Found E3 unit, ovalid = %i\n", __FILE__, __LINE__, ovalid);
            if (ovalid) {
                int playerSide;
                int curside;
                char* tmpname;
                playerSide = pl->getSide();
                for (y=0; y<utype->getOwners().size(); ++y) {
                    tmpname = utype->getOwners()[y];
                    string sider = string(tmpname);

                    // note: should avoid hardcoded side names
                    if (sider == "gdi" || sider == "GDI") {
                        curside = PS_GOOD;
                    } else if (sider == "nod" || sider == "NOD") {
                    	curside = PS_BAD;
                    } else {
                        curside = PS_NEUTRAL;
                    }

                    if (curside == (playerSide&~PS_MULTI) || (utype->isDoubleOwned())){// && pc::Config.gamemode != 0)) {
                        retval.push_back(utype->getTName());
                        break;
                    }
                }
            }
        }
    }
    return retval;
}

/**
 *
 */
vector<const char*> UnitAndStructurePool::getBuildableStructures(Player* pl)
{
    vector<const char*> retval;
    vector<StructureType*> prereqs;
    typedef multimap<StructureType*, vector<StructureType*>* >::const_iterator I;
    pair<I,I> b;
    Uint32 x,y;
    bool ivalid, ovalid, buildall;
    StructureType* stype;
    buildall = pl->canBuildAll();

    for (x=0;x<structuretypepool.size();++x) {
        stype = structuretypepool[x];
        if (!stype->isValid())
            continue;
        b = struct_prereqs.equal_range(stype);
        ovalid = true;
        if (buildall) {
            if (strlen(stype->getTName()) < 5) {
                retval.push_back(stype->getTName());
            }
            continue;
        }

        //if ( ( (stype->getBuildlevel() < 99) && (p::ccmap->getGameMode() != 0)) ||
        //        (stype->getBuildlevel() <= p::ccmap->getMissionData()->buildlevel)) {
        // because in inifiles that means we can't built it
        // @todo change Techlevel to signed because it can be -1 !!!
        if (stype->getTechLevel() != -1 && stype->getTechLevel() < pl->getTechLevel())
        {
        	for (I i = b.first; i != b.second; ++i) {
                // need all of these
                prereqs = *(i->second);
                ivalid = false;
                for (y = 0;y < prereqs.size() ; ++y) {
                    // need just one of these
                    if (pl->ownsStructure(prereqs[y])) {
                        ivalid = true;
                        break;
                    }
                }
                if (ovalid) {
                    ovalid = ivalid;
                }
            }
            if (ovalid)
            {
                int playerSide;
                int curside;
                string tmpname;
                playerSide = pl->getSide()&~PS_MULTI;
                for (y=0;y<stype->getOwners().size();++y)
                {
                    tmpname = stype->getOwners()[y];
                    // note: should avoid hardcoded side names
                    if (tmpname == "gdi" || tmpname == "GDI")
                    {
                        curside = PS_GOOD;
                    } else if (tmpname == "nod" || tmpname == "NOD")
                    {
                        curside = PS_BAD;
                    } else {
                        curside = PS_NEUTRAL;
                    }
                    if (curside == playerSide) {
                        retval.push_back(stype->getTName());
                        break;
                    }
                }
            }
        }
    }
    return retval;
}

/**
 *
 */
void UnitAndStructurePool::hideUnit(Unit* un)
{
	// @todo: Airborne infantry not supported yet
    if ( ((UnitType*)un->getType())->isInfantry() ) {
        InfantryGroup *ig = un->getInfantryGroup();
        if (ig->GetNumInfantry() == 1) {
            // old cell is now empty
            unitandstructmat[un->getPos()].flags &= ~(US_IS_UNIT|US_UNIT_LOWER_RIGHT);
            delete un->getInfantryGroup();
        } else {
            ig->RemoveInfantry(un->getSubpos());
        }
    } else {
		if (!un->IsAirBound())
        	unitandstructmat[un->getPos()].flags &= ~(US_IS_UNIT|US_UNIT_LOWER_RIGHT);
		else
        	unitandstructmat[un->getPos()].flags &= ~(US_IS_AIRUNIT|US_AIRUNIT_LOWER_RIGHT);
    }
}

/**
 *
 */
Uint8 UnitAndStructurePool::unhideUnit(Unit* un, Uint16 newpos, bool unload)
{
	Uint8 subpos = 0;
	Uint8 i;
	InfantryGroup *ig = 0;

	if (newpos >= p::ccmap->getSize()){
		return 0;
	}

	// @todo: Airborne infantry not supported jet
	if( ((UnitType *)un->getType())->isInfantry() )
	{
		// First check that we can move here
		if (unitandstructmat[newpos].flags & US_IS_UNIT) {
			ig = unitpool[unitandstructmat[newpos].unitnumb]->getInfantryGroup();
			// search for an empty sub position for the unit
			if (ig != 0){
				subpos = ig->GetFreePos();
				if (subpos > 5){
					//assume no free pos
					logger->error ("%s line %i: Found no free position in infantry group (****this could lead to a crash******)\n", __FILE__, __LINE__);
					return subpos;
				}
			}
		}

		if (unitandstructmat[newpos].flags & US_IS_AIRUNIT) {
			ig = unitpool[unitandstructmat[newpos].airunitnumb]->getInfantryGroup();
			// search for an empty sub position for the unit
			if (ig != 0){
				subpos = ig->GetFreePos();
				if (subpos > 5){
					//assume no free pos
					logger->error ("%s line %i: Found no free position in infantry group (****this could lead to a crash******)\n", __FILE__, __LINE__);
					return subpos;
				}
			}
		}


		subpos = un->getSubpos();
		ig = un->getInfantryGroup();
		if (!unload) {
			if (ig->GetNumInfantry() == 1) {
				// old cell is now empty
				if (!un->IsAirBound())
					unitandstructmat[un->getPos()].flags &= ~(US_IS_UNIT|US_UNIT_LOWER_RIGHT);
				else
					unitandstructmat[un->getPos()].flags &= ~(US_IS_AIRUNIT|US_AIRUNIT_LOWER_RIGHT);
				// delete ig;
			} else {
				ig->RemoveInfantry(subpos);
				if (!un->IsAirBound()){
					if (un->getNum() == (unitandstructmat[un->getPos()].unitnumb)) {
						//unitandstructmat[un->getPos()].flags&=0xffff0000;
						unitandstructmat[un->getPos()].unitnumb = 0;
						for (i = 0; i < 5; ++i)
							if (!ig->IsClear(i))
								break;

						if (ig->UnitAt(i)->getNum() > unitpool.size()){
						logger->error ("%s line %i: ERROR, unitnum = %i, size = %i\n", __FILE__, __LINE__, ig->UnitAt(i)->getNum(), unitpool.size());
						}else{
							unitandstructmat[un->getPos()].unitnumb = ig->UnitAt(i)->getNum();
						}
					}
				}else{
					// Airbound
					if (un->getNum() == (unitandstructmat[un->getPos()].airunitnumb)) {
						unitandstructmat[un->getPos()].unitnumb = 0;
						for (i = 0; i < 5; ++i)
							if (!ig->IsClear(i))
								break;

						if (ig->UnitAt(i)->getNum() > unitpool.size()){
							logger->error ("%s line %i: ERROR, unitnum = %i, size = %i\n", __FILE__, __LINE__, ig->UnitAt(i)->getNum(), unitpool.size());
						}else{
							unitandstructmat[un->getPos()].airunitnumb = ig->UnitAt(i)->getNum();
						}
					}
				}
			}
		}
		// check indirectly for infantry group
		// if the new cell has the US_IS_UNIT flag set, it is assumed
		// that there is an infantry group for that cell
		if (unitandstructmat[newpos].flags & US_IS_UNIT) {
			ig = unitpool[unitandstructmat[newpos].unitnumb]->getInfantryGroup();
			// search for an empty sub position for the unit
			subpos = ig->GetFreePos();
			if (subpos > 5){
				//assume no free pos
				logger->error ("%s line %i: Found no free position in infantry group (****this could lead to a crash******)\n", __FILE__, __LINE__);
				return subpos;
			}
		} else { // infantry group does not exist for this cell yet
			ig = new InfantryGroup;
			//unitandstructmat[newpos].flags &= 0xffff0000;
			unitandstructmat[newpos].unitnumb = 0;
			if (un->getNum() > unitpool.size()){
				logger->error ("%s line %i: ERROR, unitnum = %i, size = %i\n", __FILE__, __LINE__, un->getNum(), unitpool.size());
			}else
				unitandstructmat[newpos].unitnumb = un->getNum();
			// note that the subpos variable stays zero (from start of function)
		}
		ig->AddInfantry(un, subpos);
		un->setInfantryGroup(ig);
		unitandstructmat[newpos].flags |= US_UNIT_LOWER_RIGHT|US_IS_UNIT;

		// Check if we need to handle a trigger
		if (unitandstructmat[newpos].flags & US_CELL_HAS_TRIGGER)
			HandleGlobalTrigger(TRIGGER_EVENT_ZONE_ENTRY, newpos );

	} else {
		// easier to assign directly than bitwise AND the compliment
		// of US_MOVING_HERE then bitwise OR this value.
		if (un->getNum() > unitpool.size()){
			logger->error("%s line %i: ERROR, unitnum = %i, size = %i\n", __FILE__, __LINE__, un->getNum(), unitpool.size());
		}else{
#if 0
			//unitandstructmat[newpos].flags &= (US_IS_STRUCTURE|US_STR_LOWER_RIGHT|US_HAS_L2OVERLAY|US_IS_WALL|US_HIGH_COST);
			//printf ("%s line %i: Debug flags is 0x%x\n", __FILE__, __LINE__, unitandstructmat[newpos].flags);
			// changed 31-01-2007
			// unitandstructmat[newpos].flags = US_UNIT_LOWER_RIGHT|US_IS_UNIT;
			unitandstructmat[newpos].flags = US_UNIT_LOWER_RIGHT|US_IS_UNIT;
			unitandstructmat[newpos].unitnumb = un->getNum();
#else

			if (!un->IsAirBound()){
				unitandstructmat[newpos].flags &= ~(US_MOVING_HERE);
				unitandstructmat[newpos].flags |= US_UNIT_LOWER_RIGHT|US_IS_UNIT;
				unitandstructmat[newpos].unitnumb = un->getNum();

				// Check if we need to handle a trigger
				if (unitandstructmat[newpos].flags & US_CELL_HAS_TRIGGER)
					HandleGlobalTrigger ( TRIGGER_EVENT_ZONE_ENTRY, newpos );
			}else{
				unitandstructmat[newpos].flags &= ~(US_AIR_MOVING_HERE);
				unitandstructmat[newpos].flags |= US_AIRUNIT_LOWER_RIGHT|US_IS_AIRUNIT;
				unitandstructmat[newpos].airunitnumb = un->getNum();

				// Check if we need to handle a trigger
				if (unitandstructmat[newpos].flags & US_CELL_HAS_TRIGGER)
					HandleGlobalTrigger ( TRIGGER_EVENT_ZONE_ENTRY, newpos );
			}
#endif
		}
    }

    return subpos;
}

/**
 *
 */
Talkback* UnitAndStructurePool::getTalkback(const char* talkback)
{
    map<string, Talkback*>::iterator typeentry;
    string tname(talkback);
    transform(tname.begin(), tname.end(), tname.begin(), toupper);
    typeentry = talkbackpool.find(tname);

    if (typeentry != talkbackpool.end()) {
        return typeentry->second;
    }
    Talkback* tb(new Talkback());
    tb->load(tname, tbackini);
    talkbackpool[tname] = tb;
    return tb;
}

/**
 *
 */
void UnitAndStructurePool::splitORPreReqs(const string& prereqs, vector<StructureType*>* type_prereqs)
{
    // If prerequiste are "none" we skip it
    if (prereqs == "none")
    {
        return;
    }

    char tmp[16];
    Uint32 i, i2;
    memset(tmp,0,16);
    for (i=0,i2=0;prereqs[i]!=0x0;++i) {
        if ( (i2>=1024) || (tmp != 0 && (prereqs[i] == '|')) ) {
            type_prereqs->push_back(getStructureTypeByName(tmp));
            memset(tmp,0,16);
            i2 = 0;
        } else {
            tmp[i2] = toupper(prereqs[i]);
            ++i2;
        }
    }
    type_prereqs->push_back(getStructureTypeByName(tmp));
}

/**
 * Scans neighbouring cells of a wall for walls and updates their
 * layer zero image.
 *
 * @param st pointer to the wall to scan around
 * @param add if true, the wall has been added, if false, the wall has been removed
 */
void UnitAndStructurePool::updateWalls(Structure* st, bool add, CnCMap* theMap)
{
    Structure* neighbour;
    StructureType* type;
    int cellpos;

    cellpos = st->getPos();
    type = ((StructureType*)st->getType());
    // left
    if (cellpos % theMap->getWidth() > 0)
    {
        if( unitandstructmat[cellpos -1].flags & US_IS_WALL )
        {
            // check if same type
            neighbour = structurepool[unitandstructmat[cellpos -1].structurenumb];
            if( neighbour->getType() == type ) {
                if (add
                   ) {
                    neighbour->changeImage(0, 2);
                    st->changeImage(0, 8);
                }
                else {
                    neighbour->changeImage(0,-2);
                }
            }
        }
    }
    // right
    if (cellpos % theMap->getWidth() <  theMap->getWidth() - 1)
    {
        if (unitandstructmat[cellpos +1].flags & US_IS_WALL)
        {
            // check if same type
            neighbour = structurepool[unitandstructmat[cellpos +1].structurenumb];
            if( neighbour->getType() == type ) {
                if (add
                   ) {
                    neighbour->changeImage(0, 8);
                    st->changeImage(0, 2);
                }
                else {
                    neighbour->changeImage(0,-8);
                }
            }
        }
    }
    // up
    if (cellpos/ theMap->getWidth() > 0)
    {
        if (unitandstructmat[cellpos - theMap->getWidth()].flags & US_IS_WALL)
        {
            // check if same type
            neighbour = structurepool[unitandstructmat[cellpos - theMap->getWidth()].structurenumb];
            if( neighbour->getType() == type ) {
                if (add
                   ) {
                    neighbour->changeImage(0, 4);
                    st->changeImage(0, 1);
                }
                else {
                    neighbour->changeImage(0,-4);
                }
            }
        }
    }
    // down
    if (cellpos/ theMap->getWidth() <  theMap->getHeight() - 1)
    {
        if( unitandstructmat[cellpos + theMap->getWidth()].flags & US_IS_WALL )
        {
            // check if same type
            neighbour = structurepool[unitandstructmat[cellpos + theMap->getWidth()].structurenumb];
            if( neighbour->getType() == type ) {
                if (add
                   ) {
                    neighbour->changeImage(0, 1);
                    st->changeImage(0, 4);
                }
                else {
                    neighbour->changeImage(0,-1);
                }
            }
        }
    }
}

/**
 * @note Simple constructor is private to avoid creation by default
 */
UnitAndStructurePool::UnitAndStructurePool()
{
}
