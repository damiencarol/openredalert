#ifndef PROJECTILEDATALIST_H
#define PROJECTILEDATALIST_H

#include <string>
#include <map>

class ProjectileData;
class INIFile;

using std::string;
using std::map;

class ProjectileDataList   {
public:    

	void loadProjectileData(INIFile* file, string name);

	ProjectileData* getData(string name);
    void print();
    /** @link association */
    /*# ProjectileData * lnkProjectileData; */
private:
	map < string, ProjectileData * > data;
};
#endif //PROJECTILEDATALIST_H
