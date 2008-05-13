#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <cstdio>
#include <queue>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"

#include "common.h"
#include "game/StructureType.h"
#include "game/UnitType.h"
#include "game/UnitOrStructureType.h"
#include "game/UnitOrStructure.h"
#include "game/Structure.h"
#include "game/DispatchLogState.h"
#include "game/ConStatus.h"
#include "game/Unit.h"

class VFile;

namespace Dispatcher {

class Dispatcher {
public:
    Dispatcher();
    ~Dispatcher();
    void unitMove(Unit* un, Uint32 dest);
    void unitAttack(Unit* un, UnitOrStructure* target, bool tisunit);
    void unitDeploy(Unit* un);

    void structureAttack(Structure* st, UnitOrStructure* target, bool tisunit);

    /// @TODO Implement these
    bool constructionStart(const UnitOrStructureType* type);
    void constructionPause(const UnitOrStructureType* type);
    void constructionPause(Uint8 ptype);
    void constructionResume(const UnitOrStructureType* type);
    void constructionResume(Uint8 ptype);
    void constructionCancel(const UnitOrStructureType* type);
    void constructionCancel(Uint8 ptype);

    ConStatus constructionQuery(const UnitOrStructureType* type);
    ConStatus constructionQuery(Uint8 ptype);

    /// @returns true if structure was placed at given location.
    bool structurePlace(const StructureType* type, Uint32 pos, Uint8 owner);
    bool structurePlace(const char* typen, Uint32 pos, Uint8 owner);
    /// Spawns a unit at the player's appropriate primary building
    bool unitSpawn(UnitType* type, Uint8 owner);
    bool unitSpawn(const char* tname, Uint8 owner);
    /// Temporary function to place a unit directly on the map
    bool unitCreate(const char* tname, Uint32 pos, Uint8 subpos, Uint8 owner);

    Uint16 getExitCell(const UnitOrStructureType* type);
    Uint16 getExitCell(Uint8 ptype);
private:
    DispatchLogState logstate;
    Uint8 localPlayer;
};

}

#endif //DISPATCHER_H
