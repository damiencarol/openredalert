#ifndef VFSUTILS_H
#define VFSUTILS_H

#include <vector>

#include "SDL/SDL_types.h"

#include "vfs/archive.h"
#include "misc/gametypes.h"

using std::vector;

class ExternalFiles;
class MIXFiles;

class VFSUtils
{
public:
	/** Sets up externals so that the logger can work */
	static void VFS_PreInit(const char* binpath);
	static void VFS_Init(const char *binpath);
	static void VFS_Destroy();
	/** Try to load all archive of a specific game */
	static void VFS_LoadGame(gametypes gt);
	/** Most code just uses the VFS for reading, so this default to reading */
	static VFile *VFS_Open(const char *fname);
	static VFile *VFS_Open(const char *fname, const char* mode);
	/** Close a VFile */
	static void VFS_Close(VFile *file);
	static const char* VFS_getFirstExisting(const std::vector<const char*>& files);
	static const char* VFS_getFirstExisting(Uint32 count, ...);

	/**
	 * External files added to the originals files
	 */
	static ExternalFiles* externals;
	/**
	 * Originals mixFiles from westwood files
	 */
	static MIXFiles* mixfiles;
};

#endif //VFSUTILS_H
