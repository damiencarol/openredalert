// INIFile.h
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

#ifndef INIFILE_H
#define INIFILE_H

#include <map>
#include <string>
#include <vector>
#include <list>

#include "SDL/SDL_types.h"
#include "IniEntry.h"
#include "INIKey.h"

using std::string;
using std::map;
using std::vector;
using std::list;


/**
 * Parses inifiles.
 *
 */
class INIFile
{
public:
    explicit INIFile(const char* filename);
    ~INIFile();

    /// @todo Would be nice if there was a version that returned a non-copy.
    char* readString(const char* section, const char* value);
    char* readString(const char* section, const char* value, const char* deflt);

    int readInt(const char* section, const char* value, Uint32 deflt);
    int readInt(const char* section, const char* value);

    float readFloat(const char* section, const char* value);
    float readFloat(const char* section, const char* value, float deflt);

    INIKey readKeyValue(const char* section, Uint32 keynum);
    INIKey readIndexedKeyValue(const char* section, Uint32 keynum, const char* prefix=0);
    string readSection(Uint32 secnum);

    int readYesNo(const char* section, const char* value, const char* defaut);

    /** Function to test if a section is in the inifile */
    bool isSection(string section);
    /** Function to test if a key is in a section in the inifile */
    bool isKeyInSection(const string& section, const string& keyString);

    /** Function to get number of key/value per section */
    int getNumberOfKeysInSection(string section);

private:
    /** Internal data */
    map<string, INISection> Inidata;
};

#endif //INIFILE_H
