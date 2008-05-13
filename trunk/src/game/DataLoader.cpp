#include "DataLoader.h"

#include "WeaponDataList.h"
#include "WarheadDataList.h"
#include "ProjectileDataList.h"


DataLoader::DataLoader()
{
	lnkWeaponDataList = new WeaponDataList();
    lnkWarheadDataList = new WarheadDataList();
    lnkProjectileDataList = new ProjectileDataList();
}
