// UnitOrStructureType.cpp
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
