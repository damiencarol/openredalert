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


#endif
