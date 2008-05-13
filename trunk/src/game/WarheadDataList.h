#ifndef WARHEADDATALIST_H
#define WARHEADDATALIST_H

#include "misc/INIFile.h"

#include "misc/INIFile.h"

class WarheadData;

class WarheadDataList {
public:    

    void loadWarheadData(INIFile* file, string name);
    
    WarheadData* getData(string name);
    int size();
    void print();
private:
	map < string, WarheadData * > data;
	
    /** @link association */
    /*# WarheadData * lnkWarheadData; */
};
#endif //WARHEADDATALIST_H
