// Game.cpp
// 1.5

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

#include "Game.h"

#include <stdexcept>

#include "SDL/SDL.h"

#include "Logger.hpp"
#include "ActionEventQueue.h"
#include "CnCMap.h"
#include "Dispatcher.h"
#include "video/GraphicsEngine.h"
#include "PlayerPool.h"
#include "misc/config.h"
#include "audio/SoundEngine.h"
#include "audio/SoundError.h"
#include "game/Ai.h"
#include "game/Player.h"
#include "vfs/vfs.h"
#include "video/ImageCache.h"
#include "ui/Menu.h"
#include "ui/Cursor.h"
#include "ui/PauseMenu.h"
#include "ui/Input.h"
#include "ui/Sidebar.h"
#include "ui/SidebarError.h"
#include "GameMode.h"
#include "MissionData.h"
#include "vfs/mix/mixvfs.h"
#include "video/VQAMovie.h"
#include "MissionMapsClass.h"
#include "video/SHPImage.h"
#include "RedAlertDataLoader.h"
#include "GameError.h"
#include "LoadMapError.h"
#include "video/VideoError.h"
#include "game/UnitAndStructurePool.h"
#include "ui/MapAnimationMenu.hpp"
#include "TriggerManager.hpp"
#include "game/Unit.hpp"

using std::string;
using std::runtime_error;
using VQA::VQAMovie;

using Sound::SoundEngine;
using OpenRedAlert::Sound::SoundError;
using OpenRedAlert::Game::TriggerManager;
using UI::MapAnimationMenu;

namespace p
{
    extern Dispatcher* dispatcher;
    extern CnCMap* ccmap;
    extern ActionEventQueue* aequeue;
    extern RedAlertDataLoader * raLoader;
}
namespace pc
{
    extern GraphicsEngine* gfxeng;
    extern Ai* ai;
    extern ConfigType Config;
    extern vector<SHPImage *> *imagepool;
    extern bool quit;
    extern Cursor* cursor;
    extern ImageCache* imgcache;
    extern Input* input;
    extern Sidebar* sidebar;
    extern SoundEngine* sfxeng;
}

/**
 * Constructor, loads the map, sidebar and such.
 * plays briefing and actionmovie
 */
Game::Game()
{
    MissionNr = 0;
    OldUptime = 0;
    pc::ai = 0;
    pc::input = 0;
    pc::cursor = 0;
    pc::sidebar = 0;
    p::dispatcher = 0;
    p::aequeue = 0;
    p::ccmap = 0;
    pc::imagepool = 0;
    pc::gfxeng = 0;
    pc::sfxeng = 0;

    // At start the sound "BattleControlTerminated" is not played
    BattleControlTerminated = false;
}

/**
 * Destructor, frees up some memory
 */
Game::~Game()
{
    FreeMemory();
}

/**
 * Initialize game (after the menu)
 *
 * @note in this methode we catch error during playing of movies to avoid arror when mix files is CD1 (allies) and their are not ALL soviets movies
 */
void Game::InitializeMap(string MapName)
{
    /// Copy the game mode
    this->gamemode = pc::Config.gamemode;

    /// Create a new action event queue
    p::aequeue = new ActionEventQueue();

    /// Create and initialize the map
    try
    {
        p::ccmap = new CnCMap();
        p::ccmap->Init(GAME_RA, this->gamemode);
        p::ccmap->loadMap(MapName, 0);
    }
    catch (LoadMapError& ex)
    {
        Logger::getInstance()->Error(__FILE__, __LINE__, ex.what());
        // loadmap will have printed the error
        throw GameError("Error during load of the Map in Game::InitializeMap\n");
    }

    /// Create a new dispatcher
    p::dispatcher = new Dispatcher();
    switch (pc::Config.dispatch_mode)
    {
    case 0:
        break;
    case 1:
        // Record
        break;
    case 2:
        // Playback
        break;
    default:
        Logger::getInstance()->Error("Invalid dispatch mode");
        throw GameError("Invalid dispatch mode:");
        break;
    }

    switch (gamemode)
    {
    case GAME_MODE_SINGLE_PLAYER:
        try
        {
            // Try to Play the "Brief" Movie
            string briefMovieName = p::ccmap->getMissionData().brief;
            // Check that "Brief" movie is different of "<none>"
            if (briefMovieName != "<none>")
            {
                // Try to play the movie
                try {
                    VQAMovie* movBrief = new VQAMovie(briefMovieName.c_str());
                    movBrief->play();
                } catch(runtime_error& er)
                {}
            }

            // Try to Play the "Action" Movie
            string actionMovieName = p::ccmap->getMissionData().action;
            // Check that "Action" movie is different of "<none>
            if (actionMovieName != "<none>")
            {
                // Try to play the movie
                try {
                    VQAMovie* actionMovie = new VQAMovie(actionMovieName.c_str());
                    actionMovie->play();
                } catch(runtime_error& er)
                {}
            }
        }
        catch (std::runtime_error&)
        {}
        break;
    case GAME_MODE_SKIRMISH:
    case GAME_MODE_MULTI_PLAYER:
    default:
        break;
    }

    /// init the sidebar
    try
    {
        pc::sidebar = new Sidebar(p::ccmap->getPlayerPool()->getLPlayer(),
                pc::gfxeng->getHeight(),
                p::ccmap->getMissionData().theater.c_str());
    }
    catch (SidebarError& error)
    {
        // Log it
        Logger::getInstance()->Error(error.what());
        // Throw game error
        throw GameError("Can't construct the Sidebar\n");
    }

    /// init the cursor
    MACRO_LOG_DEBUG("Init cursor.");
    pc::cursor = new Cursor();

    /// init the input functions
    MACRO_LOG_DEBUG("Init input.");
    pc::input = new Input(pc::gfxeng->getWidth(),
                          pc::gfxeng->getHeight(),
                          pc::gfxeng->getMapArea());

    MACRO_LOG_DEBUG("End of Game::InitializeMap");
}

/**
 * GraphicsEngine, SoundEngine, VFSUtils file system, ImageCache and imagepool, RADataLoader and ConfigType object from ini file (externals)
 */
void Game::InitializeGameClasses()
{
    /// Initialize the Graphics Engine
    try
    {
        Logger::getInstance()->Info("Initializing the graphics engine...");
        pc::gfxeng = new GraphicsEngine();
        Logger::getInstance()->Info("done\n");
    }
    catch (VideoError& ex)
    {
        Logger::getInstance()->Error(ex.what());
        throw runtime_error("Unable to Initialize the graphics engine");
    }

    /// Initialize Sound
    try
    {
        Logger::getInstance()->Info("Initializing the sound engine...");
        pc::sfxeng = new SoundEngine(pc::Config.nosound);
        Logger::getInstance()->Info("done\n");
    }
    catch (SoundError& error)
    {
        Logger::getInstance()->Error(error.what());
        Logger::getInstance()->Info("failed.  exiting\n");
        throw runtime_error("Unable to Initialize the sound engine");
    }

    /// Initialize the VFS file system
    VFSUtils::VFS_PreInit(pc::Config.binpath.c_str());
    VFSUtils::VFS_Init(pc::Config.binpath.c_str());
    VFSUtils::VFS_LoadGame(pc::Config.gamenum);

    // Get the mission maps
    //pc::MissionsMapdata = new MissionMapsClass();

    // Init the image cache
    pc::imagepool = new std::vector<SHPImage*>();
    pc::imgcache->setImagePool(pc::imagepool);
    //pc::imgcache->setImagePool(new std::vector<SHPImage*>());


    /// Init the Data Loader
    try
    {
        Logger::getInstance()->Info("Initializing the RA Data loader...");
        p::raLoader = new RedAlertDataLoader();
        p::raLoader->load();
        Logger::getInstance()->Info("done\n");
    }
    catch (...)
    {
        Logger::getInstance()->Error("failed.  exiting\n");
        throw runtime_error("Unable to Initialize the RA Data loader");
    }

    // Load the music files (for background music
    // Create playlist with all music of RedAlert
    if (pc::sfxeng->CreatePlaylist() != true)
    {
        //
        Logger::getInstance()->Error("Could not create playlist!\n");
        //
        throw GameError("Could not create playlist!\n");
    }

    // Load the sounds ini file
    INIFile *soundini = GetConfig("sound.ini");

    /// Structures
    pc::Config.StructureDestroyed = soundini->readString("STRUCTURES",
            "destroyed", "kaboom22.aud");
    pc::Config.StructureHalfDestroyed = soundini->readString("STRUCTURES",
            "halfdestroyed", "kaboom1.aud");
    pc::Config.StructureSold = soundini->readString("STRUCTURES", "sold",
            "strusld1.aud");
    pc::Config.StructurePlaced = soundini->readString("STRUCTURES", "placed",
            "build5.aud");
    pc::Config.RepairStructure = soundini->readString("STRUCTURES", "repair",
            "repair1.aud");
    pc::Config.PrimairyStructureSelected = soundini->readString("STRUCTURES",
            "primairy", "pribldg1.aud");
    pc::Config.StructureStartBuild = soundini->readString("STRUCTURES",
            "startbuilding", "abldgin1.aud");
    pc::Config.StructureReady = soundini->readString("STRUCTURES",
            "constrcomplete", "conscmp1.aud");

    /// Units
    pc::Config.UnitFull = soundini->readString("UNITS", "full", "unitful1.aud");
    pc::Config.UnitLost = soundini->readString("UNITS", "lost", "unitlst1.aud");
    pc::Config.RepairUnit = soundini->readString("UNITS", "repair",
            "repair1.aud");
    pc::Config.UnitRepaired = soundini->readString("UNITS", "repaired",
            "unitrep1.aud");
    pc::Config.UnitSold = soundini->readString("UNITS", "sold", "unitsld1.aud");
    pc::Config.UnitReady = soundini->readString("UNITS", "ready",
            "unitrdy1.aud");
    pc::Config.UnitDeployed = soundini->readString("UNITS", "deployed",
            "build5.aud");
    pc::Config.AirUnitLost = soundini->readString("UNITS", "airunitlost",
            "aunitl1.aud");
    pc::Config.TrainUnit = soundini->readString("UNITS", "starttraining",
            "train1.aud");

    /// Base
    pc::Config.BaseUnderAttack = soundini->readString("BASE", "underattack",
            "baseatk1.aud");
    pc::Config.CmdCentreUnderAttack = soundini->readString("BASE",
            "centerunderattack", "cmdcntr1.aud");

    /// Naval
    pc::Config.NavalUnitLost = soundini->readString("NAVAL", "unitlost",
            "navylst1.aud");

    /// General
    pc::Config.BattleControlTerm = soundini->readString("GENERAL", "terminate",
            "bct1.aud");
    pc::Config.BuildingCanceled = soundini->readString("GENERAL",
            "buildingcanceled", "cancld1.aud");
    pc::Config.BuildingOnHold = soundini->readString("GENERAL",
            "buildingonhold", "onhold1.aud");
    pc::Config.Newoptions = soundini->readString("GENERAL", "newoptions",
            "newopt1.aud");
    pc::Config.NoDeploy = soundini->readString("GENERAL", "nodeploy",
            "nodeply1.aud"); // Has to be use jet
    pc::Config.NoFunds = soundini->readString("GENERAL", "nomoney",
            "nofunds1.aud"); // Has to be use jet
    pc::Config.Nopower = soundini->readString("GENERAL", "nopower",
            "nopowr1.aud"); // Has to be use jet
    pc::Config.Reinforcements = soundini->readString("GENERAL",
            "reinforcements", "reinfor1.aud"); // Has to be use jet
    pc::Config.RadarUp = soundini->readString("GENERAL", "radarup",
            "radaron2.aud");
    pc::Config.RadarDown = soundini->readString("GENERAL", "radardown",
            "radardn1.aud");
    pc::Config.MissionWon = soundini->readString("GENERAL", "missionwon",
            "misnwon1.aud");
    pc::Config.MissionLost = soundini->readString("GENERAL", "missionlost",
            "misnlst1.aud");

    /// Money
    pc::Config.MoneyCountUp = soundini->readString("MONEY", "countup",
            "cashup1.aud"); // Has to be use jet
    pc::Config.MoneyCountDown = soundini->readString("MONEY", "countdown",
            "cashdn1.aud"); // Has to be use jet
}

/**
 * Free memory
 */
void Game::FreeMemory()
{
    if (pc::imgcache != 0)
    {
        pc::imgcache->Cleanup();
    }

    // Free input
    if (pc::input != 0)
    {
        delete pc::input;
    }
    if (pc::cursor != 0)
    {
        delete pc::cursor;
    }
    if (pc::sidebar != 0)
    {
        delete pc::sidebar;
    }
    if (p::dispatcher != NULL)
        delete p::dispatcher;
    if (p::aequeue != NULL)
        delete p::aequeue;
    if (p::ccmap != 0){
        delete p::ccmap;
    }
    if (pc::ai != NULL)
    {
        delete pc::ai;
    }
    if (pc::imagepool != 0)
    {
        delete pc::imagepool;
    }
    if (pc::gfxeng != NULL)
    {
        delete pc::gfxeng;
    }
    if (pc::sfxeng != NULL)
    {
        delete pc::sfxeng;
    }

    CleanConfig();

    VFSUtils::VFS_Destroy();

    pc::ai = 0;
    pc::input = 0;
    pc::cursor = 0;
    pc::sidebar = 0;
    p::dispatcher = 0;
    p::aequeue = 0;
    p::ccmap = 0;
    pc::imagepool = 0;
    pc::gfxeng = 0;
    pc::sfxeng = 0;
}

/**
 * Play the mission.
 */
void Game::play()
{
    bool missionWon = false;
    MissionMapsClass* missions = NULL;
    string missionName;

    // Create and load informations about maps of games
    // (from Mix archives)
    missions = new MissionMapsClass();

    do
    {
        // Create the new game classes
        InitializeGameClasses();

        // Menu to use
        Menu* lMenu = new Menu();

        // Start with a clean image cache
        pc::imgcache->Cleanup();


        //****************************
        // @todo DEBUG
        //****************************
/*
        pc::Config.UseFogOfWar	= true;
        string SelectedMap = missions->getGdiMissionMap(0);
        pc::Config.mapname	= SelectedMap;	//"SCU01EA";
        pc::Config.gamemode	= GAME_MODE_SINGLE_PLAYER;		// single player (missions)
        pc::Config.mside	= "gdi"; //"goodguy";
        pc::Config.cside	= "multi";
        pc::Config.side_colour = "yellow";
        pc::Config.pause = false;
        pc::Config.quit_mission = false;

        InitializeMap(pc::Config.mapname);
        pc::gfxeng->setupCurrentGame();

        p::ccmap->restoreLocation(0);
        //
        // main gameloop
        //
        pc::ai = new Ai;
        int k=0;
        PauseMenu* lPauseMenu1 = new PauseMenu();
        while (!pc::input->shouldQuit() && !pc::quit && k <1000*1000*10)
        {
            // first thing we want to do is scroll the map
            if (p::ccmap->toScroll()){
                p::ccmap->doscroll();
            }

            // Draw the pause menu (when needed)
            while (pc::Config.pause)
            {
                lPauseMenu1->HandleMenu();
            }

            // Draw the scene
            pc::gfxeng->renderScene();

            // Run scheduled events
            p::aequeue->runEvents();

            // Handle the input
            pc::input->handle();
            // Handle the ai
            pc::ai->handle();
            // Handle timing triggers
            HandleTiming();

            // Handle mission actions
            handleAiCommands();

            if (gamemode == 2)
            {
                // Synchronise events with server
            }
            //if (k==1)
            //	SDL_SaveBMP(pc::gfxeng->get_SDL_ScreenSurface(), "out.bmp");
            k++;
            //fflush(stdout);
            //logger->debug("k\n");
        }
        logger->debug("end of DEBUG\n");
        exit(0);
*/


        // Draw the starting menu
        if (!missionWon)
        {
            while (false == lMenu->isDone() ||
                   (true == lMenu->isDone() && true == lMenu->isProlog()))
            {
                // Load the Menu sound "intro.aud"
                pc::sfxeng->LoadSound("intro.aud");
                // Play this sound
                pc::sfxeng->PlayLoopedSound("intro.aud", 0);

                // Play the battle control terminated sound (if needed)
                if (BattleControlTerminated)
                {
                    pc::sfxeng->PlaySound(pc::Config.BattleControlTerm);
                }
                // Start with a clean image cache
                //pc::imgcache->Cleanup();

                // Draw the menu and handle menu input
                lMenu->HandleMenu();

                // Cleanup the image cache again
                //pc::imgcache->Cleanup();

                if (true == lMenu->isProlog())
                {
                    // Halt the menu sound
                    pc::sfxeng->StopMusic();
                    pc::sfxeng->StopLoopedSound(-1);

                    try
                    {
                        VQAMovie mov("PROLOG");
                        mov.play();
                    } catch (...)
                    {
                    }
                }
            }
        }

        BattleControlTerminated = false;

        // Halt the menu sound
        pc::sfxeng->StopMusic();
        pc::sfxeng->StopLoopedSound(-1);

        // Exit the game (if the user wants to)
        if (true == lMenu->isQuit())
        {
            Logger::getInstance()->Info("Exit by user (game menu)\n");
            FreeMemory();
            return;
        }

        if (pc::Config.gamemode == GAME_MODE_SINGLE_PLAYER)
        {
            if (pc::Config.mside == "gdi")
            {
                pc::Config.mapname = missions->getGdiMissionMap(MissionNr);
            }
            else if (pc::Config.mside == "nod")
            {
                pc::Config.mapname = missions->getNodMissionMap(MissionNr);
            }
            else
            {
                Logger::getInstance()->Error(__FILE__ , __LINE__, "Unknown mission type");
            }
        }

        // Initialize (load) the map
        InitializeMap(pc::Config.mapname);

        // Start playing the background music
        if (p::ccmap->getMissionData().theme != "No theme")
        {
            pc::sfxeng->PlayTrack(p::ccmap->getMissionData().theme);
        }


        // Play Animation Map
        if (pc::Config.gamemode == GAME_MODE_SINGLE_PLAYER)
        {
            MapAnimationMenu myAnimMenu;
            // @todo change that to get the good parameters
           //             myAnimMenu.Play(*(pc::gfxeng), MissionNr, true);
        }
        

        // Setup the current game
        pc::gfxeng->setupCurrentGame();

        // Check use of Fog of War
        if (!pc::Config.UseFogOfWar)
        {
            p::ccmap->getPlayerPool()->getLPlayer()->setVisBuild(Player::SOB_SIGHT, true);
        }

        if (p::ccmap->getGameMode() == GAME_MODE_SINGLE_PLAYER)
        {
            /// Jump to start location
            /// @todo: Jump to correct start location in multiplayer games.
            p::ccmap->restoreLocation(0);
        }
        else
        {
            // The following is a hack that sets the location
            // to the current players location
            Player *LocalPlayer = p::ccmap->getPlayerPool()->getLPlayer();

            Uint32 StartPos = LocalPlayer->getPlayerStart();

            // printf ("Startpos = %i, Current scrollpos = %i\n", StartPos, p::ccmap->getScrollPos());
            Uint32 x, y;

            if (StartPos%p::ccmap->getWidth()> 14)
            {
                x = StartPos%p::ccmap->getWidth() - 14;
            }
            else
            {
                x = StartPos%p::ccmap->getWidth();
            }

            if (StartPos/p::ccmap->getWidth()> 10)
            {
                y = StartPos/p::ccmap->getWidth() - 10;
            }
            else
            {
                y = StartPos/p::ccmap->getWidth() - 10;
            }

            if (x> p::ccmap->getWidth())
            {
                x = p::ccmap->getWidth();
            }

            if (y> p::ccmap->getHeight())
            {
                y = p::ccmap->getHeight();
            }

            p::ccmap->setScrollPos(x, y);
        }

        // printf ("Startpos = %i, Current scrollpos = %i\n", StartPos, p::ccmap->getScrollPos());

        OldUptime = 0;

        //
        // main gameloop
        //
        pc::ai = new Ai();
        PauseMenu* lPauseMenu = new PauseMenu();
        // Create the trigger manager
        TriggerManager lTriggerManager(p::ccmap);

        while (!pc::input->shouldQuit() && !pc::quit)
        {
            // first thing we want to do is scroll the map
            if (p::ccmap->toScroll()){
                p::ccmap->doscroll();
            }

            // Draw the pause menu (when needed)
            while (pc::Config.pause)
            {
                //pc::PauseMenu->HandleMenu();
                lPauseMenu->HandleMenu();
            }

            // Draw the scene
            pc::gfxeng->renderScene();

            // Run scheduled events
            p::aequeue->runEvents();

            // Handle the input
            pc::input->handle();

            // Handle the ai
            //pc::ai->handle();

            // Handle triggers
            lTriggerManager.handle();
            // Handle AiCommand for mission
            //handleAiCommands();
            // Handle timing triggers
            //HandleTiming();
                        

            if (gamemode == GAME_MODE_MULTI_PLAYER)
            {
                // Synchronise events with server
            }
        }

        BattleControlTerminated = true;

        // Stop all the music
        pc::sfxeng->StopMusic();
        pc::sfxeng->StopLoopedSound(-1);

    // Check if it was a single player game
        if (gamemode == GAME_MODE_SINGLE_PLAYER)
        {
            // Get Local Player
            Player* localPlayer = p::ccmap->getPlayerPool()->getPlayer(p::ccmap->getMissionData().player);            // ? if player won ?
            if (localPlayer->isVictorious() == true)
            {
                missionWon = true;
                MissionNr++;
                try
                {
                    VQAMovie mov(p::ccmap->getMissionData().winmov.c_str());
                    mov.play();
                }
                catch (runtime_error&)
                {
                }
            }
            else if (localPlayer->isDefeated() == true) 
                {
                    missionWon = false;
                    try {
                        VQAMovie mov(p::ccmap->getMissionData().losemov.c_str());
                        mov.play();
                    }
                    catch (runtime_error&)
                    {}
                }
                else
                {
                // Game was abborted
                missionWon = false;
               }
    }

        // Stop all the music
        pc::sfxeng->StopMusic();
        pc::sfxeng->StopLoopedSound(-1);

        // Write game stats in the .log
        //dumpstats();

        // Free objects
        //FreeMemory();

        // Check if it's the last mission
        if (p::ccmap->isEndOfGame()) {
            // Game is over
            missionWon = false;
        }
    }
    while (!pc::quit);

    // Free missions names loader (not needed)
    //delete missions;
}

/*void Game::HandleTiming()
{
    Uint32 uptime;
    // get elapsed time
    uptime = p::aequeue->getElapsedTime();
    // elapsed time in sec.
    uptime /= 1000;
    if ((uptime - OldUptime) > 6)
    {
        HandleGlobalTrigger(TRIGGER_EVENT_TIME_ELAPSED, uptime/6);
        OldUptime = uptime;
    }
}*/

/**
 * Prints stats of a game session
 */
void Game::dumpstats()
{
    Player* pl;
    Uint8 h, m, s, i;
    Uint32 uptime;

    uptime = p::aequeue->getElapsedTime();
    uptime /= 1000;
    h = uptime/3600;
    uptime %= 3600;
    m = uptime/60;
    s = uptime%60;

    // TODO check this message : logger->renderGameMsg(false);
    //logger->gameMsg("Time wasted: %i hour%s%i minute%s%i second%s", h,
    //		(h!=1 ? "s " : " "), m, (m!=1 ? "s " : " "), s, (s!=1 ? "s " : " "));
    
    for (i = 0; i < p::ccmap->getPlayerPool()->getNumPlayers(); i++)
    {
        pl = p::ccmap->getPlayerPool()->getPlayer(i);
        /*logger->gameMsg("%s\nUnit kills:  %i\n     losses: %i\n"
            "Structure kills:  %i\n          losses: %i\n", pl->getName().c_str(),
                pl->getUnitKills(), pl->getUnitLosses(),
                pl->getStructureKills(), pl->getStructureLosses());*/
    }
}

/**
 *
 */
void Game::handleAiCommands()
{
    for (unsigned int n = 0; n<p::uspool->getNumbUnits(); n++)
    {
        Unit* unit = p::uspool->getUnit(n);
        if (unit!=0)
        {
            if (!unit->IsMoving() && !unit->IsAttacking() && !unit->IsHarvesting())
            {
                if (unit->aiCommandList.size()>0)
                {
                    AiCommand* com = unit->aiCommandList[0];
                    //if (com->getId() == 3 || com->getId() == 8)
                    {
                        MACRO_LOG_DEBUG("AICOMMAND MOVE UNIT = " + unit->getType()->getName())
                        
                        Uint32 pos = p::ccmap->getWaypoint(com->getWaypoint());
                        //logger->debug("here$-1%d\n", pos);

                        unit->move(pos, true);
                        vector<AiCommand*>::iterator i = unit->aiCommandList.begin();
                        unit->aiCommandList.erase(i);
                    }
                }
            }
        }
    }
}
