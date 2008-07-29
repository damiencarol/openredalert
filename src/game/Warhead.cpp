// Warhead.cpp
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

#include "Warhead.h"

#include <vector>
#include <string>
#include <iostream>
#include <iterator>

#include "video/ImageNotFound.h"
#include "video/SHPImage.h"
#include "include/Logger.h"
#include "WarheadData.h"
#include "WarheadDataList.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::iterator;

extern Logger * logger;

namespace pc
{
extern vector <SHPImage *> * imagepool;
}

Warhead::Warhead(string name, WarheadDataList* data)
{
	// Get the Data of this warhead
	WarheadData* lWarheadData = data->getData(name);

	// Assign the Data
	this->lnkWarheadData = lWarheadData;
	
	// Loading of the Explosion Anim
	this->explosionImages = 0;
	
	// switch with Explosion type
	switch (this->lnkWarheadData->getExplosion())
	{
	case 0:
		// If explosion type = 0(none) no anim
		this->explosionImages = 0;
		break;
	case 1:
		// If explosion type = 1(piff) anim "piff.shp"
		this->explosionImages = new SHPImage("piff.shp", -1);
		break;
	case 2:
		// If explosion type = 2(piffs) anim "piffpiff.shp"
		this->explosionImages = new SHPImage("piffpiff.shp", -1);
		break;		
	case 3:
		// If explosion type = 3(fire) anim "fire1.shp"
		/// @todo test if it's the good animation
		this->explosionImages = new SHPImage("fire1.shp", -1);
		break;		
	case 4:
		// If explosion type = 4(frags) anim "frag1.shp"
		/// @todo test if it's the good animation
		this->explosionImages = new SHPImage("frag1.shp", -1);
		break;		
	case 5:
		// If explosion type = 5(pops) anim "piffpiff.shp"
		/// @todo test if it's the good animation
		this->explosionImages = new SHPImage("piffpiff.shp", -1);
		break;		
	case 6:
		// If explosion type = 6(nuke) anim "atomsfx.shp"
		this->explosionImages = new SHPImage("atomsfx.shp", -1);
		break;		
	default:
		this->explosionImages = 0;
		break;
	}
}

Warhead::~Warhead()
{
	// Free the Data
	delete this->lnkWarheadData;
}

bool Warhead::getWall()
{
	return this->lnkWarheadData->getWall();
}

WarheadData* Warhead::getType()
{
	return this->lnkWarheadData;
}

Uint8 Warhead::getVersus(armor_t armor)
{
	switch (armor)
	{
	case AC_none:
		return this->lnkWarheadData->getVersusNone();
	case AC_wood:
		return this->lnkWarheadData->getVersusWood();
	case AC_light:
		return this->lnkWarheadData->getVersusLight();
	case AC_heavy:
		return this->lnkWarheadData->getVersusHeavy();
	case AC_concrete:
		return this->lnkWarheadData->getVersusConcrete();
	default:
		return this->lnkWarheadData->getVersusNone();
	}
}
