// VFSUtils.h
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

#ifndef VFSUTILS_H
#define VFSUTILS_H

#include <vector>
#include <string>

#include "SDL/SDL_types.h"

#include "vfs/archive.h"
#include "misc/gametypes.h"

using std::vector; 
using std::string;

class ExternalFiles;
class MIXFiles;

class VFSUtils
{
public:
	/** Sets up externals so that the logger can work */
	static void VFS_PreInit(const char* binpath);
	static void VFS_Init(const string& binpath);
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
