#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <map>

#include "SDL/SDL_types.h"

class UnitOrStructureType;

using std::map;

class Production : public map <const UnitOrStructureType*, Uint8>
{
};
#endif //PRODUCTION_H
