// WarheadDataList.h
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

#ifndef WARHEADDATALIST_H
#define WARHEADDATALIST_H

#include "misc/INIFile.h"

class WarheadData;

class WarheadDataList
{
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
