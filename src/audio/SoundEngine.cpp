// SoundEngine.cpp
// 1.3

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

#include "SoundEngine.h"

#include <string>

#include "SoundCommon.h"
#include "SOUND_DECODE_STATE.h"
#include "include/Logger.h"
#include "include/config.h"

namespace pc {
    extern ConfigType Config;
}
extern Logger * logger;

using std::string;

/**
 */
SoundEngine::SoundEngine(bool disableSound) : 
	nosound(disableSound), 
	mutesound(false), 
	musicFinished(true), 
	currentTrack(playlist.begin())
{
    if (nosound){
        return;
    }
    
    
	// Warning the Mix_OpenAudio uses libmikmod witch seems to create the music.raw file
    if (Mix_OpenAudio(SOUND_FREQUENCY, SOUND_FORMAT, SOUND_CHANNELS, 1024 /*4096*/) < 0) {
        logger->error("%s line %i: Unable to open sound: %s\n", __FILE__, __LINE__, Mix_GetError());
        nosound = true;
    }

    // Set volumes to half of max by default; this should be a fallback, real setting should be read from config
    SetSoundVolume(MIX_MAX_VOLUME / 2);
    SetMusicVolume(MIX_MAX_VOLUME / 2);
}

/**
 */
SoundEngine::~SoundEngine()
{
    if (nosound){
        return;
    }

    // Stop all playback
    Mix_HaltChannel(-1);
    Mix_HookMusic(NULL, NULL);
    
    // Clean up the sound cache
    for_each(soundCache.begin(), soundCache.end(), SoundCacheCleaner());

    Mix_CloseAudio();
}

/**
 */
bool SoundEngine::CreatePlaylist()
{
	// If no sound needed create nothing
	if (nosound){
		return true;
	}
	
	// Clear the list
	playlist.clear();
	
	// Create the amazing playlist
	playlist.push_back("bigf226m.aud");
	playlist.push_back("crus226m.aud");
	playlist.push_back("fac1226m.aud");
	playlist.push_back("hell226m.aud");
	playlist.push_back("fac2226m.aud");
	playlist.push_back("run1226m.aud");
	playlist.push_back("tren226m.aud");
	playlist.push_back("work226m.aud");
	playlist.push_back("await.aud");
	playlist.push_back("dense_r.aud");
	playlist.push_back("fogger1a.aud");
	playlist.push_back("mud1a.aud");
	playlist.push_back("radio2.aud");
	playlist.push_back("rollout.aud");
	playlist.push_back("snake.aud");
	playlist.push_back("terminat.aud");
	playlist.push_back("twin.aud");
	playlist.push_back("vector1a.aud");
	playlist.push_back("smsh226m.aud");
		
	// Set the current track to the first track
	currentTrack = playlist.begin();
			
	// Return true to indicate success in playlist loading
	return true;
}

/**
 * @param volume Volume wanted between 0 and 128
 */
void SoundEngine::SetSoundVolume(int volume)
{
    if (nosound){
        return;
    }
    
    // standard control
    // if the sound volume asked is > to the sound volume MAX 
    // from the SDL_mixer constantes then the sound is set to max :)
    if (volume > MIX_MAX_VOLUME) {
    	volume = MIX_MAX_VOLUME;
    } else if (volume < 0) {
    	volume = 0;
    }
    
    // Set the volume of all channel (-1)
    Mix_Volume(-1, volume);
    
    soundVolume = volume;
    mutesound = volume == 0;
}

/**
 */
void SoundEngine::PlaySound(const string& sound)
{
	if (sound.empty()){
		return;
	}

	// Play this sound looped just 1 time
    PlayLoopedSound(sound, 1);
}

/**
 */
int SoundEngine::PlayLoopedSound(const string& sound, unsigned int loops)
{
    if (nosound || mutesound || sound.empty())
    {
        return -1;
    }

    SoundBuffer* snd = LoadSoundImpl(sound);
    if (snd == 0)
    {
        return -1;
    }

    if (Mix_Paused (-1))
    {
        Mix_Resume(-1);
    }

    int channel = 0;
    channel = Mix_PlayChannel(-1, snd->chunk, static_cast<int>(loops)-1);
    Mix_Volume(channel, soundVolume);

    return channel;
}

/**
 */
void SoundEngine::StopLoopedSound(int id)
{
    if (nosound){
        return;
    }
    
    Mix_HaltChannel(id);
}

/**
 */
void SoundEngine::PauseLoopedSound(int id)
{
    if (nosound)
    {
        return;
    }
    
    Mix_Pause(id);
}

/**
 */
void SoundEngine::ResumeLoopedSound(int id)
{
    if (nosound)
    {
        return;
    }
    
    Mix_Resume(id);
}

void SoundEngine::SetMusicVolume(int volume)
{
    if (nosound)
    {
        return;
    }

    // standard control
    // if the sound volume asked is > to the sound volume MAX 
    // from the SDL_mixer constantes then the sound is set to max :)
    if (volume > MIX_MAX_VOLUME) {
    	volume = MIX_MAX_VOLUME;
    }  

    Mix_VolumeMusic(volume);
}

void SoundEngine::PlayMusic()
{
    if (nosound){
        return;
    }
    if (!Mix_PlayingMusic()) {
        if (Mix_PausedMusic()) {
            Mix_ResumeMusic();
        } else {
            PlayTrack(*currentTrack);
        }
    }
}

void SoundEngine::PauseMusic()
{
    if (nosound){
        return;
    }
    Mix_PauseMusic();
}

void SoundEngine::StopMusic()
{
    if (nosound)
        return;

    Mix_HookMusic(NULL, NULL);
    musicDecoder.Close();
}


void SoundEngine::PlayTrack(const string& sound)
{
    if (nosound)
        return;

    StopMusic();

    if (sound == "No theme") {
        PlayMusic();
        return;
    }

    if (musicDecoder.Open(sound)) {
        musicFinished = false;
        Mix_HookMusic(MusicHook, reinterpret_cast<void*>(&musicFinished));
    }
}

void SoundEngine::NextTrack()
{
    if (nosound)
        return;

    if (++currentTrack == playlist.end())
        currentTrack = playlist.begin();

    PlayTrack(*currentTrack);
}

/**
 */
void SoundEngine::PrevTrack()
{
    if (nosound)
        return;

    if (currentTrack == playlist.begin())
        currentTrack = playlist.end();

    PlayTrack(*(--currentTrack));
}

/**
 *
 */
void SoundEngine::MusicHook(void* userdata, Uint8* stream, int len)
{
    bool* musicFinishedPtr = reinterpret_cast<bool*>(userdata);
    if (!*musicFinishedPtr)
    {
        SampleBuffer buffer;
        Uint32 ret = musicDecoder.Decode(buffer, len);
        if (ret == SOUND_DECODE_COMPLETED) {
            musicDecoder.Close();
            *musicFinishedPtr = true;
        } else if (ret == SOUND_DECODE_ERROR) {
            logger->error("Sound: Error during music decoding, stopping playback of current track.\n");
            *musicFinishedPtr = true;
            return;
        }
        memcpy(stream, &buffer[0], buffer.size());
    }
}

/**
 */
void SoundEngine::SetMusicHook(MixFunc mixfunc, void* arg)
{
    Mix_HookMusic(mixfunc, arg);
}

void SoundEngine::LoadSound(const string& sound)
{
    LoadSoundImpl(sound);
}

SoundBuffer* SoundEngine::LoadSoundImpl(const string& sound)
{
    SoundBuffer* buffer = 0;

    // Check if sound is already loaded and cached
    SoundCache::iterator cachedSound = soundCache.find(sound);
    if (cachedSound != soundCache.end()) {
       buffer = cachedSound->second;
    } else {
        // Load and cache sound
        if (soundDecoder.Open(sound)) {
            buffer = new SoundBuffer();
            // length = 0 because we don't know the size
            if (soundDecoder.Decode(buffer->data, 0) == SOUND_DECODE_COMPLETED)
            {
                buffer->chunk = Mix_QuickLoad_RAW(&buffer->data[0], static_cast<Uint32>(buffer->data.size()));
                soundCache.insert(SoundCache::value_type(sound, buffer));
            } else {
                delete buffer;
                buffer = 0;
            }
            soundDecoder.Close();
        }
    }
    return buffer;
}

bool SoundEngine::NoSound() const
{
	return nosound;
}
