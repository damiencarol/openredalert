// WeaponDataList.cpp
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

#include "WeaponDataList.h"

#include <string>
#include <map>
#include <iterator>

#include "misc/INIFile.h"
#include "WarheadDataList.h"
#include "WeaponData.h"

#include "misc/INIFile.h"

using std::string;
using std::map;
using std::iterator;

void WeaponDataList::loadWeapon(INIFile* file, string name, WarheadDataList* warheadList, ProjectileDataList* projectileList)
{
	WeaponData* ptrWeaponData;
	
	// Load the weapon
	ptrWeaponData = WeaponData::loadWeaponData(file, name);
	
	// save the object (the weapon loaded)
	data[name] = ptrWeaponData;
	
	
	// Get the linked Warhead
	string ptr = ptrWeaponData->getWarhead();
	// Load the linked warhead
	if (ptr.size() > 0) {
		// Get the warhead
		string nameWarhead = ptrWeaponData->getWarhead();
		
		// Load the Warhead
		warheadList->loadWarheadData(file, nameWarhead);
	}
	
	
}

