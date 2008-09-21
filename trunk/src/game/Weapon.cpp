// Weapon.cpp
// 1.4

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

#include "Weapon.h"

#include <algorithm>
#include <cctype>
#include <vector>

#include "Projectile.h"
#include "UnitOrStructure.h"
#include "Warhead.h"
#include "ExplosionAnim.h"
#include "ActionEventQueue.h"
#include "UnitOrStructure.h"
#include "include/Logger.h"
#include "weaponspool.h"
#include "video/ImageNotFound.h"
#include "video/SHPImage.h"
#include "audio/SoundEngine.h"
#include "UnitOrStructureType.h"
#include "InfantryGroup.h"
#include "ProjectileAnim.h"
#include "RedAlertDataLoader.h"

#include "include/common.h"

using std::vector;
using std::for_each;

namespace p
{
	extern WeaponsPool * weappool;
	extern ActionEventQueue * aequeue;
	extern RedAlertDataLoader * raLoader;
}
namespace pc
{
	extern SoundEngine * sfxeng;
	extern std::vector<SHPImage *> * imagepool;
}
extern Logger * logger;

Weapon::Weapon(const char* wname)
{
	char *pname= 0;
	char *whname= 0;
	//char *faname= 0;
	//char *faimage= 0;
	map<string, Projectile*>::iterator projentry;
	map<string, Warhead*>::iterator wheadentry;
	INIFile * weapini;
	//SHPImage * fireanimtemp;
	//Uint8 additional;
	//Uint8 i;
	string projname, warheadname;
	string weapname;

	INIFile* rules = 0;

	name = string(wname);

	rules = new INIFile("rules.ini");

	weapini = p::weappool->getWeaponsINI();
	weapname = (string)wname;

	// UPPER the string 'weapname'
	for_each(weapname.begin(), weapname.end(), toupper);


	pname = weapini->readString(wname, "projectile");
	if (pname == NULL)
	{
		logger->warning(
				"Unable to find projectile for weapon \"%s\" in inifile..\n",
				wname);
		throw 0;
	}
	projname = (string)pname;

	// UPPER the string 'projname'
	for_each(projname.begin(), projname.end(), toupper);


	projentry = p::weappool->projectilepool.find(projname);
	if (projentry == p::weappool->projectilepool.end() )
	{
		try
		{
			projectile = new Projectile(string(pname),
					p::raLoader->lnkProjectileDataList,
					pc::imagepool);
		}
		catch(...)
		{
			logger->warning("Unable to find projectile \"%s\" used for weapon \"%s\".\nUnit using this weapon will be unarmed\n", pname, wname);
			delete[] pname;
			throw 0;
		}
		p::weappool->projectilepool[projname] = projectile;
	}
	else
	{
		projectile = projentry->second;
	}
	delete[] pname;

	whname = weapini->readString(wname, "warhead");
	if (whname==NULL)
	{
		logger->warning(
				"Unable to find warhead for weapon \"%s\" in inifile..\n",
				wname);
		throw 0;
	}
	warheadname = (string)whname;

	transform(warheadname.begin(), warheadname.end(), warheadname.begin(),
			toupper);
	wheadentry = p::weappool->warheadpool.find(warheadname);
	if (wheadentry == p::weappool->warheadpool.end() )
	{
		try
		{
			// Try to create the Warhead
			whead = new Warhead(whname, p::raLoader->lnkWarheadDataList);
		}
		catch(...)
		{
			logger->warning("Unable to find Warhead \"%s\" used for weapon \"%s\".\nUnit using this weapon will be unarmed\n", whname, wname);
			delete[] whname;
			throw 0;
		}
		p::weappool->warheadpool[warheadname] = whead;
	}
	else
	{
		whead = wheadentry->second;
	}
	delete[] whname;

	speed = weapini->readInt(wname, "speed", 100);
	range = weapini->readInt(wname, "range", 4);
	reloadtime = weapini->readInt(wname, "reloadtime", 50);
	damage = weapini->readInt(wname, "damage", 10);
	burst = weapini->readInt(wname, "burst", 1);
	heatseek = (weapini->readInt(wname, "heatseek", 0) != 0);


	// pc::imagepool->push_back(new SHPImage("minigun.shp", mapscaleq));
	//firesound = weapini->readString(wname, "firesound");
	//printf("wname = %s\n", wname);
	report = rules->readString(wname, "Report");
	if (report != 0){
		string soundWeap = report;
		soundWeap += string(".aud");
		transform(soundWeap.begin(), soundWeap.begin(), soundWeap.end(), tolower);
		//logger->debug("Report = %s\n", soundWeap.c_str());
		report = cppstrdup(soundWeap.c_str());
		pc::sfxeng->LoadSound(report);
	}
	reloadsound = weapini->readString(wname, "reloadsound");
	if (reloadsound != 0)
		pc::sfxeng->LoadSound(reloadsound);

	chargingsound = weapini->readString(wname, "chargingsound");
	if (chargingsound != 0)
		pc::sfxeng->LoadSound(chargingsound);

	fuel = weapini->readInt(wname, "fuel", 0);
	seekfuel = weapini->readInt(wname, "seekfuel", 0);

	// @todo Implemente Anim in [Weapon]
	/*
	fireimage = pc::imagepool->size()<<16;
	faname = weapini->readString(wname, "fireimage", "none");
	//printf ("%s line %i: Weapon = %s, fireimage = %s\n", __FILE__, __LINE__, wname, faname);
	if (strcmp((faname), ("none")) == 0)
	{
		delete[] faname;
		numfireimages = 0;
		numfiredirections = 1;
		fireimage = 0;
	}
	else
	{
		additional = (Uint8)weapini->readInt(faname, "additional", 0);
		faimage = weapini->readString(faname, "image", "minigun.shp");
		try
		{
			fireanimtemp = new SHPImage(faimage, -1);
		}
		catch (ImageNotFound&)
		{
			throw 0;
		}
		delete[] faimage;
		faimage = NULL;
		numfireimages = fireanimtemp->getNumImg();
		numfiredirections = weapini->readInt(faname, "directions", 1);
		if (numfiredirections == 0)
		{
			numfiredirections = 1;
		}
		fireimages = new Uint32[numfiredirections];
		fireimages[0] = fireimage;
		pc::imagepool->push_back(fireanimtemp);
		if (additional != 0)
		{
			char* tmpname = new char[12];
			for (i=2; i<=additional; ++i)
			{
				sprintf(tmpname, "image%i", i);
				faimage = weapini->readString(faname, tmpname, "");
				if (strcmp((faimage), ("")) != 0)
				{
					try
					{
						fireanimtemp = new SHPImage(faimage, -1);
					}
					catch (ImageNotFound&)
					{
						throw 0;
					}
					fireimages[i-1]=(pc::imagepool->size()<<16);
					numfireimages += fireanimtemp->getNumImg();
					pc::imagepool->push_back(fireanimtemp);
				}
				else
				{
					fireimages[i] = 0;
					logger->warning("%s was empty in [%s]\n", tmpname, faname);
				}
				delete[] faimage;
				faimage = NULL;
			}
			delete[] tmpname;
		}
		else if (numfiredirections != 1)
		{
			for (i=1; i<numfiredirections; ++i)
			{
				fireimages[i] = fireimage+i*(numfireimages/numfiredirections);
			}
		}
		delete[] faname;
	}*/

	// Free rules.ini
	delete rules;
}

Weapon::~Weapon()
{
	// If Report sound exist
	if (report != 0)
	{
		// delete it
		delete[] report;
	}

	// If Reload sound exist
	if (reloadsound != 0)
	{
		delete[] reloadsound;
	}

	// @todo Implemente Anim in [Weapon]
	//if (fireimage != 0)
	//{
	//	delete[] fireimages;
	//}
}

Uint8 Weapon::getReloadTime() const
{
	return reloadtime;
}

Uint8 Weapon::getRange() const
{
	return range;
}

Uint8 Weapon::getSpeed() const
{
	return speed;
}

Sint16 Weapon::getDamage() const
{
	return damage;
}

bool Weapon::getWall() const
{
	return whead->getWall();
}

Projectile *Weapon::getProjectile()
{
	return projectile;
}

Warhead * Weapon::getWarhead()
{
	return whead;
}

char * Weapon::getChargingSound()
{
	return chargingsound;
}

void Weapon::fire(UnitOrStructure* owner, Uint16 target, Uint8 subtarget)
{
	// If sound report is defined
	if (report != 0)
	{
		// Play the sound
		pc::sfxeng->PlaySound(report);
	}

	// @todo implemente Anim in [Weapon]
	/*if (fireimage != 0)
	{
		Uint32 length = numfireimages;
		Uint8 facing;
		if (owner->getType()->getNumLayers() == 1)
		{
			facing = (owner->getImageNum(0))&0x1f;
		}
		else
		{
			facing = (owner->getImageNum(1))&0x1f;
		}
		if (!owner->getType()->isInfantry())
		{
			facing >>= 2;
		}
		length /= numfiredirections;
		if (numfiredirections == 1)
		{
			facing = 0;
		}*/
//		new ExplosionAnim(1, owner->getPos(),fireimages[facing],
//				(Uint8)length,/*owner->getXoffset()+*/InfantryGroup::GetUnitOffsets()[owner->getSubpos()],
//				/*owner->getYoffset()+*/InfantryGroup::GetUnitOffsets()[owner->getSubpos()]);


	//}

	ProjectileAnim* proj = new ProjectileAnim(0, this, owner, target, subtarget);
	p::aequeue->scheduleEvent(proj);

	this->Reload();
}

bool Weapon::isHeatseek() const
{
	return heatseek;
}

bool Weapon::isInaccurate() const
{
	return this->projectile->getInaccurate();
}

double Weapon::getVersus(armor_t armor) const
{
	return (this->whead->getVersus(armor)) / (double)100.0;
}

Uint8 Weapon::getFuel() const
{
	return fuel;
}

Uint8 Weapon::getSeekFuel() const
{
	return seekfuel;
}

const char * Weapon::getName() const
{
	return name.c_str();
}

void Weapon::Reload()
{
	if (reloadsound != 0)
	{
		pc::sfxeng->PlaySound(reloadsound);
	}
}
