// SoundCacheCleaner.hpp

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

#ifndef SOUNDCACHECLEANER_HPP_
#define SOUNDCACHECLEANER_HPP_

#include <algorithm>

#include "SDL/SDL_mixer.h"

#include "SoundCache.h"
#include "SoundBuffer.h"

using std::unary_function;

namespace OpenRedAlert
{

namespace Sound
{

struct SoundCacheCleaner : public unary_function<SoundCache::value_type, void>
{
    void operator()(const SoundCache::value_type& p)
    {
        Mix_FreeChunk(p.second->chunk);
        delete p.second;
    }
};

}

}

#endif /* SOUNDCACHECLEANER_HPP_ */
