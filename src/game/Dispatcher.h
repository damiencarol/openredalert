// Dispatcher.h
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

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "SDL/SDL_types.h"

#include "DispatchLogState.h"
#include "ConStatus.h"

class Unit;
class UnitType;
class Structure;
class StructureType;
class UnitOrStructure;
class UnitOrStructureType;

/**
 * Dispatcher a the game
 * 
 * Use to future client/server architecture
 */
class Dispatcher 
{
public:
    Dispatcher();
    ~Dispatcher();
    
    void unitMove(Unit* un, Uint32 dest);
    void unitAttack(Unit* un, UnitOrStructure* target, bool tisunit);
    void unitDeploy(Unit* un);

    void structureAttack(Structure* st, UnitOrStructure* target, bool tisunit);

    /// @todo Implement these
    bool constructionStart(const UnitOrStructureType* type);
    void constructionPause(const UnitOrStructureType* type);
    void constructionPause(Uint8 ptype);
    void constructionResume(const UnitOrStructureType* type);
    void constructionResume(Uint8 ptype);
    void constructionCancel(const UnitOrStructureType* type);
    void constructionCancel(Uint8 ptype);

    ConStatus constructionQuery(const UnitOrStructureType* type);
    ConStatus constructionQuery(Uint8 ptype);

    /** @return true if structure was placed at given location. */
    bool structurePlace(const StructureType* type, Uint32 pos, Uint8 owner);
    bool structurePlace(const char* typen, Uint32 pos, Uint8 owner);
    /** Spawns a unit at the player's appropriate primary building */
    bool unitSpawn(UnitType* type, Uint8 owner);
    bool unitSpawn(const char* tname, Uint8 owner);
    /** Temporary function to place a unit directly on the map */
    bool unitCreate(const char* tname, Uint32 pos, Uint8 subpos, Uint8 owner);

    Uint16 getExitCell(const UnitOrStructureType* type);
    Uint16 getExitCell(Uint8 ptype);
    
private:
    DispatchLogState logstate;
    Uint8 localPlayer;
};

#endif //DISPATCHER_H
