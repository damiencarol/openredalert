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
 * @param the name of the inifile to open.
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


	// TODO DEBUG
	//logger->debug("INIFile: Loading::%s\n", filename);

	// Open the File
	inifile = VFSUtils::VFS_Open(filename);
	if (inifile == 0)
	{
		string s = "Unable to open ";
		s += filename;
		logger->error("%s\n", s.c_str());
		
		throw runtime_error(s);
	}

	// TODO DEBUG
	//logger->debug("... [OK]\n");

	cursectionName = "";

	// parse the inifile and write data to inidata
	while (inifile->getLine(line, 1024) != 0)
	{
		str = line;

		// TODO DEBUG
		//logger->debug("%s\n", str);

		while ((*str) == ' ' || (*str) == '\t')
		{
			str++;
		}
		if ((*str) == ';')
		{
			continue;
		}
		if (sscanf(str, "[%[^]]]", key) == 1)
		{
			if (cursectionName != "")
			{
				//inidata[cursectionName] = cursection;
				Inidata[cursectionName] = newSection;
				//                cursection.clear();
				newSection.clear();
			}
			for (i = 0; key[i] != '\0'; i++)
			{
				key[i] = toupper(key[i]);
			}
			cursectionName = key;
		}
		else if (cursectionName != "" && sscanf(str, "%[^=]=%[^\r\n;]", key,
				value) == 2)
		{
			for (i = 0; key[i] != '\0'; i++)
			{
				key[i] = toupper(key[i]);
			}
			if (strlen(key) > 0)
			{
				str = key+strlen(key)-1;
				while ((*str) == ' ' || (*str) == '\t')
				{
					(*str) = '\0';
					if (str == key)
					{
						break;
					}
					str--;
				}
			}
			if (strlen(value) > 0)
			{
				str = value+strlen(value)-1;
				while ((*str) == ' ' || (*str) == '\t')
				{
					(*str) = '\0';
					if (str == value)
					{
						break;
					}
					str--;
				}
			}
			str = value;
			while ((*str) == ' ' || (*str) == '\t')
			{
				str++;
			}
			//cursection[(string)key] = (string)str;
			Entry.first = (string)key;
			Entry.second = (string)str;
			newSection.push_back(Entry);
			//            std::pair entry ((string)key, (string)str);
			//            newSection.push_back (key, str);
		}
	}
	if (cursectionName != "")
	{
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
INIFile::~INIFile()
{
	// delete all entries in inidata
	this->Inidata.clear();
}

/** 
 * Function to extract a string from a ini file. The string is mallocated 
 * in this function so it should be freed.
 * 
 * @param the section in the file to extract string from.
 * @param the name of the string to extract.
 * @return the extracted string.
 */
char* INIFile::readString(const char* section, const char* value)
{
	char* retval;
	map<string, INISection>::iterator sec;
	INIKey key;

	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);
	string v = value;
	transform(v.begin(), v.end(), v.begin(), toupper);

	sec = Inidata.find(s);
	if (sec == Inidata.end())
	{
		return 0;
	}

	//    key = sec->second.find(v);
	key = sec->second.begin();

	for (unsigned int i = 0; i < sec->second.size(); i++)
	{
		if (key->first == v)
		{
			break;
		}
		key++;
	}

	if (key == sec->second.end())
	{
		return 0;
	}
	// allocate a new string
	retval = new char[key->second.length()+1];
	return strcpy(retval, key->second.c_str());
}
/**
 * wrapper around readString to return a provided default instead of NULL
 */
char* INIFile::readString(const char* section, const char* value,
		const char* deflt)
{
	char* tmp;

	tmp = readString(section, value);
	if (tmp == 0)
	{
		/* a new string is allocated because this guarentees
		 * that the return value can be delete[]ed safely 
		 */
		tmp = cppstrdup(deflt);
	}
	return tmp;
}

int INIFile::readInt(const char* section, const char* value, Uint32 deflt)
{
	try
	{
		// Try to return the value
		return readInt(section, value);
	}
	catch (KeyNotFound&)
	{
		// Log it
		//logger->warning("Key [%s]%s was not found. Default value %d was return instead.\n", section, value, deflt);
		// Return the default value
		return deflt;
	}
	catch (...)
	{
		// Log it
		logger->error("Error in reading key <[%s]%s>. Default value %d was return instead.\n", section, value, deflt);
		// Return the default value
		return deflt;
	}
}

/** 
 * Function to extract a integer value from a ini file. The value
 * can be given in hex if it starts with 0x.
 * 
 * @param the section in the file to extract values from.
 * @param the name of the value to extract.
 * @return the value.
 */
int INIFile::readInt(const char* section, const char* value)
{
	int retval;
	map<string, INISection>::iterator sec;
	INIKey key;

	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);
	string v = value;
	transform(v.begin(), v.end(), v.begin(), toupper);

	sec = Inidata.find(s);
	if (sec == Inidata.end())
	{
		throw KeyNotFound("Can't find the section [" + string(section) + "] in ini file.");
	}

	//    key = sec->second.find(v);
	key = sec->second.begin();

	for (unsigned int i = 0; i < sec->second.size(); i++)
	{
		if (key->first == v)
		{
			break;
		}
		key++;
	}

	if (key == sec->second.end())
	{
		throw KeyNotFound("Can't Find the key [" + string(value) + "] in the section [" + string(section) + "] of ini file.");
	}
	if (sscanf(key->second.c_str(), "%d", &retval) != 1)
	{
		throw runtime_error("Unable to cast to int.");
	}
	return retval;
}

/** 
 * Function to extract a float value from a ini file. The value
 * can be given in hex if it starts with 0x.
 * 
 * @param the section in the file to extract values from.
 * @param the name of the value to extract.
 * @return the value.
 */
float INIFile::readFloat(const char* section, const char* value)
{
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
	if (sec == Inidata.end())
	{
		throw new KeyNotFound("The section [" + string(section) + "] was not found in the file.");
	}

	// Try to find the section (by iterations)
	key = sec->second.begin();
	for (unsigned int i = 0; i < sec->second.size(); i++)
	{
		if (key->first == v)
		{
			break;
		}
		key++;
	}

	// If the key was not found throw an INI::KeyNotFound exception
	if (key == sec->second.end())
	{
		throw new KeyNotFound("Key " + string(section) + ":" + string(value) + " was not found in the ini file.");
	}
	// If we can't "sscan" in float
	if (sscanf(key->second.c_str(), "%f", &retval) != 1)
	{
		throw new runtime_error("Key " + string(section) + ":" + string(value) + "=" + string(key->second.c_str()) +" can't be cast in float.");
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
float INIFile::readFloat(const char* section, const char* value, float deflt)
{
	// Try to read the value
	try
	{
		return readFloat(section, value);
	}
	catch (INI::KeyNotFound& ex)
	{
		// If exception "Not key found" then return default value
		return deflt;
	}
}

/** 
 * Use inside a loop to read all keys of a section.  The order is as read
 * from the inifile.
 *
 * @param section The name of the section from which to read.
 * @param keynum Will skip (keynum-1) entries in section.
 * @returns an iterator to the keynum'th key in section.
 */
INIKey INIFile::readKeyValue(const char* section, Uint32 keynum)
{
	map<string, INISection>::iterator sec_new;
	INIKey Key;
	string s;
	
	// get the string from (char*) section
	s = string(section);
	
	// Upper the section string
	transform(s.begin(), s.end(), s.begin(), toupper);

	sec_new = Inidata.find(s);
	if (sec_new == Inidata.end())
	{
		throw KeyNotFound("Section [" + string(section) + "] not found in .ini file.");
	}

	if (keynum >= sec_new->second.size())
	{
		logger->error("throw 0 in INIFile::readKeyValue()\n");		
		throw KeyNotFound("Key number [????] in Section [" + string(section) + "] not found in .ini file.");
	}

	Key = sec_new->second.begin();
	for (Uint32 i = 0; i < keynum; ++i)
	{
		Key++;
	}
	if (Key == sec_new->second.end()){
		logger->error("throw 0 in INIFile::readKeyValue()\n");		
		throw KeyNotFound("Key number [????] in Section [" + string(section) + "] not found in .ini file.");
	}
	return Key;
}

INIKey INIFile::readIndexedKeyValue(const char* section, Uint32 index,
		const char* prefix)
{
	map<string, INISection>::iterator sec;
	INIKey key;
	//	char TempString[255];
	std::stringstream TempStr;

	string s = section;
	transform(s.begin(), s.end(), s.begin(), toupper);

	sec = Inidata.find(s);
	if (sec == Inidata.end())
	{
		throw 0;
	}

	if (index > sec->second.size())
	{
		throw 0;
	}

	string keyval;
	if (prefix)
		keyval = prefix;
	//keyval += lexical_cast<string>(index);

	TempStr.str("");
	TempStr << (unsigned int)index;
	//	sprintf (TempString, "%u", index);
	//	keyval += TempString;
	keyval += TempStr.str();

	key = sec->second.begin();

	for (unsigned int i = 0; i < sec->second.size(); i++)
	{
		if (key->first == keyval)
		{
			return key;
		}
		key++;
	}
	if (key == sec->second.end())
	{
		throw 0;
	}
	// we should never get here
	return key;

}

string INIFile::readSection(Uint32 secnum)
{
	map<string, INISection>::iterator sec;
	Uint32 i;

	if (secnum >= Inidata.size())
	{
		throw 0;
	}
	sec = Inidata.begin();
	for (i = 0; i < secnum; i++)
	{
		sec++;
		if (sec == Inidata.end())
		{
			throw 0;
		}
	}
	return sec->first;
}

bool INIFile::isSection(string section)
{
	map<string, INISection>::iterator sec;
	
	sec = Inidata.find(section);
	if (sec == Inidata.end()){
		return false;
	}else{
		return true;
	}
	
}

int INIFile::readYesNo(const char* section, const char* value,
		const char* defaut)
{
	char* tmpPtAA = this->readString(section, value, defaut);
	string tmpAA = (string)tmpPtAA;
	Uint32 a;
	if (tmpAA == "yes")
	{
		a = 1;
	}
	else
	{
		a = 0;
	}
	delete[] tmpPtAA;

	return a;
}


/**
 \fn splitStr(std::list<std::string>& l, const std::string& seq, char s1, char s2, bool keeptok)
 \brief Splits a given std::string ( as param 'seq' ) into token separated by one starting character token and ending the token with a second given separator character.
 \param std::list<std::string>& reference to a string list that will receives all the resulting token
 \param std::string seq which is the string stream to split
 \param char s1 the first separator character that will start the token to be put into the resulting list
 \param char s2 the ending separator that will finish the token

 \param bool keeptok - optional boolean that is to be given TRUE if the separator characters are needed to be part of the tokens

 \return integer that has the number of token in the resulting list
 */

int splitStr(std::list<std::string>& l, const std::string& seq, char s1,
		char s2, bool keeptok)

{

	typedef std::string::size_type ST;
	std::vector<int> tok_s1;
	std::vector<int> tok_s2;

	if (l.size())
		l.clear();

	ST pos=0, start=0, LEN=seq.size();

	while (pos < LEN)
	{

		if (seq[pos] == s1)
		{

			start = pos;

			if (s2)
			{

				while ( (pos <LEN) && (seq[pos] != s2))
					++pos;

				if (pos <LEN)
				{

					tok_s2.push_back(pos);

					tok_s1.push_back(start);

					start = pos+1;

				}

			}

			else
				tok_s1.push_back(start);

		}

		++pos;

	}

	if (s2)
	{

		if ( (tok_s1.size() != tok_s2.size() ) || (tok_s1.size() == 0))
		{

			//screwed: return the original string

			l.push_back(seq);

			return 1;

		}

		if (tok_s1.size())
		{

			if (tok_s1[0])
				l.push_back(seq.substr(0, tok_s1[0] - (keeptok ? 0 : 1)) );

			for (pos = 0; pos < tok_s1.size(); pos++)
			{

				if (pos>0)
				{

					int c = tok_s1[pos] - tok_s2[pos-1];

					if (c > 1)
						l.push_back(seq.substr(tok_s2[pos-1]+1, c-1));

				}

				l.push_back(seq.substr(tok_s1[pos], tok_s2[pos]-tok_s1[pos]+1));

			}

		}

		if (tok_s2.back() < (LEN-1))
			l.push_back(seq.substr(tok_s2.back()+1, (LEN)-(tok_s2.back()+1)));

	}

	return l.size();

}

/**

 \fn splitStr(std::list<std::string>& l, const std::string& seq, const std::string& _1cdelim, bool keeptoken=false, bool _removews=true )

 \brief Splits a string into tokens separeted by supplied delimiters as a std::string.

 \param std::list<std::string>& L reference to the resulting string tokens

 \param std::string seq The string stream to split

 \param std::string _lcdelim - a std::string that contains all of the single delimiters

 \param bool keeptok -- same as the above function

 \param bool removews -- Set to TRUE if requiered to remove white space characters ( space, "\n\r" etc...)

 \return integer that has the number of token in the resulting list
 */
int splitStr(std::list<std::string>& L, const std::string& seq,
		const std::string& _1cdelim, bool keeptoken, bool _removews)
{

	typedef std::string::size_type ST;

	std::string delims = _1cdelim;

	std::string STR;

	if (delims.empty())
		delims = "\n\r";

	if (_removews)
		delims += " ";

	ST pos=0, LEN = seq.size();

	while (pos < LEN)
	{

		STR=""; // Init/clear the STR token buffer

		// remove any delimiters including optional (white)spaces

		while ( (delims.find(seq[pos]) != std::string::npos) && (pos < LEN))
			++pos;

		// leave if @eos

		if (pos==LEN)
			return L.size();

		// Save token data

		while ( (delims.find(seq[pos]) == std::string::npos) && (pos < LEN))
			STR += seq[pos++];

		// put valid STR buffer into the supplied list

		//std::cout << "[" << STR << "]";

		if ( !STR.empty() )
			L.push_back(STR);

	}

	return L.size();

}
