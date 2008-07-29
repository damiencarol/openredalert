// ProjectileDataList.cpp
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

#include "ProjectileDataList.h"

#include <iterator>
#include <string>
#include <iostream>

#include "ProjectileData.h"
#include "misc/INIFile.h"
#include "include/Logger.h"

using std::string;
using std::iterator;
using std::cout;
using std::endl;

extern Logger * logger;

/**
 */
ProjectileData* ProjectileDataList::getData(string name)
{
	map < string, ProjectileData * >::iterator itRecherche;
		
	itRecherche=data.find(name);
	if (itRecherche != data.end())
	{
		// Return the ProjectileData found
		//ut << (*itRecherche).second->getWall() << endl;
		return (*itRecherche).second;
	}	
	
	// If here NO PROJECTILEDATA FOUND
	logger->error("PROJECTILEDATA [%s] not found in DataList !!!\n", name.c_str());
	
	// Return new ProjectileData by default
	logger->warning("New virtual ProjectileData was returned...\n");
	return new ProjectileData();
}
    
/**
 */
void ProjectileDataList::loadProjectileData(INIFile* file, string name)
{
	// Save a ref to the ProjectileData
	data[name] = ProjectileData::loadProjectileData(file, name);
}

/**
 */
void ProjectileDataList::print()
{
	map < string, ProjectileData* >::iterator itRecherche;
	
	itRecherche = data.begin();
	while (itRecherche != data.end())
	{
		cout << "[" << (*itRecherche).first << "]" << endl;		
		(*itRecherche).second->print();
		
		itRecherche++;		
	}
}
