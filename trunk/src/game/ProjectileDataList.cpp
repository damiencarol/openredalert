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
    

void ProjectileDataList::loadProjectileData(INIFile* file, string name)
{
	// Save a ref to the ProjectileData
	data[name] = ProjectileData::loadProjectileData(file, name);
}

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
