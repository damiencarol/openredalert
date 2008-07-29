// SoundCache.h
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

#ifndef SOUNDCACHE_H
#define SOUNDCACHE_H

#include <string>
#include <map>

class SoundBuffer;

using std::string;
using std::map;

/**
 * Cache for all buffer (sounds)
 */
class SoundCache : public map<string, SoundBuffer*>
{
};

#endif //SOUNDCACHE_H
