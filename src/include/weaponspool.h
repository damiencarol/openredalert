#ifndef WEAPONSPOOL_H
#define WEAPONSPOOL_H

#include <string>
#include <map>

#include "misc/INIFile.h"

class INIFile;
class Projectile;
class Weapon;
class Warhead;

using std::string;
using std::map;

class WeaponsPool {
public:
    friend class Weapon;
   // friend class Projectile;
   // friend class Warhead;
    WeaponsPool();
    ~WeaponsPool();


    Weapon * getWeapon(const char * wname);
    INIFile * getWeaponsINI();

private:
    map < string, Weapon * > weaponspool;
    map < string, Projectile * > projectilepool;
    map < string, Warhead * > warheadpool;
    INIFile * weapini;
};

#endif //WEAPONSPOOL_H
