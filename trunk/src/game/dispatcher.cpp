#include "ActionEventQueue.h"

#include <string>

#include "include/ccmap.h"
#include "include/common.h"
#include "include/config.h"
#include "include/dispatcher.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "game/Unit.h"
#include "include/UnitAndStructurePool.h"
#include "vfs/vfs.h"
#include "StructureType.h"
#include "UnitType.h"
#include "UnitOrStructure.h"
#include "Structure.h"
#include "Unit.h"

using std::string;

namespace Dispatcher {

/** NOTE: I've stripped out the sections related to logging and playback as that
 * part isn't as stable as the rest (basically need to fix a horrible synch
 * issue with the playback)
 */

Dispatcher::Dispatcher() :
    logstate(NORMAL),
    localPlayer(p::ppool->getLPlayerNum())
{
}

Dispatcher::~Dispatcher()
{
    switch (logstate) {
    case RECORDING:
        break;
    case PLAYING:
        break;
    case NORMAL:
    default:
        break;
    }
}

void Dispatcher::unitMove(Unit* un, Uint32 dest)
{
    if (un == 0) {
        return;
    }
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            un->move(dest);
            break;
        case PLAYING:
        default:
            break;
    }
}

void Dispatcher::unitAttack(Unit* un, UnitOrStructure* target, bool tisunit)
{
    if (un == 0) {
        return;
    }
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            un->attack(target);
            break;
        case PLAYING:
        default:
            break;
    }
}

void Dispatcher::unitDeploy(Unit* un)
{
    if (un == 0) {
        return;
    }
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            un->deploy();
            break;
        case PLAYING:
        default:
            break;
    }
}

void Dispatcher::structureAttack(Structure* st, UnitOrStructure* target, bool tisunit)
{
    if (st == 0) {
        return;
    }
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            st->attack(target);
            break;
        case PLAYING:
        default:
            break;
    }
}

bool Dispatcher::structurePlace(const StructureType* type, Uint32 pos, Uint8 owner) {
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            /// XXX TEMP HACK!
            return p::uspool->createStructure(const_cast<StructureType*>(type),pos,owner,FULLHEALTH,0,true);
            break;
        case PLAYING:
        default:
            break;
    };
    /// XXX This won't always be true.
    return true;
}

bool Dispatcher::structurePlace(const char* tname, Uint32 pos, Uint8 owner) {
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            return p::uspool->createStructure(tname,pos,owner,FULLHEALTH,0,true);
            break;
        case PLAYING:
        default:
            break;
    };
    /// XXX This won't always be true.
    return true;
}

bool Dispatcher::unitSpawn(UnitType* type, Uint8 owner) {
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            return p::uspool->spawnUnit(type,owner);
            break;
        case PLAYING:
        default:
            break;
    };
    /// XXX This won't always be true.
    return true;
}

bool Dispatcher::unitSpawn(const char* tname, Uint8 owner) {
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            return p::uspool->spawnUnit(tname,owner);
            break;
        case PLAYING:
        default:
            break;
    };
    /// XXX This won't always be true.
    return true;
}

bool Dispatcher::unitCreate(const char* tname, Uint32 pos, Uint8 subpos, Uint8 owner) {
    switch (logstate) {
        case RECORDING:
            // deliberate fallthrough
        case NORMAL:
            return p::uspool->createUnit(tname,pos,subpos,owner,FULLHEALTH,0);
            break;
        case PLAYING:
        default:
            break;
    };
    /// XXX This won't always be true.
    return true;
}

} /* namespace Dispatcher */
