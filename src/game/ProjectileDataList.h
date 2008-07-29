// ProjectileDataList.h
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

#ifndef PROJECTILEDATALIST_H
#define PROJECTILEDATALIST_H

#include <string>
#include <map>

class ProjectileData;
class INIFile;

using std::string;
using std::map;

/**
 * List of data of projectiles
 */
class ProjectileDataList
{
public:
	/** Load data of projectile from .ini file */
	void loadProjectileData(INIFile* file, string name);
	/** Get projectile data of a loaded projectile */
	ProjectileData* getData(string name);
	/** Print all the projectile data in the list */
	void print();
	/** @link association */
	/*# ProjectileData * lnkProjectileData; */
	
private:
	/** List of internal data loaded */ 
	map < string, ProjectileData *> data;
};

#endif //PROJECTILEDATALIST_H
