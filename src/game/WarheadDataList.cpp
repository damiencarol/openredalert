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
	
	// TODO DEBUG
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
