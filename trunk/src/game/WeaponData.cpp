// WeaponData.cpp
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

#include "WeaponData.h"

#include <string>
#include <iostream>

#include "SDL/SDL_types.h"

#include "misc/INIFile.h"
#include "include/Logger.h"

#include "misc/INIFile.h"

using std::string;
using std::cout;
using std::endl;

extern Logger * logger;

string  WeaponData::getAnim()
{
	return anim;
}

Uint32 WeaponData::getBurst()
{
	return burst;
}

void WeaponData::setBurst(Uint32 burst)
{
	this->burst = burst;
}

Uint32 WeaponData::getTurboBoost()
{
	return turboBoost;
}

void WeaponData::setTurboBoost(Uint32 turboBoost)
{
	this->turboBoost = turboBoost;
}

Uint32 WeaponData::getSupress()
{
	return supress;
}

void WeaponData::setSupress(Uint32 supress)
{
	this->supress = supress;
}

string WeaponData::getWarhead()
{
	return warhead;
}

void WeaponData::setWarhead(string warhead)
{
	this->warhead = warhead;
}

Uint32 WeaponData::getSpeed()
{
	return speed;
}

void WeaponData::setSpeed(Uint32 speed)
{
	this->speed = speed;
}

string  WeaponData::getReport()
{
	return report;
}

void WeaponData::setReport(string  report)
{
	this->report = report;
}

Uint32 WeaponData::getRange()
{
	return range;
}

void WeaponData::setRange(Uint32 range)
{
	this->range = range;
}

Uint32 WeaponData::getRof()
{
	return rof;
}

void WeaponData::setRof(Uint32 rof)
{
	this->rof = rof;
}

string  WeaponData::getProjectile()
{
	return projectile;
}

void WeaponData::setProjectile(string  projectile)
{
	this->projectile = projectile;
}

Uint32 WeaponData::getDamage()
{
	return damage;
}

void WeaponData::setDamage(Uint32 damage)
{
	this->damage = damage;
}

Uint32 WeaponData::getCharges()
{
	return charges;
}

void WeaponData::setCharges(Uint32 charges)
{
	this->charges = charges;
}

Uint32 WeaponData::getCamera()
{
	return camera;
}

void WeaponData::setCamera(Uint32 camera)
{
	this->camera = camera;
}

void WeaponData::setAnim(string  anim)
{
	this->anim = anim;
}
WeaponData* WeaponData::loadWeaponData(INIFile * file, string name)
{
	WeaponData* ptrWeaponData;

	// Create the WeaponData object
	ptrWeaponData = new WeaponData();

	// Anim = animation to display as a firing effect
	char * tmpPt = file->readString(name.c_str(), "Anim");
	if (tmpPt != NULL)
	{
		string tmpStr = (string)tmpStr;
		ptrWeaponData->setAnim(tmpStr);
	}

	// Burst = number of rapid succession shots from this weapon (def=1)
	Uint32 tmpBurst = file->readInt(name.c_str(), "Burst", 1);
	ptrWeaponData->setBurst(tmpBurst);

	// Camera = Reveals area around firer (def=no)?
	char* tmpPtCamera = file->readString(name.c_str(), "Camera");
	if (tmpPtCamera != NULL)
	{
		string tmpCamera = (string)tmpPtCamera;
		Uint32 a;
		if (tmpCamera == "yes")
		{
			a = 1;
		}
		else
		{
			a = 0;
		}
		ptrWeaponData->setCamera(a);
	}

	// Charges = Does it have charge-up-before-firing logic (def=no)?
	char* tmpPtCharges = file->readString(name.c_str(), "Charges", "no");
	string tmpCharges = (string)tmpPtCharges;
	Uint32 b;
	if (tmpCharges == "yes")
	{
		b = 1;
	}
	else
	{
		b = 0;
	}
	ptrWeaponData->setCharges(b);

	// Damage = the amount of damage (unattenuated) dealt with every bullet
	Uint32 tmpDamage = file->readInt(name.c_str(), "Damage");
	ptrWeaponData->setDamage(tmpDamage);

	// Projectile = projectile characteristic to use
	char* tmpPtProjectile = file->readString(name.c_str(), "Projectile");
	if (tmpPtProjectile != NULL)
	{
		string tmpProjectile = (string)tmpPtProjectile;
		ptrWeaponData->setProjectile(tmpProjectile);
	}

	// ROF = delay between shots [15 = 1 second at middle speed setting]
	Uint32 tmpROF = file->readInt(name.c_str(), "ROF");
	ptrWeaponData->setRof(tmpROF);

	// Range = maximum cell range
	Uint32 tmpRange = file->readInt(name.c_str(), "Range");
	ptrWeaponData->setRange(tmpRange);

	// Report = sound to play when firing
	char* tmpPtReport = file->readString(name.c_str(), "Report");
	if (tmpPtReport != NULL)
	{
		string tmpReport = (string)tmpPtReport;
		ptrWeaponData->setReport(tmpReport);
	}

	// Speed = speed of projectile to target (100 is maximum)
	Uint32 tmpSpeed = file->readInt(name.c_str(), "Speed");
	ptrWeaponData->setSpeed(tmpSpeed);

	// Warhead = warhead to attach to projectile
	char* tmpPtWarhead = file->readString(name.c_str(), "Warhead");
	if (tmpPtWarhead != NULL)
	{
		string tmpWarhead = (string)tmpPtWarhead;
		ptrWeaponData->setWarhead(tmpWarhead);
	}

	// Supress = Should nearby friendly buildings be scanned for and if found, discourage firing on target (def=no)?
	char* tmpPtSupress = file->readString(name.c_str(), "Supress", "no");
	string tmpSupress = (string)tmpPtSupress;
	Uint32 c;
	if (tmpSupress == "yes")
	{
		c = 1;
	}
	else
	{
		c = 0;
	}
	ptrWeaponData->setSupress(c);

	// TurboBoost = Should the weapon get a boosted speed bonus when firing upon aircraft?
	char* tmpPtTurboBoost = file->readString(name.c_str(), "TurboBoost", "no");
	string tmpTurboBoost = (string)tmpPtTurboBoost;
	Uint32 d;
	if (tmpTurboBoost == "yes")
	{
		d = 1;
	}
	else
	{
		d = 0;
	}
	ptrWeaponData->setTurboBoost(d);

	// Returns the constructed object
	return ptrWeaponData;
}

/**
 * Print the Data
 */
void WeaponData::print()
{
	cout << "Anim=" << getAnim() << endl;
	cout << "Burst=" << getBurst() << endl;
	cout << "Camera=" << getCamera() << endl;
	cout << "Charges=" << getCharges() << endl;
	cout << "Damage=" << getDamage() << endl;
	cout << "Projectile=" << getProjectile() << endl;
	cout << "ROF=" << getRof() << endl;
	cout << "Range=" << getRange() << endl;
	cout << "Report=" << getReport() << endl;
	cout << "Speed=" << getSpeed() << endl;
	cout << "Warhead=" << getWarhead() << endl;
	cout << "Supress=" << getSupress() << endl;
	cout << "TurboBoost=" << getTurboBoost() << endl;
}
