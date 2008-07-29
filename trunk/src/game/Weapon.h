// Weapon.h
// 1.3

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

#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <vector>
#include <map>

#include "SDL/SDL_types.h"

#include "armour_t.h"

using std::string;
using std::vector;
using std::map;

class Projectile;
class Warhead;
class UnitOrStructure;

/**
 * Weapon in the game
 */
class Weapon 
{
public:
    Weapon(const char * wname);
    ~Weapon();
    
    Uint8 getReloadTime() const;
    Uint8 getRange() const;
    Uint8 getSpeed() const;
    Sint16 getDamage() const;
    bool getWall() const;
    Projectile* getProjectile();

    Warhead* getWarhead();

    char* getChargingSound();

    void fire(UnitOrStructure* owner, Uint16 target, Uint8 subtarget);

    //Uint32 tmppif;
    
    bool isHeatseek() const;

    bool isInaccurate() const;

    double getVersus(armor_t armour) const;

    Uint8 getFuel() const;

    Uint8 getSeekFuel() const;

    const char* getName() const;

    void Reload();

private:
    Projectile* projectile;
    Warhead* whead;
    Uint8 speed;
    Uint8 range;
    Uint8 reloadtime;
    Sint16 damage;
    Uint8 burst;

    /** how many ticks this projectile can move for until being removed. */
    Uint8 fuel;

    /**
     * How many ticks can this projectile change course to track its target before falling back to flying
     * in a straight line.
     */
    Uint8 seekfuel;
    bool heatseek; 
    bool charges;
    
    // @todo Implemente Anim in [Weapon]
    //Uint32 fireimage;
    //Uint32* fireimages;
    //Uint8 numfireimages; 
    //Uint8 numfiredirections;
    
    char* report;
    char* reloadsound;
    char* chargingsound;
    string name;
};

#endif //WEAPON_H
