// OpenRedAlert.cpp
//
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

#include <iostream>
#include <exception>
#include <map>
#include <stdexcept>
#include <string>

#include <ctime>
#include <locale.h>

#include "SDL/SDL.h"

#include "audio/SoundEngine.h"
#include "game/Game.h"
#include "game/GameError.h"
#include "Logger.hpp"
#include "vfs/vfs.h"
#include "video/GraphicsEngine.h"
#include "video/VideoError.h"
#include "video/VQAMovie.h"
#include "video/WSAMovie.h"
#include "video/WSAError.h"

#include "game/MultiPlayerMaps.h"

#ifndef VERSION
#define VERSION "6xx"
#endif

using std::abort;
using std::map;
using std::set_terminate;
using std::string;
using std::runtime_error;
using std::cout;
using std::endl;

using Sound::SoundEngine;

/** Logger for the application */
Logger *logger;

void PrintUsage(); // In args.cpp
bool parse(int argc, char** argv); // In args.cpp

// Below main
void cleanup();
void fcnc_terminate_handler();

namespace pc {
    extern ConfigType Config;
    extern vector<SHPImage *> *imagepool;
    extern GraphicsEngine * gfxeng;
    /** SoundEngine of the game */
    extern Sound::SoundEngine* sfxeng;
}

using VQA::VQAMovie;

int main(int argc, char** argv)
{
    // Log to the console the GPL license
    cout << "OpenRedAlert  Copyright (C) 2008-2010  Damien Carol" << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
    cout << "This is free software, and you are welcome to redistribute it" << endl;
    cout << "under certain conditions; see 'COPYING' for details." << endl;
    cout.flush();

    // Register end functions
    atexit(cleanup);
    set_terminate(fcnc_terminate_handler);

    // Correct the way that floats are readed
    setlocale(LC_ALL, "C");

    // Check if help wanted
    if ((argc > 1) && ( string(argv[1]) == "-h" ||
            string(argv[1]) == "--help" || string(argv[1]) == "-?"))
    {
        PrintUsage();
        return EXIT_SUCCESS;
    }
    
    // Check if version wanted
    if ((argc > 1) && ( string(argv[1]) == "-v" ||
            string(argv[1]) == "--version" ))
    {
        // Print version
        printf("openredalert %s", VERSION);
        return EXIT_SUCCESS;
    }

    
    const string& binpath = determineBinaryLocation(argv[0]);
    string lf(binpath);
    lf += "/debug.log";

    // Initialize the Virtual File System
    VFSUtils::VFS_PreInit(binpath.c_str());
    
    // Loads arguments
    if (!parse(argc, argv)) {
        return 1;
    }
    pc::Config = getConfig();

    pc::Config.binpath = binpath;

    // Init with the path of the binaries
    VFSUtils::VFS_Init(binpath.c_str());
    VFSUtils::VFS_LoadGame(pc::Config.gamenum);
    // Log success of loading RA gmae
    Logger::getInstance()->Info(".MIX archives loading ok\n");

    // Test loading multi-player map
    //logger->note("Test loading multi-player map\n");
    //MultiPlayerMaps* toto = new MultiPlayerMaps();
    //
    //return 0;


    // Load the start
    Logger::getInstance()->Info("Please wait, OpenRedAlert " + string(VERSION) + " is starting\n");

    try {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0) {
            Logger::getInstance()->Error("Couldn't initialize SDL: " + string(SDL_GetError()));
            exit(1);
        }

        if (pc::Config.debug) {
            // Don't hide if we're debugging as the lag when running inside
            // valgrind is really irritating.
            Logger::getInstance()->Debug("Debug mode is enabled.");
        }
        else {
            // Hide the cursor since we have our own.
            SDL_ShowCursor(0);
        }

        // Initialize Video
        try {
            Logger::getInstance()->Info("Initializing the graphics engine...");
            pc::gfxeng = new GraphicsEngine();
            Logger::getInstance()->Info("Done.");
        }
        catch (VideoError& ex) {
            Logger::getInstance()->Error("Failed.");
            Logger::getInstance()->Error(ex.what());
            throw runtime_error("Unable to Initialize the graphics engine");
        }

        // Initialize Sound
        Logger::getInstance()->Info("Initializing the sound engine...");
        pc::sfxeng = new SoundEngine(pc::Config.nosound);
        Logger::getInstance()->Info("done\n");

        // "Standalone" VQA Player
        if (pc::Config.playvqa) {
            Logger::getInstance()->Info(string("Now playing ") + pc::Config.vqamovie);
            try {
                VQA::VQAMovie mov(pc::Config.vqamovie.c_str());
                mov.play();
            }
            catch (runtime_error&) {
                Logger::getInstance()->Info("Failed to play movie: " + pc::Config.vqamovie);
            }
        }

        // Play the intro if requested
        if (pc::Config.intro) {
            Logger::getInstance()->Info("Now playing the Introduction movie");
            try {
                VQAMovie mov("logo");
                mov.play();
            }
            catch (runtime_error&) {
            }

            try {
                WSAMovie* choose = new WSAMovie("choose.wsa");
                choose->animate(*(pc::gfxeng));
            }
            catch (WSAError&) {
            }
        }

        // "Standalone" VQA Player
        if (pc::Config.gamenum == GAME_RA) {
            //Logger::getInstance()->Info("Now playing %s\n", pc::Config.vqamovie.c_str());
            try {
                VQAMovie mov("english");
                mov.play();
            }
            catch (runtime_error&) {
                // Oke, failed to read the redalert intro, try the demo intro
                Logger::getInstance()->Info("%s line %i: Failed to play movie: english.vqa --> trying redintro.vqa\n");
                try {
                    VQAMovie mov("redintro.vqa");
                    mov.play();
                }
                catch (runtime_error&) {
                    Logger::getInstance()->Info("%s line %i: Failed to play movie: redintro.vqa\n");
                }
            }
        }

        // Init the rand functions
        srand(static_cast<unsigned int>(time(0)));

        // Stop all the music
        pc::sfxeng->StopMusic();
        pc::sfxeng->StopLoopedSound(-1);

        // Clean the graphics engine
        if (pc::gfxeng != 0) {
            delete pc::gfxeng;
        }
        pc::gfxeng = 0;

        try {
            // Initialize game engine
            Logger::getInstance()->Info("Initializing game engine.");
            Game gsession;
            // Start the game engine
            Logger::getInstance()->Info("Starting game.");
            // Play the session
            gsession.play();
            // Log end off session
            Logger::getInstance()->Info("Shutting down.");
        }
        catch (GameError&) {
            // Log it
            Logger::getInstance()->Error("Error during game.");
        }
       
    }
    catch (runtime_error& e) {
        Logger::getInstance()->Error(e.what());
        //#if _WIN32
        //MessageBox(0, e.what(), "Fatal error", MB_ICONERROR|MB_OK);
        //#endif
    }
    return 0;
}

/**
 * Wraps around a more verbose terminate handler and cleans up better
 */
void fcnc_terminate_handler() 
{
    cleanup();

#if __GNUC__ == 3 && __GNUC_MINOR__ >= 1 && ! defined (__MORPHOS__)
    // GCC 3.1+ feature, and is turned on by default for 3.4.
    using __gnu_cxx::__verbose_terminate_handler;
    __verbose_terminate_handler();
#else
    abort();
#endif
}

/**
 */
void cleanup()
{
    // Free logger singleton
    Logger::freeSingleton();


    // Free VFS
    VFSUtils::VFS_Destroy();

    // Free SDL
    SDL_Quit();
}
