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

namespace pc {
	extern vector <SHPImage *> * imagepool;
}

Warhead::Warhead(string name, WarheadDataList* data)
{
	// Get the Data of this warhead
	WarheadData* lWarheadData = data->getData(name);
	
	// Assign the Data
	this->lnkWarheadData = lWarheadData;
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


Uint8 Warhead::getVersus(armour_t armour)
{
	switch (armour) {
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
