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

