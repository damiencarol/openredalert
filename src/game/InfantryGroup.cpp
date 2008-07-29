// InfantryGroup.cpp
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

#include "InfantryGroup.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <math.h>

#include "SDL/SDL_timer.h"

#include "CnCMap.h"
#include "weaponspool.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "PlayerPool.h"
#include "audio/SoundEngine.h"
#include "UnitAndStructurePool.h"
#include "Unit.h"

using std::string;
using std::vector;

InfantryGroup::InfantryGroup()
{
	//logger->debug("Setting up infgroup %p\n", this);
	for (int i=0;i<5;i++){
		positions[i] = NULL;
	}
	numinfantry = 0;
}

InfantryGroup::~InfantryGroup()
{
	//logger->debug("Destructing infgroup %p\n", this);
	// printf ("%s line %i: Destroying infgroup\n", __FILE__, __LINE__);
}

const Sint8 InfantryGroup::unitoffsets[10] = {
    // Theses values have been heavily tested, do NOT change them unless you're
    //        _really_ sure of what you are doing
    // X value
    -13, -19, -7, -19, -7,
    // Y value
    -3, -7, -7, 1, 1
};

bool InfantryGroup::AddInfantry(Unit* inf, Uint8 subpos)
{
	assert(subpos < 5);
	assert(numinfantry < 5);
	positions[subpos] = inf;
	++numinfantry;
	return true;
}

bool InfantryGroup::RemoveInfantry(Uint8 subpos)
{
	assert(subpos < 5);
	assert(numinfantry > 0);
	positions[subpos] = NULL;
	--numinfantry;
	return true;
}

bool InfantryGroup::IsClear(Uint8 subpos)
{
	assert(subpos < 5);
	return (positions[subpos] == NULL);
}

Uint8 InfantryGroup::GetNumInfantry() const
{
	return numinfantry;
}

bool InfantryGroup::IsAvailable() const
{
	return (numinfantry < 5);
}

Uint8 InfantryGroup::GetFreePos() const
{
	for (int i = 0; i < 5; ++i) {
		if (0 == positions[i]) {
			return i;
		}
	}
	return (Uint8)-1;
}

Unit* InfantryGroup::UnitAt(Uint8 subpos)
{
	assert(subpos < 5);
	return positions[subpos];
}

Uint8 InfantryGroup::GetImageNums(Uint32** inums, Sint8** xoffsets, Sint8** yoffsets)
{
	(*inums) = new Uint32[numinfantry];
	(*xoffsets) = new Sint8[numinfantry];
	(*yoffsets) = new Sint8[numinfantry];
	int j = 0;
	for (int i = 0; i < 5; ++i) {
		if (0 != positions[i]) {
			(*inums)[j]=positions[i]->getImageNum(0);
			(*xoffsets)[j]=positions[i]->getXoffset()+unitoffsets[i];
			(*yoffsets)[j]=positions[i]->getYoffset()+unitoffsets[i+5];
			//printf ("%s line %i: pos = %i, offset = %i\n", __FILE__, __LINE__, positions[i]->getXoffset(), unitoffsets[i+5]);
			j++;
		}
	}
	return numinfantry;
}

void InfantryGroup::GetSubposOffsets(Uint8 oldsp, Uint8 newsp, Sint8* xoffs, Sint8* yoffs)
{
	*xoffs = unitoffsets[oldsp] - unitoffsets[newsp];
	*yoffs = unitoffsets[oldsp+5] - unitoffsets[newsp+5];
}

const Sint8* InfantryGroup::GetUnitOffsets()
{
	return unitoffsets;
}

Unit * InfantryGroup::GetNearest(Uint8 subpos)
{
    static const Uint8 lut[20] = {
        1,2,3,4,
        3,0,2,4,
        1,0,4,3,
        1,0,4,2,
        3,0,2,1
    };
    Uint8 x;

    // The compiler will optimise this nicely with -funroll-loops,
    // leaving it like this to keep it readable.
    for (x = 0; x < 4; ++x)
        if (0 != positions[lut[x + subpos * 4]]) return positions[lut[x + subpos * 4]];
    return 0;
}
