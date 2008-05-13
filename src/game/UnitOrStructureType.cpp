#include "UnitOrStructureType.h"
#include "UnitType.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <math.h>

#include "SDL/SDL_timer.h"
#include "ui/Sidebar.h"
#include "game/Unit.h"
#include "InfantryGroup.h"
#include "StructureType.h"
#include "GameMode.h"
#include "include/talkback.h"
#include "include/weaponspool.h"
#include "video/ImageNotFound.h"
#include "TalkbackType.h"
#include "Weapon.h"
#include <vector>
#include "include/ccmap.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "audio/SoundEngine.h"
#include "include/UnitAndStructurePool.h"

using std::vector;
using std::string;

Uint8 UnitOrStructureType::getPType() const {
    return ptype;
}

void UnitOrStructureType::setPType(Uint8 p) {
    ptype = p;
}

bool UnitOrStructureType::isValid() const {
    return valid;
}

UnitOrStructureType::~UnitOrStructureType() { }

UnitOrStructureType::UnitOrStructureType() : ptype(0), valid(false) { }

std::vector < char * > UnitOrStructureType::getPrereqs() const {
    return prereqs;
}

Uint8 UnitOrStructureType::getTechlevel() const {
    return techlevel;
}

Uint16 UnitOrStructureType::getCost() const {
    return cost;
}

Uint8 UnitOrStructureType::getSpeed() const {
    return speed;
}

Uint16 UnitOrStructureType::getMaxHealth() const {
    return maxhealth;
}

Uint8 UnitOrStructureType::getSight() const {
    return sight;
}
