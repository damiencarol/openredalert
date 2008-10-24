// INIFile.h
//
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

#include "INISection.h"

using std::string;
using std::map;

/**
 * Parses inifiles.
 *
 */
class INIFile
{
public:
    explicit INIFile(const string& filename);
    ~INIFile();

    /** Read a String */
    string readString(const string& section, const string& key) const;
    /** Read a string and return the default value if the key not exist */
    string readString(const string& section, const string& key, const string& defaultValue) const;

    /** Read an integer */
    int readInt(const string& section, const string& key) const;
    /** Read an integer and return the default value if the key not exist */
    int readInt(const string& section, const string& key, const int defaultValue) const;
    
    float readFloat(const string& section, const string& key);
    float readFloat(const string& section, const string& key, const float defaultValue);

    INISection::const_iterator readKeyValue(const char* section, unsigned int keynum);
    INISection::const_iterator readIndexedKeyValue(const char* section, unsigned int keynum, const char* prefix=0);
    string readSection(unsigned int secnum);

    /** Read a Key with value equal 'yes' or 'no' */
    int readYesNo(const string& section, const string& value, const int defaultValue) const;

    /** Function to test if a section is in the inifile */
    bool isSection(const string& section) const;
    /** Function to test if a key is in a section in the inifile */
    bool isKeyInSection(const string& section, const string& keyString) const;

    /** Function to get number of key/value per section */
    int getNumberOfKeysInSection(const string& section) const;
    
    /** Get the file name of the ini file */
    string getFileName() const;

private:
	/** File name of the inifile loaded */
	string filename;
	
    /** Internal data */
    map<string, INISection> Inidata;
};

#endif //INIFILE_H
