// Game.h
// 1.0

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

#ifndef GAME_H
#define GAME_H

#include <string>

#include "SDL/SDL_types.h"

#include "include/config.h" 

using std::string;

/**
 * 
 */
class Game
{
public:
    Game();
    ~Game();
	void HandleGameMenu();
	void InitializeMap(string MapName);
	void InitializeGameClasses (void);
	void FreeMemory();
    void play();
    void HandleTiming (void);
    void dumpstats();

private:
	Uint8 MissionNr;
	Uint32 OldUptime;
    ConfigType config;
    Uint8 gamemode;
	bool BattleControlTerminated;
};

#endif //GAME_H
