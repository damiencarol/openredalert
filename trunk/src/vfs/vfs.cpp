#include "vfs.h"

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

extern Logger * logger;

ExternalFiles * VFSUtils::externals= NULL;
MIXFiles * VFSUtils::mixfiles= NULL;

/** 
 * Sets up externals so that the logger can work
 */
void VFSUtils::VFS_PreInit(const char* binpath)
{
	externals = new ExternalFiles(binpath);
	externals->loadArchive("data/settings/");
}

/** 
 * TODO install prefix
 */
void VFSUtils::VFS_Init(const char* binpath)
{
	INIFile *filesini= NULL;
	string tempstr;
	Uint32 keynum;

	//logger->debug("Assuming binary is installed in \"%s\"\n", binpath);

	if (strcasecmp(".", binpath)!=0)
	{
		externals->loadArchive("./");
	}
	tempstr = binpath;
	tempstr += "/";
	externals->loadArchive(tempstr.c_str());

	// TODO prevoir pour linux
	/*
	 #if defined _WIN32
	 #else
	 externals->loadArchive("/etc/freecnc/");
	 #endif
	 */

	try
	{
		filesini = GetConfig("files.ini");
	}
	catch(runtime_error&)
	{
		logger->error("Unable to locate files.ini.\n");
		return;
	}
	for (Uint32 pathnum = 1;; ++pathnum)
	{
		INIKey key;
		try
		{
			key = filesini->readIndexedKeyValue("GENERAL",pathnum,"PATH");
		}
		catch(...)
		{
			logger->error("Unenable to read [GENERAL]-PATH\n");
			break;
		}
		string defpath = key->second;
		if (defpath[defpath.length()-1] != '/' && defpath[defpath.length()-1] != '\\')
		{
			defpath += "/";
		}
		externals->loadArchive(defpath.c_str());
	}

	mixfiles = new MIXFiles();

	for (Uint32 gamenum = 1;; ++gamenum)
	{
		INIKey key;
		try
		{
			key = filesini->readIndexedKeyValue("GENERAL",gamenum,"GAME");
		}
		catch(...)
		{
			logger->error("Unenable to read [GENERAL]-GAME\n");
			break;
		}
		logger->note("Trying to load \"%s\"...\n", key->second.c_str());
		try
		{
			// First check we have all the required mixfiles.
			for (keynum = 1;;keynum++)
			{
				INIKey key2;
				try
				{
					key2 = filesini->readIndexedKeyValue(key->second.c_str(),keynum,
							"REQUIRED");
				}
				catch(...)
				{
					logger->error("Unenable to read [???]-REQUIRED\n");
					break;
				}
				if( !mixfiles->loadArchive(key2->second.c_str()) )
				{
					logger->warning("Missing required file \"%s\"\n",
							key2->second.c_str());
					throw runtime_error("Missing required file" + key2->second);
				}

			}
		}
		catch(...)
		{
			mixfiles->unloadArchives();
			continue;
		}
		// Now load as many of the optional mixfiles as we can.
		for (keynum = 1;; keynum++)
		{
			INIKey key2;
			try
			{
				key2 = filesini->readIndexedKeyValue(key->second.c_str(),keynum,
						"OPTIONAL");
			}
			catch(...)
			{
				logger->error("Unenable to read [???]-OPTIONAL\n");
				break;
			}
			mixfiles->loadArchive(key2->second.c_str());
		}
		return;
	}

	// Notify that the program can't load mix archives
	logger->error("Unable to find mixes for any of the supported games!\n"
		"Check your configuration and try again.\n");

	exit(1);
}

void VFSUtils::VFS_Destroy()
{
	// Free mixfiles (originals from Westwood)
	if (mixfiles != NULL)
	{
		delete mixfiles;
	}
	mixfiles = NULL;

	// Free externals files
	if (externals != NULL)
	{
		delete externals;
	}
	externals = NULL;
}

VFile * VFSUtils::VFS_Open(const char *fname)
{
	return VFS_Open(fname, "rb");
}

VFile * VFSUtils::VFS_Open(const char *fname, const char* mode)
{
	Uint32 fnum; // id of the loaded file
	
	// Try to get the file
	fnum = externals->getFile(fname, mode);
	if (fnum != (Uint32)-1)
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
		if (fnum != (Uint32)-1)
		{
			return new VFile(fnum, mixfiles);
		}
	}
	
	// No file found for this name
	return 0;
}

/**
 * Close a VFile
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

const char* VFSUtils::VFS_getFirstExisting(const std::vector<const char*>& files)
{
	VFile* tmp;
	for (Uint32 i=0; i<files.size(); ++i)
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

const char* VFSUtils::VFS_getFirstExisting(Uint32 count, ...)
{
	VFile* tmp;
	va_list ap;
	char* name;
	va_start(ap,count);
	while (count--)
	{
		name = (char*)va_arg(ap,char*);
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
 * Try to load all archive of a specific game
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
