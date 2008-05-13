// SoundEngine.h
// 1.3

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include <map>
#include <string>

#ifdef RA_SOUND_ENGINE
#include "SDL/SDL_mixer.h"
#endif

#include "SoundCommon.h"
#include "SoundCache.h"

class SoundBuffer;

using std::map;
using std::string;


class SoundEngine
{
public:
    SoundEngine(bool disableSound = false);
    ~SoundEngine();

    void LoadSound(const std::string& sound);

    // Game sounds
    void SetSoundVolume(unsigned int volume);
    void PlaySound(const std::string& sound);
    int  PlayLoopedSound(const std::string& sound, unsigned int loops);
    void StopLoopedSound(int id);
    void PauseLoopedSound(int id);
    void ResumeLoopedSound(int id);

    // Music
    void SetMusicVolume(unsigned int volume);
    /** Create the playlist with RA musics */
    bool CreatePlaylist();
    void PlayMusic(); // Starts|Resumes playing
    void PauseMusic();
    void StopMusic();

    void PlayTrack(const std::string& sound); // Plays a specific track
    void NextTrack(); // Selects the next track in the playlist
    void PrevTrack(); // Selects the previous track in the playlist

    static void MusicHook(void* userdata, Uint8* stream, int len);

    typedef void (*MixFunc)(void*, Uint8*, int);

    void SetMusicHook(MixFunc mixfunc, void *arg);

    bool NoSound() const;

private:
    typedef std::vector<std::string> Playlist;

    SoundBuffer* LoadSoundImpl(const std::string& sound);

    bool nosound;
    bool mutesound;
    bool musicFinished;
    unsigned int soundVolume;

    SoundCache soundCache;

    Playlist playlist;
    Playlist::iterator currentTrack;
};

#endif //SOUNDENGINE_H
