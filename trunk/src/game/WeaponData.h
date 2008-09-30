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

class INIFile;


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

    int getBurst();

    void setBurst(int burst);

    int getTurboBoost();

    void setTurboBoost(int turboBoost);

    int getSupress();

    void setSupress(int supress);

    string getWarhead();

    void setWarhead(string warhead);

    int getSpeed();

    void setSpeed(int speed);

    string getReport();

    void setReport(string report);

    int getRange();

    void setRange(int range);

    int getRof();

    void setRof(int rof);

    string getProjectile();

    void setProjectile(string projectile);

    int getDamage();

    void setDamage(int damage);

    int getCharges();

    void setCharges(int charges);

    int getCamera();

    void setCamera(int camera);

    static WeaponData* loadWeaponData(INIFile* file, const string& name);
    void print();
    	
private:
	/** animation to display as a firing effect */
	string anim;
	/** number of rapid succession shots from this weapon (def=1)*/
	int burst;
	/** Reveals area around firer (def=no)?*/
	int camera;
	/** Does it have charge-up-before-firing logic (def=no)?*/
	int charges;
	/** the amount of damage (unattenuated) dealt with every bullet*/
	int damage;
	/** projectile characteristic to use */
	string projectile;
	/** delay between shots [15 = 1 second at middle speed setting] */
	int rof;
	/** maximum cell range */
	int range;
	/** sound to play when firing */
	string report;
	/** speed of projectile to target (100 is maximum) */
	int speed;
	/** warhead to attach to projectile */
	string warhead;
	/** Should nearby friendly buildings be scanned for and if found, discourage firing on target (def=no)?*/
	int supress;
	/** Should the weapon get a boosted speed bonus when firing upon aircraft? */
	int turboBoost;
};

#endif //WEAPONDATA_H
