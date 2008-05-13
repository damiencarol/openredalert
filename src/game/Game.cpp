// Game.cpp
// 1.5

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

#include "Game.h"

#include <stdexcept>

#include "ActionEventQueue.h"
#include "include/ccmap.h"
#include "include/dispatcher.h"
#include "video/GraphicsEngine.h"
#include "include/Input.h"
#include "include/PlayerPool.h"
#include "include/config.h"
#include "ui/Sidebar.h"
#include "audio/SoundEngine.h"
#include "audio/SoundError.h"
#include "game/Ai.h"
#include "game/Player.h"
#include "vfs/vfs.h"
#include "video/ImageCache.h"
#include "ui/Menu.h"
#include "ui/Cursor.h"
#include "ui/PauseMenu.h"
#include "GameMode.h"
#include "include/Logger.h"
#include "game/MissionData.h"
#include "ui/SidebarError.h"
#include "vfs/mix/mixvfs.h"
#include "video/VQAMovie.h"
#include "MissionMapsClass.h"
#include "video/SHPImage.h"
#include "RedAlertDataLoader.h"
#include "GameError.h"
#include "LoadMapError.h"
#include "video/VideoError.h"

using std::string;
using std::runtime_error;
using VQA::VQAMovie;

extern Logger * logger;

namespace p
{
	extern PlayerPool * ppool;
	extern ActionEventQueue* aequeue;
	extern RedAlertDataLoader * raLoader;
}
namespace pc
{
	extern GraphicsEngine * gfxeng;
	extern Ai * ai;
	extern ConfigType Config;
	extern vector<SHPImage *> *imagepool;	
}
extern MIXFiles * mixfiles;

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
 * Game menu (early game start)
 */
void Game::HandleGameMenu()
{
	Menu lMenu;

	// Start with a clean image cache
	pc::imgcache->Cleanup();

	// Draw the menu and handle menu input
	lMenu.HandleMenu();

	// Cleanup the image cache again
	pc::imgcache->Cleanup();
}

/** 
 * Initialize game (after the menu)
 */
void Game::InitializeMap(string MapName)
{
	// Copy the game mode
	this->gamemode = pc::Config.gamemode;

	p::aequeue = new ActionEventQueue();

	// Initialize the map
	try
	{
		p::ccmap = new CnCMap();
		p::ccmap->Init(GAME_RA, this->gamemode);
		p::ccmap->loadMap(pc::Config.mapname.c_str(), NULL);
	}
	catch (LoadMapError& ex)
	{
		logger->error("LoadMapError\n", ex.what());
		// loadmap will have printed the error
		throw GameError("Error during load of the Map in Game::InitializeMap\n");
	}

	p::dispatcher = new Dispatcher::Dispatcher();
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
		logger->error("Invalid dispatch mode: %i\n", pc::Config.dispatch_mode);
		throw GameError("Invalid dispatch mode:");
		break;
	}

	switch (gamemode)
	{
	case 0:
		try
		{
			// Try to Play the "Briefing" Movie
			char * tmpBrief;
			tmpBrief = new char[strlen(p::ccmap->getMissionData()->brief)+1];
			strcpy(tmpBrief, p::ccmap->getMissionData()->brief);
			logger->note ("%s line %i: Brief = %s\n", __FILE__, __LINE__, tmpBrief);
			VQAMovie movBrief(tmpBrief);
			movBrief.play();

			// Try to Play the "Action" Movie
			char * tmpAction;
			tmpAction = new char[strlen(p::ccmap->getMissionData()->action)+1];
			strcpy(tmpAction, p::ccmap->getMissionData()->action);
			logger->note ("%s line %i: Action = %s\n", __FILE__, __LINE__, tmpAction);
			VQAMovie actionMovie(tmpAction);
			actionMovie.play();

		}
		catch (std::runtime_error&)
		{
		}
		break;
	case 1:
	case 2:
	default:
		break;
	}

	// init sidebar
	try
	{
		pc::sidebar = new Sidebar( p::ppool->getLPlayer(), pc::gfxeng->getHeight(), p::ccmap->getMissionData()->theater );
	}
	catch (SidebarError)
	{
		// Log it
		logger->error("Can't construct the Sidebar");
		// Throw game error
		throw GameError("Can't construct the Sidebar");
	}

	// init cursor
	pc::cursor = new Cursor();

	// init the input functions
	pc::input = new Input( pc::gfxeng->getWidth(), pc::gfxeng->getHeight(), pc::gfxeng->getMapArea() );

}

void Game::InitializeGameClasses()
{
	// Initialise Video
	try
	{
		logger->note("Initialising the graphics engine...");
		pc::gfxeng = new GraphicsEngine();
		logger->note("done\n");
	}
	catch (VideoError& ex)
	{
		logger->note("failed.  %s \n", ex.what());
		throw runtime_error("Unable to initialise the graphics engine");
	}

	// Initialise Sound
	try
	{
		logger->note("Initialising the sound engine...");
		pc::sfxeng = new SoundEngine(pc::Config.nosound);
		logger->note("done\n");
	}
	catch (SoundError&)
	{
		logger->note("failed.  exiting\n");
		throw runtime_error("Unable to initialise the sound engine");
	}

	VFSUtils::VFS_PreInit(pc::Config.binpath.c_str());
	VFSUtils::VFS_Init(pc::Config.binpath.c_str());
	VFSUtils::VFS_LoadGame(pc::Config.gamenum);

	// Get the mission maps
	//pc::MissionsMapdata = new MissionMapsClass();

	// Init the image cache
	pc::imagepool = new std::vector<SHPImage*>();
	pc::imgcache->setImagePool(pc::imagepool);

	// Init the Data Loader
	try
	{
		logger->note("Initialising the RA Data loader...");
		p::raLoader = new RedAlertDataLoader();
		p::raLoader->load();
		logger->note("done\n");
	}
	catch (...)
	{
		logger->note("failed.  exiting\n");
		throw runtime_error("Unable to initialise the RA Data loader");
	}

	// Load the music files (for background music	
	// Create playlist with all music of RedAlert
	if (pc::sfxeng->CreatePlaylist() != true)
	{
		//
		logger->error("Could not create playlist!\n");
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
	if (pc::imgcache != NULL)
	{
		pc::imgcache->Cleanup();
	}
	if (pc::input != NULL)
	{
		delete pc::input;
	}
	if (pc::cursor != NULL)
	{
		delete pc::cursor;
	}
	if (pc::sidebar != NULL)
	{
		delete pc::sidebar;
	}
	if (p::dispatcher != NULL)
		delete p::dispatcher;
	if (p::aequeue != NULL)
		delete p::aequeue;
	if (p::ccmap != NULL)
		delete p::ccmap;
	if (pc::ai != NULL)
	{
		delete pc::ai;
	}
	if (pc::imagepool != NULL)
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

		
		// TODO debug datum loader
/*
		//INIFile* rules = new INIFile("rules.ini");
		//RedAlertDataLoader loader;
		//loader.load();
		CnCMap* map = new CnCMap();
		logger->debug("0\n");
		map->InitCnCMap();
		logger->debug("1\n");
		map->loadMap("scg01ea", NULL);
		logger->debug("2\n");
		exit(0);
*/
		// TODO DEBUG
		CnCMap* themap;
		themap = new CnCMap();
		logger->debug("$create ok\n");
		themap->Init(GAME_RA, GAME_MODE_SINGLE_PLAYER);
		logger->debug("$init ok\n");
		themap->loadMap("scg01ea", 0);
		logger->debug("$load ok\n");
		exit(0);
					
		// Load the Menu sound "intro.aud"
		pc::sfxeng->LoadSound("intro.aud");
		// Play this sound
		pc::sfxeng->PlayLoopedSound("intro.aud", 0);
		
		// Draw the starting menu
		if (!missionWon)
		{
			// Play the battle control terminated sound (if needed)
			if (BattleControlTerminated)
			{
				pc::sfxeng->PlaySound(pc::Config.BattleControlTerm);
			}
			HandleGameMenu();
		}

		BattleControlTerminated = false;
		
		// Halt the menu sound
		pc::sfxeng->StopMusic();
		pc::sfxeng->StopLoopedSound(-1);

		// Exit the game (if the user wants to)
		if (pc::quit)
		{
			logger->note("Exit by user\n");
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
				logger->note("%s line %i: Unknown mission type\n",__FILE__ , __LINE__);
			}
		}

		logger->debug("Mission name = %s\n", pc::Config.mapname.c_str());

		// Initialize (load) the map
		InitializeMap( pc::Config.mapname );
		
		// Start playing the background music
		pc::sfxeng->PlayTrack(p::ccmap->getMissionData()->theme);

		// Setup the current game
		pc::gfxeng->setupCurrentGame();

		// Check use of Fog of War
		if (!pc::Config.UseFogOfWar)
		{
			logger->note ("%s line %i: Use no fog of war?\n", __FILE__, __LINE__);
			p::ppool->getLPlayer()->setVisBuild(Player::SOB_SIGHT, true);
		}

		if (p::ccmap->getGameMode() == GAME_MODE_SINGLE_PLAYER)
		{
			/// Jump to start location
			/// @TODO: Jump to correct start location in multiplayer games.
			p::ccmap->restoreLocation(0);
		}
		else
		{
			// The following is a hack that sets the location
			// to the current players location
			Player *LocalPlayer = p::ppool->getLPlayer();

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
		pc::ai = new Ai;
		//pc::PauseMenu = new PauseMenu();
		PauseMenu * lPauseMenu = new PauseMenu();
		while (!pc::input->shouldQuit() && !pc::quit)
		{

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
			pc::ai->handle();

			// Handle timing triggers
			HandleTiming();

			if (gamemode == 2)
			{
				// Synchronise events with server
			}
		}

		BattleControlTerminated = true;

		// Stop the music
		pc::sfxeng->StopMusic();

		// Check if it was a single player game
		if (gamemode == GAME_MODE_SINGLE_PLAYER)
		{
			// ? if player won ?
			if (p::ppool->hasWon())
			{
				missionWon = true;
				MissionNr++;
				try
				{
					VQAMovie *mov = new VQAMovie(p::ccmap->getMissionData()->winmov);
					mov->play();
				}
				catch (std::runtime_error&)
				{
				}
			}
			else if (p::ppool->hasLost() )
			{
				missionWon = false;
				try
				{
					VQAMovie *mov = new VQAMovie(p::ccmap->getMissionData()->losemov);
					mov->play();
				}
				catch (std::runtime_error&)
				{
				}
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
		dumpstats();

		// Free objects
		FreeMemory();
	}
	while (!pc::quit);

	// Free missions names loader (not needed)
	//delete missions;
}

void Game::HandleTiming()
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
}

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

	logger->renderGameMsg(false);
	logger->gameMsg("Time wasted: %i hour%s%i minute%s%i second%s", h,
			(h!=1 ? "s " : " "), m, (m!=1 ? "s " : " "), s, (s!=1 ? "s " : " "));
	for (i = 0; i < p::ppool->getNumPlayers(); i++)
	{
		pl = p::ppool->getPlayer(i);
		logger->gameMsg("%s\nUnit kills:  %i\n     losses: %i\n"
			"Structure kills:  %i\n          losses: %i\n", pl->getName(),
				pl->getUnitKills(), pl->getUnitLosses(),
				pl->getStructureKills(), pl->getStructureLosses());
	}
}
