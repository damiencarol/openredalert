// Selection.h
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

#ifndef SELECTION_H
#define SELECTION_H

#include <list>

#include "SDL/SDL_types.h"

class Structure;
class Unit;

using std::list;

/**
 * Selectino of Units and structure in the game
 */
class Selection
{
public:
    Selection();
    ~Selection();

    bool addUnit(Unit *selunit, bool enemy);
    void removeUnit(Unit *selunit);
    bool addStructure(Structure *selstruct, bool enemy);
    void removeStructure(Structure *selstruct);

    bool saveSelection(Uint8 savepos);
    bool loadSelection(Uint8 loadpos);
    bool mergeSelection(Uint8 loadpos);

    void clearSelection();

    void purge(Unit* sel);
    void purge(Structure* sel);

    Uint32 numbUnits() const ;
    Unit* getUnit(Uint32 UnitNumb) ;
    /** Return a ref to a selected structure */
    Structure* getStructure(Uint32 structureNumber);

    bool canAttack() const ;
    bool canMove() const ;
	bool areWaterBound ();
    bool isEnemy() const ;
    bool empty() const ;
    /** Returns the number of the player who owns all things selected */
    Uint8 getOwner() const;
    void moveUnits(Uint32 pos);
    void attackUnit(Unit* target);
    void attackStructure(Structure* target);
    void checkSelection();
    Unit* getRandomUnit();
    bool getWall() const;
    void stop();
    
private:
    list<Unit*> sel_units;
    list<Structure*> sel_structs;

    Uint32 numattacking;
    bool enemy_selected;
    list<Unit*>saved_unitsel[10];
    list<Structure*>saved_structsel[10];
};

#endif //SELECTION_H
