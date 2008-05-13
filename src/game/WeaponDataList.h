#ifndef WEAPONDATALIST_H
#define WEAPONDATALIST_H

#include <string>
#include <map>

#include "misc/INIFile.h"

#include "misc/INIFile.h"

class WarheadDataList;
class ProjectileDataList;
class WeaponData;

using std::string;
using std::map;


class WeaponDataList   {
public:
	void loadWeapon(INIFile* file, string name, WarheadDataList* warheadList, ProjectileDataList* projectileList);

private:

	map < string, WeaponData * > data;

    /** @link association */
    /*# WeaponData * lnkWeaponData; */
};

#endif //WEAPONDATALIST_H
