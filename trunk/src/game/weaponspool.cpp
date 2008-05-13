#include "include/weaponspool.h"

#include <map>
#include <string>
#include <algorithm>

#include "misc/INIFile.h"
#include "include/Logger.h"
#include "include/ProjectileAnim.h"
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

WeaponsPool::WeaponsPool()
{
	//weapini = GetConfig("weapons.ini");
	if (getConfig().gamenum == GAME_RA)
	{
		printf("%s line %i: Loading ra weapens ini\n",__FILE__ , __LINE__);
		weapini = GetConfig("./ra/weapons.ini");
	}
	else
	{
		weapini = GetConfig("./td/weapons.ini");
	}
}

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
	// TODO DEBUG
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

