// WarheadData.cpp
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

#include "WarheadData.h"

#include <iostream>

#include "SDL/SDL_types.h"

#include "misc/INIFile.h"
#include "include/Logger.h"
#include "include/common.h"

#include "misc/INIFile.h"

using std::cout;
using std::endl;

extern Logger *logger;

Uint32 WarheadData::getSpread()
{
	return spread;
}
void WarheadData::setSpread(Uint32 spread)
{
	this->spread = spread;
}
Uint32 WarheadData::getWall()
{
	return wall;
}
void WarheadData::setWall(Uint32 wall)
{
	this->wall = wall;
}
Uint32 WarheadData::getWood()
{
	return wood;
}
void WarheadData::setWood(Uint32 wood)
{
	this->wood = wood;
}
Uint32 WarheadData::getOre()
{
	return ore;
}
void WarheadData::setOre(Uint32 ore)
{
	this->ore = ore;
}
Uint32 WarheadData::getVersusNone()
{
	return versusNone;
}
void WarheadData::setVersusNone(Uint32 versusNone)
{
	this->versusNone = versusNone;
}
Uint32 WarheadData::getVersusWood()
{
	return versusWood;
}
void WarheadData::setVersusWood(Uint32 versusWood)
{
	this->versusWood = versusWood;
}
Uint32 WarheadData::getVersusLight()
{
	return versusLight;
}
void WarheadData::setVersusLight(Uint32 versusLight)
{
	this->versusLight = versusLight;
}
Uint32 WarheadData::getVersusHeavy()
{
	return versusHeavy;
}
void WarheadData::setVersusHeavy(Uint32 versusHeavy)
{
	this->versusHeavy = versusHeavy;
}
Uint32 WarheadData::getVersusConcrete()
{
	return versusConcrete;
}
void WarheadData::setVersusConcrete(Uint32 versusConcrete)
{
	this->versusConcrete = versusConcrete;
}
Uint32 WarheadData::getExplosion()
{
	return explosion;
}
void WarheadData::setExplosion(Uint32 explosion)
{
	this->explosion = explosion;
}
Uint32 WarheadData::getInfDeath()
{
	return infDeath;
}
void WarheadData::setInfDeath(Uint32 infDeath)
{
	this->infDeath = infDeath;
}

WarheadData* WarheadData::loadWarheadData(INIFile * file, string name)
{
	WarheadData* ptrWarheadData;

	// Create the WarheadData object
	ptrWarheadData = new WarheadData();

	// Spread = damage spread factor [larger means greater spread] (def=1)
	//  [A value of 1 means the damage is halved every pixel distant from
	// center point.
	//   a value of 2 means damage is halved every 2 pixels, etc.]
	Uint32 tmpSpread = file->readInt(name.c_str(), "Spread", 1);
	ptrWarheadData->setSpread(tmpSpread);

	// Wall = Does this warhead damage concrete walls (def=no)?
	char* tmpPtWall = file->readString(name.c_str(), "Wall", "no");
	string tmpWall = (string)tmpPtWall;
	Uint32 a;
	if (tmpWall == "yes")
	{
		a = 1;
	}
	else
	{
		a = 0;
	}
	delete[] tmpPtWall;
	ptrWarheadData->setWall(a);

	// Ore = Does this warhead destroy ore (def=no)?
	char* tmpPtOre = file->readString(name.c_str(), "Ore", "no");
	string tmpOre = (string)tmpPtOre;
	Uint32 b;
	if (tmpOre == "yes")
	{
		b = 1;
	}
	else
	{
		b = 0;
	}
	delete[] tmpPtOre;
	ptrWarheadData->setOre(b);
	
	// Verses = damage value verses various armor types (as percentage 
	// of full damage)...
	// -vs- none, wood (buildings), light armor, heavy armor, concrete
	char* tmpPtVerses = file->readString(name.c_str(), "Verses");
	if (tmpPtVerses != NULL) {
		Uint32 versus[5];
		
		versus[0] = 100;
		versus[1] = 100;
		versus[2] = 100;
		versus[3] = 100;
		versus[4] = 100;
				
		sscanf(splitList(tmpPtVerses, ',')[0], "%u", &versus[0]);
		sscanf(splitList(tmpPtVerses, ',')[1], "%u", &versus[1]);
		sscanf(splitList(tmpPtVerses, ',')[2], "%u", &versus[2]);
		sscanf(splitList(tmpPtVerses, ',')[3], "%u", &versus[3]);
		sscanf(splitList(tmpPtVerses, ',')[4], "%u", &versus[4]);
	
		delete[] tmpPtVerses;
				
		ptrWarheadData->setVersusNone(versus[0]);
		ptrWarheadData->setVersusWood(versus[1]);
		ptrWarheadData->setVersusLight(versus[2]);
		ptrWarheadData->setVersusHeavy(versus[3]);
		ptrWarheadData->setVersusConcrete(versus[4]);				
	}
	
	// Explosion = which explosion set to use when warhead of this
	// type impacts (def=0)
	// 0=none, 1=piff, 2=piffs, 3=fire, 4=frags, 5=pops, 6=nuke
	Uint32 tmpExplosion = file->readInt(name.c_str(), "Explosion", 0);
	ptrWarheadData->setExplosion(tmpExplosion);

	// InfDeath = which infantry death animation to use (def=0)
	// 0=instant die, 1=twirl die, 2=explodes, 3=flying death, 
	// 4=burn death, 5=electro
	Uint32 tmpInfDeath = file->readInt(name.c_str(), "InfDeath", 0);
	ptrWarheadData->setInfDeath(tmpInfDeath);

	// Returns the constructed object
	return ptrWarheadData;
}

/**
 * Print the Data
 */
void WarheadData::print()
{
	cout << "Spread=" << getSpread() << endl;
	cout << "Wall=" << getWall() << endl;
	cout << "Wood=" << getWood() << endl;
	cout << "Ore=" << getOre() << endl;
	cout << "Verses=" << getVersusNone() << "%," << getVersusWood() << "%,"
			<< getVersusLight() << "%," << getVersusHeavy() << "%,"
			<< getVersusConcrete() << "%" << endl;
	cout << "Explosion=" << getExplosion() << endl;
	cout << "InfDeath=" << getInfDeath() << endl;
}

