#include "UnitOrStructureType.h"

#include <string>
#include <vector>

#include "SDL/SDL_types.h"

using std::vector;
using std::string;

UnitOrStructureType::UnitOrStructureType() :
	ptype(0), 
	valid(false),
	techLevel(-1)
{
}

UnitOrStructureType::~UnitOrStructureType()
{
}

Uint8 UnitOrStructureType::getPType() const
{
	return ptype;
}

void UnitOrStructureType::setPType(Uint8 p)
{
	ptype = p;
}

bool UnitOrStructureType::isValid() const
{
	return valid;
}

vector < char *> UnitOrStructureType::getPrereqs() const
{
	return prereqs;
}

Sint32 UnitOrStructureType::getTechLevel() const
{
	return techLevel;
}

Uint16 UnitOrStructureType::getCost() const
{
	return cost;
}

Uint8 UnitOrStructureType::getSpeed() const
{
	return speed;
}

Uint16 UnitOrStructureType::getMaxHealth() const
{
	return maxhealth;
}

Uint8 UnitOrStructureType::getSight() const
{
	return sight;
}
