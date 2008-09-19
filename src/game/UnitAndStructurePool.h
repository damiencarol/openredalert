// UnitAndStructurePool.h
// 1.3

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

#ifndef UNITANDSTRUCTUREPOOL_H
#define UNITANDSTRUCTUREPOOL_H


#define US_IS_UNIT				0x40000000
#define US_IS_STRUCTURE			0x20000000
#define US_IS_WALL				0x10000000
#define US_HAS_L2OVERLAY		0x08000000
#define US_MOVING_HERE			0x07000000
#define US_UNIT_LOWER_RIGHT		0x00800000
#define US_STR_LOWER_RIGHT		0x00400000
#define US_HIGH_COST			0x00200000	// This indicates one can drive over this place but agains high cost ;)
#define US_IS_AIRUNIT			0x00100000
#define US_AIR_MOVING_HERE		0x00040000
#define US_AIRUNIT_LOWER_RIGHT	0x00020000
#define US_CELL_HAS_TRIGGER		0x00010000

//#define US_HAS_PROJECTILE 0x00800000
//#define US_HAS_HIGHPROJ   0x00400000
//#define US_HAS_EXPLOTION  0x00200000

//#define US_HAS_AIRUNIT

#include <set>
#include <string>
#include <map>
#include <vector>

#include "SDL/SDL_types.h"

#include "UnitOrStructure.h"

class RA_Teamtype;
class UnitAndStructureMat;
class UnitOrStructure;
class UnitOrStructureType;
class INIFile;
class L2Overlay;
class Unit;
class UnitType;
class Player;
class Structure;
class StructureType;
class InfantryGroup;
class Talkback;
class CnCMap;
class AiCommand;

using std::string;
using std::vector;
using std::map;
using std::multimap;

/**
 * Stores all units and structures.
 * 
 * Handles level two overlays.
 */
class UnitAndStructurePool {
public:
	/** The constructor needs theater because many structures graphics depends of theater */ 
    UnitAndStructurePool(const char* theTheater);
    ~UnitAndStructurePool();

	Uint8 getStructureNum(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoffsets, Sint8 **yoffsets);
	Uint8 getUnitNum(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoffsets, Sint8 **yoffsets);
	Uint8 getFlyingUnitNum(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoffsets, Sint8 **yoffsets);

    /** Retrieve a limited amount of information from the cell.*/
    bool getUnitOrStructureLimAt(Uint32 curpos, float* width, float* height,
                                 Uint32* cellpos, Uint8* igroup, Uint8* owner,
                                 Uint8* pcol, bool* blocked);
    bool hasL2overlay(Uint16 cellpos) const ;
    Uint8 getL2overlays(Uint16 cellpos, Uint32 **inumbers, Sint8 **xoddset, Sint8 **yoffset);
    
    /** Add an overlay */
    multimap<Uint16, L2Overlay*>::iterator addL2overlay(Uint16 cellpos, L2Overlay* overlay);
    /** Remove an overlay */
    void removeL2overlay(multimap<Uint16, L2Overlay*>::iterator entry);

    bool createReinforcements(RA_Teamtype* Team);
    
    /** Create a structure in a map with this pool */
    bool createStructure(const char* typen, Uint16 cellpos, Uint8 owner,
                Uint16 health, Uint8 facing, bool makeanim, string trigger_name);
    
    /** Create a structure in a map with this pool */
    bool createStructure(StructureType* type, Uint16 cellpos, Uint8 owner,
            Uint16 health, Uint8 facing, bool makeanim, string trigger_name);
    
    
    /** Create a Unit and return it or return NULL */
    Unit* createUnit(const char* typen, Uint16 cellpos, Uint8 subpos,
            Uint8 owner, Uint16 health, Uint8 facing, Uint8 action, string trigger_name);
    /** Create a Unit and return it or return NULL */
    Unit* createUnit(UnitType* type, Uint16 cellpos, Uint8 subpos,
    		Uint8 owner, Uint16 health, Uint8 facing, Uint8 action, string trigger_name);
    
    
    /** Create a trigger on a cell */
    bool createCellTrigger(Uint32 cellpos);

    bool spawnUnit(const char* typen, Uint8 owner);
    bool spawnUnit(UnitType* type, Uint8 owner);

    Unit* getUnitAt(Uint32 cell, Uint8 subcell);
    Unit *getUnitAt(Uint32 cell);
    Uint32 getNumbUnits (void) ;
    Unit* getUnit(Uint32 num);
//    Structure* getStructureAt(Uint32 cell);
    Structure* getStructureAt(Uint32 cell, bool wall);
    Structure* getStructure(Uint32 num);
    Uint32 getNumbStructures (void) ;

//    UnitOrStructure* getSelectedUnitOrStructureAt(Uint32 cell, Uint8 subcell, bool wall = false);
//    UnitOrStructure* getSelectedUnitOrStructureAt(Uint32 cell);
//    UnitOrStructure* getUnitOrStructureAt(Uint32 cell, Uint8 subcell, bool wall = false);
//    UnitOrStructure* getUnitOrStructureAt(Uint32 cell);



	bool cellOccupied (Uint32 cell);
	Unit* getGroundUnitAt ( Uint32 cell, Uint8 subcell = 0 );
	Unit* getFlyingAt ( Uint32 cell, Uint8 subcell = 0 );
	Structure* getStructureAt(Uint32 cell, Uint8 subcell = 0, bool wall = false);



    InfantryGroup* getInfantryGroupAt(Uint32 cell);
    Uint16 getSelected(Uint32 pos);
    Uint16 preMove(Unit *un, Uint8 dir, Sint8 *xmod, Sint8 *ymod, Unit **BlockingUnit);
    Uint8 postMove(Unit *un, Uint16 newpos);
    void abortMove(Unit* un, Uint32 pos);
    UnitType* getUnitTypeByName(const char* unitname);
    StructureType* getStructureTypeByName(const char* structname);
    UnitOrStructureType* getTypeByName(const char* typen);
    bool freeTile(Uint16 pos) const ;
    Uint16 getTileCost( Uint16 pos, Unit* excpUn ) const;
    Uint16 getTileCost( Uint16 pos ) const;
    bool tileAboutToBeUsed(Uint16 pos) const;
    void setCostCalcOwnerAndType(Uint8 owner, Uint8 type)    ;
    void removeUnit(Unit *un);
    void removeStructure(Structure *st);
    bool hasDeleted() ;
    void showMoves();

    /** techtree code */
    void addPrerequisites(UnitType* unittype);
    /** techtree code */
    void addPrerequisites(StructureType* structtype);

    /** scans both inifiles for things with techlevel <= that of the parameter
     * then retrives those types.*/
    void preloadUnitAndStructures(Uint8 techlevel);

    /** Generate reverse dependency information from what units we have loaded.*/
    void generateProductionGroups();

    /** Used by the sidebar to know units */
    vector<const char*> getBuildableUnits(Player* pl);
    /** Used by the sidebar to know structures */
    vector<const char*> getBuildableStructures(Player* pl);

    // unit is removed from map (to be stored in transport)
    void hideUnit(Unit* un);
    Uint8 unhideUnit(Unit* un, Uint16 newpos, bool unload);

    Talkback *getTalkback(const char* talkback);
private:
	/** simple constructor is private to avoid creation by default */
	UnitAndStructurePool();
	
	/** Update the ovrlay of walls ???? */
	void updateWalls(Structure* st, bool add, CnCMap* theMap);
	    
	/** String which keep 5 lettres of the theater. Many Graphics depends of theater */
	char theaterext[5];
	    
    vector<UnitAndStructureMat> unitandstructmat;

    vector<Structure *> structurepool;
    vector<StructureType *> structuretypepool;
    map<string, Uint16> structname2typenum;

    vector<Unit *> unitpool;
    vector<UnitType *> unittypepool;
    map<string, Uint16> unitname2typenum;

    multimap<Uint16, L2Overlay*> l2pool;
    map<Uint16, Uint16> numl2images;

    multimap<StructureType*, std::vector<StructureType*>* > struct_prereqs;
    multimap<UnitType*, vector<StructureType*>* > unit_prereqs;
    void splitORPreReqs(const string& prereqs, vector<StructureType*> * type_prereqs);

    map<string, Talkback*> talkbackpool;

    INIFile* structini; 
    INIFile* unitini; 
    INIFile* tbackini; 
    INIFile* artini;

    Uint8 costcalcowner;
    Uint8 costcalctype;

	/** Image cache numbers for worn down ground  for the small one */
	Uint32 bib1; 
	/** Image cache numbers for worn down ground  for the normal one */
	Uint32 bib2; 
	/** Image cache numbers for worn down ground  for the big one */
	Uint32 bib3;

    bool deleted_unitorstruct;
    Uint16 numdeletedunit;
    Uint16 numdeletedstruct;
};

#endif //UNITANDSTRUCTUREPOOL_H
