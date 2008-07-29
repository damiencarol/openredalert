// StringTableFile.h
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

#ifndef STRINGTABLEFILE_H
#define STRINGTABLEFILE_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"

using std::string;
using std::vector;

/**
 * Utility Class to read located strings in the original mix archives files
 * 
 * @author Damien Carol
 * @version 1.0
 */
class StringTableFile {
public:
	/** Create a StringTableFile object and load all strings in the file specified */
    StringTableFile(string filename);
    /** Create a StringTableFile object and load all strings in the file specified */
    StringTableFile(const char * filename);
    /** Destructor */
    ~StringTableFile();    
    
    /** Get the string by this id number */
    string getString(Uint32 id);    
private:
	/** Load all strings in a file from mix archive */
	void loadStringFile(const char* filename);	
	
	/** List of string loaded from the file */
    vector <string> data;
};

#endif //STRINGTABLEFILE_H
