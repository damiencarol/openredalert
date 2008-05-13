#ifndef SELECTION_H
#define SELECTION_H

#include <list>

#include "SDL/SDL_types.h"
#include "game/Unit.h"

class Structure;

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
    std::list<Unit*> sel_units;
    std::list<Structure*> sel_structs;

    Uint32 numattacking;
    bool enemy_selected;
    std::list<Unit*>saved_unitsel[10];
    std::list<Structure*>saved_structsel[10];
};

#endif
