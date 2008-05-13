#ifndef DATALOADER_H
#define DATALOADER_H

class WeaponDataList;
class WarheadDataList;
class ProjectileDataList;

class DataLoader {
	
public:
	DataLoader();
	
    /** @link aggregation */
    WeaponDataList* lnkWeaponDataList;
    /** @link aggregation */
    WarheadDataList* lnkWarheadDataList;
    /** @link aggregation */
    ProjectileDataList* lnkProjectileDataList;

    //virtual void load() =0;
};

#endif //DATALOADER_H
