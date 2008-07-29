// Projectille.h
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

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"

class ProjectileData;
class ProjectileDataList;
class SHPImage;

using std::string;
using std::vector;

/**
 * Projectille in game
 */
class Projectile
{
public:
    Projectile(string pname, ProjectileDataList* weapini, vector<SHPImage*>*imagePool);
    ~Projectile();
    Uint32 getImageNum()    ;
    //Uint8 getSpeed(){return speed;}
    bool doesRotate();
	bool AntiAir();
	bool AntiGround();
	bool AntiSubmarine();
	bool getInaccurate();

private:
    Uint32 imagenum;
    Uint8 rotationimgs;


    /** @link aggregation */
    ProjectileData* lnkProjectileData;
};


#endif //PROJECTILE_H
