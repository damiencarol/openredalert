#ifndef GAME_H
#define GAME_H

#include <string>

#include "SDL/SDL_types.h"

#include "include/config.h" 

using std::string;


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
