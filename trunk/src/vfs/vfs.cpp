// VFSUtils.cpp
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

#include "vfs.h"

#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <cstdarg>
#include <stdexcept>

#include "archive.h"
#include "VFile.h"
#include "filesystem/externalvfs.h"
#include "mix/mixvfs.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "include/config.h"

using std::string;
using std::vector;
using std::runtime_error;
using std::stringstream;

extern Logger * logger;

ExternalFiles * VFSUtils::externals = 0;
MIXFiles * VFSUtils::mixfiles = 0;

/**
 */
void VFSUtils::VFS_PreInit(const char* binpath)
{
    externals = new ExternalFiles(binpath);
    externals->loadArchive("data/settings/");
}

/**
 * @todo install prefix
 *
 * @param binpath Directory to parse the files
 */
void VFSUtils::VFS_Init(const string& binpath)
{
    if (binpath != ".")
    {
        externals->loadArchive("./");
    }

    // Try to load in the binary path
    externals->loadArchive(string(binpath + "/").c_str());

    INIFile* filesini = 0;
    try
    {
        filesini = GetConfig("files.ini");
    }
    catch(runtime_error&)
    {
        logger->error("Unable to locate files.ini.\n");
        return;
    }
    //for (Uint32 pathnum = 1;; ++pathnum)
    int pathnum =1;
    {
		INIKey key;
		try
		{
			key = filesini->readIndexedKeyValue("GENERAL", pathnum, "PATH");
		} catch (...)
		{
			//logger->error("Unenable to read [GENERAL]-PATH\n");
			//break;
		}
		string defpath = key->second;
		if (defpath[defpath.length() - 1] != '/' && defpath[defpath.length()
				- 1] != '\\')
		{
			defpath += "/";
		}
		externals->loadArchive(defpath.c_str());
	}

    // Create Mix file loader
    mixfiles = new MIXFiles();

    int gamenum = 1;
	//for (Uint32 gamenum = 1;; ++gamenum)
    {
        INIKey key;
		try
		{
            key = filesini->readIndexedKeyValue("GENERAL", gamenum, "GAME");
		}
		catch(...)
		{
            logger->error("Unenable to read [GENERAL]-GAME\n");
            //break;
        }
        logger->note("Trying to load \"%s\"...\n", key->second.c_str());

        // Get the number of files
        int numKeys = filesini->getNumberOfKeysInSection("RedAlert");
        int keynum;
		try
        {
            // First check we have all the required mixfiles.
            for (keynum = 1; keynum < numKeys; keynum++)
            {
                INIKey key2;
                try
                {
                    key2 = filesini->readIndexedKeyValue(key->second.c_str(), keynum,
                                                         "REQUIRED");
                }
                catch (...)
                {
                    //logger->error("Unenable to read [%s]-REQUIRED%d\n", key->second.c_str(), keynum);
                    break;
                }
                if (!mixfiles->loadArchive(key2->second.c_str()))
                {
                    logger->warning("Missing required file \"%s\"\n",
                                    key2->second.c_str());
                    throw runtime_error("Missing required file" + key2->second);
                }

            }
        }
        catch (...)
        {
            mixfiles->unloadArchives();
            //continue;
        }
        // Now load as many of the optional mixfiles as we can.
        int maxOpti = filesini->getNumberOfKeysInSection("RedAlert");
        for (keynum = 1; keynum < maxOpti; keynum++)
        {
        	stringstream deco;
        	deco << keynum;
        	string keyName = "optional" + deco.str();

        	if (filesini->isKeyInSection(string("RedAlert"), keyName) == true)
        	{
        		string mixFileName = filesini->readString("RedAlert", keyName.c_str());
        		mixfiles->loadArchive(mixFileName.c_str());
        	}
        }
        return;
    }

    // Notify that the program can't load mix archives
    logger->error("Unable to find mixes for any of the supported games!\n"
                  "Check your configuration and try again.\n");

    // Exit with an Error
    exit(1);
}

/**
 */
void VFSUtils::VFS_Destroy()
{
    // Free mixfiles (originals from Westwood)
    if (mixfiles != 0)
    {
        delete mixfiles;
    }
    mixfiles = 0;

    // Free externals files
    if (externals != 0)
    {
        delete externals;
    }
    externals = 0;
}

/**
 */
VFile * VFSUtils::VFS_Open(const char *fname)
{
	// By default open in "read only + binary" mode
    return VFS_Open(fname, "rb");
}

/**
 */
VFile * VFSUtils::VFS_Open(const char *fname, const char* mode)
{
    unsigned int fnum; // id of the loaded file

    // Try to get the file
    fnum = externals->getFile(fname, mode);
    if (fnum != (Uint32) - 1)
    {
        // return the new file created
        return new VFile(fnum, externals);
    }
    // Won't attempt to write/create files in real archives
    if (mode[0] != 'r')
    {
        // return NULL
        return 0;
    }

    if (mixfiles != 0)
    {
        fnum = mixfiles->getFile(fname);
        if (fnum != -1)
        {
            return new VFile(fnum, mixfiles);
        }
    }

    // No file found for this name
    return 0;
}

/**
 */
void VFSUtils::VFS_Close(VFile* file)
{
    // Check if file is not NULL
    if (file != NULL)
    {
        // if not delete it
        delete file;
    }
}

/**
 */
const char* VFSUtils::VFS_getFirstExisting(const std::vector<const char*>& files)
{
    VFile* tmp;
    for (Uint32 i = 0; i < files.size(); ++i)
    {
        tmp = VFSUtils::VFS_Open(files[i], "r");
        if (tmp != NULL)
        {
            VFSUtils::VFS_Close(tmp);
            return files[i];
        }
    }
    return NULL;
}

/**
 */
const char* VFSUtils::VFS_getFirstExisting(Uint32 count, ...)
{
    VFile* tmp;
    va_list ap;
    char* name;
    va_start(ap, count);
    while (count--)
    {
        name = (char*) va_arg(ap, char*);
        tmp = VFS_Open(name);
        if (tmp != NULL)
        {
            VFS_Close(tmp);
            va_end(ap);
            return name;
        }
    }
    va_end(ap);
    return NULL;
}

/**
 */
void VFSUtils::VFS_LoadGame(gametypes gt)
{
    switch (gt)
    {
    case GAME_TD:
        externals->loadArchive("data/settings/td/");
        break;
    case GAME_RA:
        externals->loadArchive("data/settings/ra/");
        break;
    default:
        logger->error("Unknown gametype %i specified\n", gt);
        break;
    }
}
