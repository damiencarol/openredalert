// Game.h
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

#ifndef GAME_H
#define GAME_H

#include <string>

#include "SDL/SDL_types.h"

#include "include/config.h" 

using std::string;

/**
 * This object represent a game session
 */
class Game
{
public:
	Game();
	~Game();

	void InitializeMap(string MapName);
	/** Initialise some object of the game */
	void InitializeGameClasses();
	void FreeMemory();
	void play();
	void HandleTiming();
	void dumpstats();

private:
	void handleAiCommands();

	Uint8 MissionNr;
	Uint32 OldUptime;
	ConfigType config;
	Uint8 gamemode;
	bool BattleControlTerminated;
};

#endif //GAME_H
