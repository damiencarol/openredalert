// StringTableFile.cpp
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

#include "StringTableFile.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"

#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

using std::string;
using std::vector;
using std::runtime_error;

extern Logger * logger;

/**
 * @param filename File name to load
 */
StringTableFile::StringTableFile(string filename)
{
	// Load strings in the file "filename"
	this->loadStringFile(filename.c_str());
}

/**
 * @param filename File name to load
 */
StringTableFile::StringTableFile(const char* filename)
{
	// Load strings in the file "filename"
	this->loadStringFile(filename);
}

/**
 * Load all strings in the file specified and stroe it in a std::vector
 * 
 * @param filename File name to load
 */
void StringTableFile::loadStringFile(const char* filename)
{
	VFile*	stringFile; // Ref to the file in the mix (YEAH!)
	Uint16	headerLenght; // size of the header
	Uint16	numString; // Number of string in the file
	
	// Open the File
	stringFile = VFSUtils::VFS_Open(filename);	
	if (stringFile == 0)
	{
		string s = "Unable to open ";
		s += filename;
		throw runtime_error(s);
	}
	
	// Read the first byte (Uint16) to discover the size of the header
	stringFile->readWord(&headerLenght, 1);
	
	// Calculate the number of strings
	numString = headerLenght / 2;
	
		
	// Read offsets
	for (int i = 0; i < numString-1; i++)
	{
		Uint16 bufStart;
		Uint16 bufEnd;
				
		// Seek to header
		stringFile->seekSet(i*2);
		
		// Read the offset of Start in the header
		stringFile->readWord(&bufStart, 1);		
		
		// Read the offset of End in the header
		stringFile->readWord(&bufEnd, 1);		
				
		Uint16 lenght = bufEnd - bufStart;
		Uint8* tabChar = new Uint8[lenght];
		
		// Seek to the str
		stringFile->seekSet(bufStart);				
		
		// read the str
		stringFile->readByte(tabChar, lenght);
		
		string toto = string(((char*)tabChar));
		
		// Add the decoded string in the vector
		data.push_back(toto);
		
		delete tabChar;
	}
		
	// Close the file
	VFSUtils::VFS_Close(stringFile);
}

/**
 * Free only data by calling vector::clear() function.
 */
StringTableFile::~StringTableFile()
{
	// delete all entries in inidata
	this->data.clear();
}

/**
 * Return string by id
 * 
 * @param id Number (id) of the string
 * @return The string with the id specified
 */
string StringTableFile::getString(Uint32 id)
{
	string ret; // String to return
	
	// Copy the string from the internal data
	ret = this->data[id];
	
	// Return the string
	return ret;
}
