#ifndef SOUNDCACHE_H
#define SOUNDCACHE_H

#include <string>
#include <map>

class SoundBuffer;

using std::string;
using std::map;


//typedef std::map<std::string, SoundBuffer*> SoundCache;

class SoundCache : public std::map<std::string, SoundBuffer*>
{
};

#endif //SOUNDCACHE_H
