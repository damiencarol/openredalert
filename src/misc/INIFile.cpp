// INIFile.cpp
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

#include "INIFile.h"

#include <iosfwd>
#include <map>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "KeyNotFound.h"
#include "include/common.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

using std::string;
using std::stringstream;
using std::map;
using std::runtime_error;

using INI::KeyNotFound;

extern Logger * logger;

/**
 * Constructor, opens the file
 *
 * @param filename the name of the inifile to open.
 */
INIFile::INIFile(const char* filename)
{
	char line[1024];
	char key[1024];
	char value[1024];
	Uint32 i;
	char* str;

	VFile* inifile;
	string cursectionName;
	//    INISection cursection;
	INISection newSection;
	IniEntry Entry;

	//int MAXLINELENGTH = 1024;
	//int MAXSTRINGLENGTH = 128;


	// Open the File
	inifile = VFSUtils::VFS_Open(filename);
	if (inifile == 0) {
		string s = "Unable to open ";
		s += filename;
		logger->error("%s\n", s.c_str());

		throw runtime_error(s);
	}

	cursectionName = "";

	// parse the inifile and write data to inidata
	while (inifile->getLine(line, 1024) != 0) {
		str = line;

		while ((*str) == ' ' || (*str) == '\t') {
			str++;
		}
		if ((*str) == ';') {
			continue;
		}
		if (sscanf(str, "[%[^]]]", key) == 1) {
			if (cursectionName != "") {
				//inidata[cursectionName] = cursection;
				Inidata[cursectionName] = newSection;
				//                cursection.clear();
				newSection.clear();
			}
			for (i = 0; key[i] != '\0'; i++) {
				key[i] = toupper(key[i]);
			}
			cursectionName = key;
		} else if (cursectionName != "" && sscanf(str, "%[^=]=%[^\r\n;]", key,
				value) == 2) {
			for (i = 0; key[i] != '\0'; i++) {
				key[i] = toupper(key[i]);
			}
			if (strlen(key) > 0) {
				str = key + strlen(key) - 1;
				while ((*str) == ' ' || (*str) == '\t') {
					(*str) = '\0';
					if (str == key) {
						break;
					}
					str--;
				}
			}
			if (strlen(value) > 0) {
				str = value + strlen(value) - 1;
				while ((*str) == ' ' || (*str) == '\t') {
					(*str) = '\0';
					if (str == value) {
						break;
					}
					str--;
				}
			}
			str = value;
			while ((*str) == ' ' || (*str) == '\t') {
				str++;
			}
			//cursection[(string)key] = (string)str;
			Entry.first = (string) key;
			Entry.second = (string) str;
			newSection.push_back(Entry);
			//            std::pair entry ((string)key, (string)str);
			//            newSection.push_back (key, str);
		}
	}
	if (cursectionName != "") {
		//inidata[cursectionName] = cursection;
		Inidata[cursectionName] = newSection;
		//        cursection.clear();
		newSection.clear();
	}

	// Close the file
	VFSUtils::VFS_Close(inifile);
}

/**
 * Destructor, closes the file
 */
INIFile::~INIFile() {
	// delete all entries in inidata
	this->Inidata.clear();
}

/**
 * Function to extract a string from a ini file. The string is mallocated
 * in this function so it should be freed.
 *
 * @param section the section in the file to extract string from.
 * @param value the name of the string to extract.
 * @return the extracted string.
 */
char* INIFile::readString(const char* section, const char* value) {
	char* retval = 0; // Return value

	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);
	string v = value;
	transform(v.begin(), v.end(), v.begin(), toupper);

	// Try to find the key
	map<string, INISection>::iterator sec = Inidata.find(s);
	if (sec == Inidata.end()) {
		return 0;
	}

	// get a const iterator
	INIKey key = sec->second.begin();

	for (unsigned int i = 0; i < sec->second.size(); i++) {
		if (key->first == v) {
			break;
		}
		key++;
	}

	if (key == sec->second.end()) {
		return 0;
	}
	// allocate a new string
	retval = new char[key->second.length() + 1];
	// copy value
	strcpy(retval, key->second.c_str());
	// return the good one
	return retval;
}

/**
 * wrapper around readString to return a provided default instead of NULL
 */
char* INIFile::readString(const char* section, const char* value,
		const char* deflt) {
	char* tmp;

	tmp = readString(section, value);
	if (tmp == 0) {
		// a new string is allocated because this guarentees
		// that the return value can be delete[]ed safely
		tmp = cppstrdup(deflt);
	}
	// Return the new string
	return tmp;
}

/**
 *
 */
int INIFile::readInt(const char* section, const char* value, Uint32 deflt) {
	try {
		// Try to return the value
		return readInt(section, value);
	} catch (KeyNotFound&) {
		// Log it
		//logger->warning("Key [%s]%s was not found. Default value %d was return instead.\n", section, value, deflt);
		// Return the default value
		return deflt;
	} catch (...) {
		// Log it
		logger->error(
				"Error in reading key <[%s]%s>. Default value %d was return instead.\n",
				section, value, deflt);
		// Return the default value
		return deflt;
	}
}

/**
 * Function to extract a integer value from a ini file. The value
 * can be given in hex if it starts with 0x.
 *
 * @param section the section in the file to extract values from.
 * @param value the name of the value to extract.
 * @return the value.
 */
int INIFile::readInt(const char* section, const char* value) {
	int retval;
	map<string, INISection>::iterator sec;
	INIKey key;

	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);
	string v = value;
	transform(v.begin(), v.end(), v.begin(), toupper);

	sec = Inidata.find(s);
	if (sec == Inidata.end()) {
		throw KeyNotFound("Can't find the section [" + string(section)
				+ "] in ini file.");
	}

	//    key = sec->second.find(v);
	key = sec->second.begin();

	for (unsigned int i = 0; i < sec->second.size(); i++) {
		if (key->first == v) {
			break;
		}
		key++;
	}

	if (key == sec->second.end()) {
		throw KeyNotFound("Can't Find the key [" + string(value)
				+ "] in the section [" + string(section) + "] of ini file.");
	}
	if (sscanf(key->second.c_str(), "%d", &retval) != 1) {
		throw runtime_error("Unable to cast to int.");
	}
	return retval;
}

/**
 * Function to extract a float value from a ini file. The value
 * can be given in hex if it starts with 0x.
 *
 * @param section the section in the file to extract values from.
 * @param value the name of the value to extract.
 * @return the value.
 */
float INIFile::readFloat(const char* section, const char* value) {
	float retval;
	map<string, INISection>::iterator sec;
	INIKey key;

	// Get the "UPPER" strings
	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);
	string v = value;
	transform(v.begin(), v.end(), v.begin(), toupper);

	// Check if the section is in the file
	sec = Inidata.find(s);
	if (sec == Inidata.end()) {
		throw new KeyNotFound("The section [" + string(section)
				+ "] was not found in the file.");
	}

	// Try to find the section (by iterations)
	key = sec->second.begin();
	for (unsigned int i = 0; i < sec->second.size(); i++) {
		if (key->first == v) {
			break;
		}
		key++;
	}

	// If the key was not found throw an INI::KeyNotFound exception
	if (key == sec->second.end()) {
		throw new KeyNotFound("Key " + string(section) + ":" + string(value)
				+ " was not found in the ini file.");
	}
	// If we can't "sscan" in float
	if (sscanf(key->second.c_str(), "%f", &retval) != 1) {
		throw new runtime_error("Key " + string(section) + ":" + string(value)
				+ "=" + string(key->second.c_str())
				+ " can't be cast in float.");
	}

	// success return the value
	return retval;
}

/**
 * Read a value in a section of IniFile
 *
 * @param section section of the ini file
 * @param value key to read in the section
 * @param deflt default value to return
 */
float INIFile::readFloat(const char* section, const char* value, float deflt) {
	// Try to read the value
	try {
		return readFloat(section, value);
	} catch (INI::KeyNotFound& ex) {
		// If exception "Not key found" then return default value
		return deflt;
	}
}

/**
 * Function to get number of key/value per section
 */
int INIFile::getNumberOfKeysInSection(string section) {
	map<string, INISection>::iterator sec_new;
	INIKey Key;
	string s;

	// get the string from (char*) section
	s = string(section);

	// Upper the section string
	transform(s.begin(), s.end(), s.begin(), toupper);

	sec_new = Inidata.find(s);
	if (sec_new == Inidata.end()) {
		//throw KeyNotFound("Section [" + string(section) + "] not found in .ini file.");
		return 0;
	}

	return sec_new->second.size();
}

/**
 * Use inside a loop to read all keys of a section.  The order is as read
 * from the inifile.
 *
 * @param section The name of the section from which to read.
 * @param keynum Will skip (keynum-1) entries in section.
 * @returns an iterator to the keynum'th key in section.
 */
INIKey INIFile::readKeyValue(const char* section, Uint32 keynum) {
	map<string, INISection>::iterator sec_new;
	INIKey Key;
	string s;

	// get the string from (char*) section
	s = string(section);

	// Upper the section string
	transform(s.begin(), s.end(), s.begin(), toupper);

	sec_new = Inidata.find(s);
	if (sec_new == Inidata.end()) {
		throw KeyNotFound("Section [" + string(section)
				+ "] not found in .ini file.");
	}

	if (keynum >= sec_new->second.size()) {
		throw KeyNotFound("Key number [???] in Section [" + string(section)
				+ "] not found in .ini file.");
	}

	Key = sec_new->second.begin();
	for (Uint32 i = 0; i < keynum; ++i) {
		Key++;
	}
	if (Key == sec_new->second.end()) {
		logger->error("throw 0 in INIFile::readKeyValue()\n");
		throw KeyNotFound("Key number [????] in Section [" + string(section)
				+ "] not found in .ini file.");
	}
	return Key;
}

/**
 */
INIKey INIFile::readIndexedKeyValue(const char* section, Uint32 index,
		const char* prefix) {
	map<string, INISection>::iterator sec;
	INIKey key;
	//	char TempString[255];
	std::stringstream TempStr;

	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);

	sec = Inidata.find(s);
	if (sec == Inidata.end()) {
		throw 0;
	}

	if (index > sec->second.size()) {
		throw 0;
	}

	string keyval;
	if (prefix)
		keyval = prefix;
	//keyval += lexical_cast<string>(index);

	TempStr.str("");
	TempStr << (unsigned int) index;
	//	sprintf (TempString, "%u", index);
	//	keyval += TempString;
	keyval += TempStr.str();

	key = sec->second.begin();

	for (unsigned int i = 0; i < sec->second.size(); i++) {
		if (key->first == keyval) {
			return key;
		}
		key++;
	}
	if (key == sec->second.end()) {
		throw 0;
	}
	// we should never get here
	return key;

}

string INIFile::readSection(Uint32 secnum) {
	map<string, INISection>::iterator sec;
	Uint32 i;

	if (secnum >= Inidata.size()) {
		throw 0;
	}
	sec = Inidata.begin();
	for (i = 0; i < secnum; i++) {
		sec++;
		if (sec == Inidata.end()) {
			throw 0;
		}
	}
	return sec->first;
}

bool INIFile::isSection(string section) {
	map<string, INISection>::iterator sec;

	sec = Inidata.find(section);
	if (sec == Inidata.end()) {
		return false;
	} else {
		return true;
	}

}

/**
 * @param section Section to check
 * @param keyString Key to check
 * @return <code>true</code> if the key exist in the section else return <code>false</code>
 */
bool INIFile::isKeyInSection(const string& section, const string& keyString)
{
	// Upper version of the section
	string s = section;
	// Upper the section string
	transform(s.begin(), s.end(), s.begin(), toupper);

	// Try to get the section
	map<string, INISection>::iterator sec = Inidata.find(s);

	// If the section was not found
	if (sec == Inidata.end())
	{
		// Return false
		return false;
	}
	else
	{
		// Upper version of the key string
		string k = keyString;
		// Upper the key string
		transform(k.begin(), k.end(), k.begin(), toupper);

		// Get an iterator
		INIKey sec_key = sec->second.begin();
		while (sec_key!=sec->second.end())
		{
			// If the Key was found
			if (sec_key->first == k)
			{
				// Return true
				return true;
			}
			// Continue
			sec_key++;
		}
	}
	// Nothing found, return false
	return false;
}

int INIFile::readYesNo(const char* section, const char* value,
		const char* defaut) {
	char* tmpPtAA = this->readString(section, value, defaut);
	string tmpAA = (string) tmpPtAA;
	Uint32 a;
	if (tmpAA == "yes") {
		a = 1;
	} else {
		a = 0;
	}
	delete[] tmpPtAA;

	return a;
}
