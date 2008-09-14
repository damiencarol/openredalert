// weaponspool.cpp
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

#include "weaponspool.h"

#include <map>
#include <string>
#include <algorithm>

#include "misc/INIFile.h"
#include "include/Logger.h"
#include "ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "include/config.h"
#include "Projectile.h"
#include "Warhead.h"
#include "ExplosionAnim.h"
#include "video/ImageNotFound.h"

#include "Weapon.h"

#include "misc/INIFile.h"

using std::map;
using std::string;

/**
 *
 */
WeaponsPool::WeaponsPool()
{
	// Load weapons file
	#ifdef __MORPHOS__
		// "./ra/" path cause trouble in morphos. Maybe we could apply this modification to all plateforms since VFS find the good path ?
	weapini = GetConfig("weapons.ini");
	#else
	weapini = GetConfig("./ra/weapons.ini");
	#endif
	// @todo weapini = VFSUtils::VFS_Open("rules.ini");
}

/**
 *
 */
WeaponsPool::~WeaponsPool()
{
	map<string, Weapon*>::const_iterator wpclear;
	map<string, Warhead*>::const_iterator whpclear;
	map<string, Projectile*>::const_iterator ppclear;

	wpclear = weaponspool.begin();
	whpclear = warheadpool.begin();
	ppclear = projectilepool.begin();

	while (wpclear != weaponspool.end())
	{
		delete wpclear->second;
		++wpclear;
	}
	while (whpclear != warheadpool.end())
	{
		delete whpclear->second;
		++whpclear;
	}
	while (ppclear != projectilepool.end())
	{
		delete ppclear->second;
		++ppclear;
	}
}

Weapon *WeaponsPool::getWeapon(const char *wname)
{
	map<string, Weapon*>::iterator weapentry;
	Weapon *weap;
	string weapname;

	weapname = (string)wname;
	// @todo DEBUG
	//transform(weapname.begin(), weapname.end(), weapname.begin(), toupper);
	weapentry = weaponspool.find(weapname);

	if (weapentry == weaponspool.end() )
	{
		try
		{
			weap = new Weapon(wname);
		}
		catch( int )
		{
			// if weapon is NULL unit doesn't have a weapon
			return NULL;
		}
		weaponspool[weapname] = weap;
		return weap;
	}
	else
	{
		return weapentry->second;
	}
}

INIFile *WeaponsPool::getWeaponsINI()
{
	return weapini;
}

