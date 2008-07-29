// UnitOrStructure.h
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

#ifndef UNITORSTRUCTURE_H
#define UNITORSTRUCTURE_H

#include <string>

#include "SDL/SDL_types.h"

#include "UnitOrStructureType.h"

//
//  Commands that tell us what the unit or structure should be doing
//
#define COMMAND_SLEEP       0   // Unit sits still and plays dead.
#define COMMAND_ATTACK      1   // Special attack mission used by team logic.
#define COMMAND_MOVE        2   // Move to destination
#define COMMAND_QMOVE       3   // Special move to destination after all other queued moves occur.
#define COMMAND_REPEAT      4   // Run away (possibly leave the map).
#define COMMAND_GUARD       5   // Sit around and engage any enemy that wanders within weapon range.
#define COMMAND_STICKY      6   // Just like guard mode, but cannot move.
#define COMMAND_ENTER       7   // Enter building or transport for loading purposes.
#define COMMAND_CAPTURE     8   // Engineer entry logic.
#define COMMAND_HARVEST     9   // Handle harvest ore - dump at refinery loop.
#define COMMAND_AREA_GUARD  10  // Guard the general area where the unit starts at.
#define COMMAND_RETURN      11  // unused
#define COMMAND_STOP        12  // Stop moving and firing at the first available opportunity.
#define COMMAND_AMBUSH      13  // unused
#define COMMAND_HUNT        14  // Scan for and attack any enemies whereever they may be.
#define COMMAND_UNLOAD      15  // While dropping off cargo (e.g., APC unloading passengers).
#define COMMAND_SABOTAGE    16  // Tanya running to place bomb in building.
#define COMMAND_CONSTRUCT   17  // Buildings use this when building up after initial placement.
#define COMMAND_SELLING     18  // Buildings use this when deconstruction after being sold.
#define COMMAND_REPAIR      19  // Service depot uses this mission to repair attached object.
#define COMMAND_RESCUE      20  // Special team override mission.
#define COMMAND_MISSILE     21  // Missile silo special launch missile mission.
#define COMMAND_HARMLESS    22  // Unit doesn't fire and is not considered a threat.

class Weapon;
class WeaponsPool;
class Unit;
class Structure;
class UnitAndStructurePool;

using std::string;

/**
 * Like UnitOrStructureType, UnitOrStructure is a way of referring to
 * either units or structures (but only when it is either not possible
 * or not necessary to know what it actually is).  This class is also
 * abstract.  Since both classes are abstract, care must be taken to
 * ensure one does not call a pure virtual method by mistake.
 */
class UnitOrStructure {
//private:
protected:
    /* using protected data members to make the code cleaner:
     * referTo, unrefer, select and unselect are common to both Units
     * and structures, so rather than duplicate code, we handle them
     * here.
     */
    Uint8 references;
    bool deleted; 
    bool selected;
    Uint16 targetcell;
    UnitOrStructure* target;
    Uint8 showorder_timer;
public:
    UnitOrStructure() ;

    virtual ~UnitOrStructure();

    virtual Uint32 getNum() const = 0;

    virtual Sint8 getXoffset() const = 0;

    virtual Sint8 getYoffset() const = 0;

    virtual void setXoffset(Sint8 xo) ;

    virtual void setYoffset(Sint8 yo) ;

private:
    Uint16 health;
public:
    Uint16 getHealth() const;

    void setHealth(Uint16 health);

    virtual Uint8 getOwner() const = 0;

    virtual void setOwner(Uint8) = 0;

    virtual Uint16 getPos() const = 0;

    virtual Uint16 getSubpos() const = 0;

    /** 
     * get the first blocked cell in structure.
     *
     * Normally the first blocked cell is the top-left most cell.
     * However, certain structures, such as the repair bay, do not have
     * this cell blocked, so the original targetting code could not hit
     * that structure.  There is no overhead in calculating the first
     * blocked cell as it is done at the same time as the blocked matrix
     * is first created.
     */
    virtual Uint16 getBPos(Uint16 pos) const = 0;

    /**
     * Type of the Structure/Unit
     */
    virtual UnitOrStructureType* getType() = 0;

    virtual Uint32 getImageNum(Uint8 layer) const = 0;

    virtual void setImageNum(Uint32 num, Uint8 layer) = 0;

    void referTo() ;

    void unrefer();

    void remove();

    bool isAlive() ;

    void select() ;

    void unSelect() ;

    bool isSelected() ;

    virtual void attack(UnitOrStructure* target) = 0;

    virtual void applyDamage(Sint16 amount, Weapon * weap, UnitOrStructure* attacker) = 0;

    /** @returns ratio of actual health over maximum health for type. */
    virtual double getRatio() const = 0;

    virtual Uint32 getExitCell() const ;

    virtual Uint16 getTargetCell() const ;

    virtual UnitOrStructure * getTarget() ;

    virtual string getTriggerName (void)  = 0;
};

#endif
