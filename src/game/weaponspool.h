// weaponspool.h
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
