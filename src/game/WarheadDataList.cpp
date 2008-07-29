// WarheadDataList.cpp
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

#include "WarheadDataList.h"

#include <iostream>
#include <string>

#include "misc/INIFile.h"
#include "include/Logger.h"
#include "DataLoader.h"
#include "WarheadData.h"

#include "misc/INIFile.h"

using std::string;
using std::cout;
using std::endl;

extern Logger * logger;

int WarheadDataList::size()
{
	return data.size();
}

WarheadData* WarheadDataList::getData(string name)
{
	map < string, WarheadData * >::iterator itRecherche;
	
	
	itRecherche=data.find(name);
	if (itRecherche != data.end())
	{
		cout << (*itRecherche).second->getWall() << endl;
		return (*itRecherche).second;
	}	
	//return this->data[name];
	logger->error("Unenable to load [%s] WARHEAD !!!", name.c_str());
	
	// Return new Warhead by default
	logger->warning("New virtual warhead was returned...");
	return new WarheadData();
}
   

void WarheadDataList::loadWarheadData(INIFile* file, string name)
{
	// Load the Warhead
	WarheadData* ptrWaH = WarheadData::loadWarheadData(file, name);
	
	// @todo DEBUG
	if (ptrWaH == NULL) {
		
	}
	
	// Put it in the DataList
	//this->data[name] = ptrWaH;
	
	//this->data.insert(map < string, WarheadData * >::value_type(name, ptrWaH));
	data[name]=ptrWaH;
}

void WarheadDataList::print()
{
	map < string, WarheadData* >::iterator itRecherche;
	
	itRecherche = data.begin();
	while (itRecherche != data.end())
	{
		cout << "[" << (*itRecherche).first << "]" << endl;		
		(*itRecherche).second->print();
		
		itRecherche++;		
	}
}
