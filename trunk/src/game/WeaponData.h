// WeaponData.h
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

#ifndef WEAPONDATA_H
#define WEAPONDATA_H

#include <string>

#include "SDL/SDL_types.h"

#include "misc/INIFile.h"


using std::string;

/**
 * Weapon Statistics
 *
 * The weapons specified here are attached to the various combat units and buildings. 
 */
class WeaponData
{
public:
    string getAnim();

    void setAnim(string anim);

    Uint32 getBurst();

    void setBurst(Uint32 burst);

    Uint32 getTurboBoost();

    void setTurboBoost(Uint32 turboBoost);

    Uint32 getSupress();

    void setSupress(Uint32 supress);

    string getWarhead();

    void setWarhead(string warhead);

    Uint32 getSpeed();

    void setSpeed(Uint32 speed);

    string getReport();

    void setReport(string report);

    Uint32 getRange();

    void setRange(Uint32 range);

    Uint32 getRof();

    void setRof(Uint32 rof);

    string getProjectile();

    void setProjectile(string projectile);

    Uint32 getDamage();

    void setDamage(Uint32 damage);

    Uint32 getCharges();

    void setCharges(Uint32 charges);

    Uint32 getCamera();

    void setCamera(Uint32 camera);

    static WeaponData* loadWeaponData(INIFile* file, string name);
    void print();
    	
private:
	/** animation to display as a firing effect*/
	string anim;
	/** number of rapid succession shots from this weapon (def=1)*/
	Uint32 burst;
	/** Reveals area around firer (def=no)?*/
	Uint32 camera;
	/** Does it have charge-up-before-firing logic (def=no)?*/
	Uint32 charges;
	/** the amount of damage (unattenuated) dealt with every bullet*/
	Uint32 damage;
	/** projectile characteristic to use*/
	string projectile;
	/** delay between shots [15 = 1 second at middle speed setting]*/
	Uint32 rof;
	/** maximum cell range*/
	Uint32 range;
	/** sound to play when firing*/
	string report;
	/** speed of projectile to target (100 is maximum)*/
	Uint32 speed;
	/** warhead to attach to projectile*/
	string warhead;
	/** Should nearby friendly buildings be scanned for and if found, discourage firing on target (def=no)?*/
	Uint32 supress;
	/** Should the weapon get a boosted speed bonus when firing upon aircraft?*/
	Uint32 turboBoost;
};

#endif //WEAPONDATA_H
