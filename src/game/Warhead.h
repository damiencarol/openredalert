#ifndef WARHEAD_H
#define WARHEAD_H

#include <string>

#include "SDL/SDL_types.h"

#include "armour_t.h"
#include "WeaponData.h"

class WarheadDataList;
class WarheadData;

using std::string;


class Warhead {
public:
    Warhead(string whname, WarheadDataList* data);
    ~Warhead();

    //const char * getExplosionsound();
    bool getWall();
    Uint8 getVersus(armour_t armour);
    WarheadData* getType();

private:
    Uint32 explosionimage;
    Uint8 explosionanimsteps;
    char * explosionsound;

    /** @link aggregation */
    WarheadData* lnkWarheadData;    
};

#endif //WARHEAD_H
