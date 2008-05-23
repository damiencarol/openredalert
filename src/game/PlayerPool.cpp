#include "include/PlayerPool.h"

#include <vector>

#include "ActionEventQueue.h"
#include "Player.h"
#include "include/config.h"

#include "misc/INIFile.h"

using std::vector;

namespace pc
{
extern ConfigType Config;
}

PlayerPool::PlayerPool(INIFile *inifile, Uint8 gamemode)
{
	for (int i = 0; i < 8; i++)
	{
		playerstarts[i] = 0;
	}
	lost = false;
	won = false;
	mapini = inifile;
	updatesidebar = false;
	this->gamemode = gamemode;
}

PlayerPool::~PlayerPool()
{
	Uint8 i;
	for (i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i] != NULL)
		{
			delete playerpool[i];
		}
		playerpool[i] = NULL;
	}
}

const int PlayerPool::MultiColourStringToNumb(const char* colour)
{
	if (strcasecmp(colour, "yellow") == 0)
		{
			return 0;
		}
		else if (strcasecmp(colour, "blue") == 0)
		{
			return 1;
		}
		else if (strcasecmp(colour, "red") == 0)
		{
			return 2;
		}
		else if (strcasecmp(colour, "green") == 0)
		{
			return 3;
		}
		else if (strcasecmp(colour, "yellow") == 0)
		{
			return 4;
		}
		else if (strcasecmp(colour, "orange") == 0)
		{
			return 5;
		}
		else if (strcasecmp(colour, "gray") == 0)
		{
			return 6;
		}
		else if (strcasecmp(colour, "turquoise") == 0)
		{
			return 7;
		}
		else if (strcasecmp(colour, "darkred") == 0)
		{
			return 8;
	} else {
		// Error TODO -> do something better than this :)
		return 1;
	}
}

void PlayerPool::setLPlayer(const char* pname)
{
	Uint8 i;
	for (i = 0; i < playerpool.size(); i++)
	{
		if ( !strcasecmp(playerpool[i]->getName(), pname) )
		{
			localPlayer = i;
			return;
		}
	}
	//logger->warning("Tried to set local player to non-existing player \"%s\"\n", pname);
	playerpool.push_back(new Player(pname, mapini));
	localPlayer = static_cast<Uint8>(playerpool.size()-1);
	playerpool[localPlayer]->setPlayerNum(localPlayer);
}

void PlayerPool::setLPlayer(Uint8 number, const char* nick, const char* colour,
		const char* mside)
{
	Uint8 i;
	for (i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i]->getMSide() == number)
		{
			localPlayer = i;
			playerpool[i]->setSettings(nick, colour, mside);
			return;
		}
	}
	//logger->warning("Tried to set local player to non-existing player number %i\n", number);
	/*  playerpool.push_back(new Player("multi", mapini, this));
	 localPlayer = playerpool.size()-1;
	 playerpool[localPlayer]->setSettings(nick,colour,mside);
	 */
}

void PlayerPool::setPlayer(Uint8 number, const char* nick, const int colour,
		const char* mside)
{
	Uint8 i;
	for (i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i]->getMSide() == number)
		{
			//localPlayer = i;
			playerpool[i]->setSettings(nick, colour, mside);
			return;
		}
	}
	//logger->warning("Tried to set local player to non-existing player number %i\n", number);
	/*  playerpool.push_back(new Player("multi", mapini, this));
	 localPlayer = playerpool.size()-1;
	 playerpool[localPlayer]->setSettings(nick,colour,mside);
	 */
}

Uint8 PlayerPool::getPlayerNum(const char *pname)
{
	Uint8 i;
	
	
	
	for (i = 0; i < playerpool.size(); i++)
	{
		if ( !strcasecmp(playerpool[i]->getName(), pname) )
		{
			return i;
		}
	}
	//logger->error("%s line %i: Create new player: %s\n", __FILE__, __LINE__, pname);
	playerpool.push_back(new Player(pname, mapini));
	playerpool[playerpool.size()-1]->setPlayerNum(static_cast<Uint8>(playerpool.size()-1));
	return static_cast<Uint8>(playerpool.size()-1);
}

char RA_house[20][10] =
{ "Spain", "Greece", "Ussr", "England", "Ukraine", "Germany", "France",
		"Turkey", "Goodguy", "Badguy", "Special", "Neutral", "Multi1",
		"Multi2", "Multi3", "Multi4", "Multi5", "Multi6", "Multi7", "Multi8" 
};

int PlayerPool::getPlayerNumByHouseNum(Uint8 House)
{
	Uint8 i;

	// Check if num <19 (because their are only 20 houses)
	if (House > 19)
	{
		return -1;
	}

	for (i = 0; i < playerpool.size(); i++)
	{
		if ( !strcasecmp(playerpool[i]->getName(), RA_house[House]) )
		{
			return i;
		}
	}
	return -1;
}

int PlayerPool::getHouseNumByPlayerNum(Uint8 Player)
{
	Uint8 i;
	if (Player >= playerpool.size())
		return -1;

	for (i = 0; i < 19; i++)
	{
		if ( !strcasecmp(playerpool[Player]->getName(), RA_house[i]) )
		{
			return i;
		}
	}
	return -1;
}

Player* PlayerPool::getPlayerByName(const char* pname)
{
	return playerpool[getPlayerNum(pname)];
}

vector<Player*> PlayerPool::getOpponents(Player* pl)
{
	vector<Player*> opps;
	for (Uint8 i = 0; i < playerpool.size(); i++)
	{
		if (!playerpool[i]->isDefeated() && !pl->isAllied(playerpool[i]))
		{
			opps.push_back(playerpool[i]);
		}
	}
	return opps;
}

void PlayerPool::playerDefeated(Player *pl)
{
	Uint8 i;

	pl->clearAlliances();
	for (i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i] == pl)
		{
			break;
		}
	}
	if (i == localPlayer)
	{
		lost = true;
	}
	if (!lost)
	{
		Uint8 defeated = 0;
		for (i = 0; i < playerpool.size(); ++i)
		{
			if (playerpool[i]->isDefeated())
			{
				++defeated;
			}
		}
		if (playerpool.size() - defeated == 1)
		{
			won = true;
		}
		else if (playerpool.size() - defeated
				== playerpool[localPlayer]->getNumAllies() )
		{
			won = true;
		}
	}
}

void PlayerPool::playerUndefeated(Player* pl)
{
	Uint8 i;

	pl->setAlliances();
	for (i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i] == pl)
		{
			break;
		}
	}
	if (i == localPlayer)
	{
		lost = false;
	}
	if (gamemode == 0)
	{
		if (!lost)
		{
			won = false;
		}
	}
}

INIFile * PlayerPool::getMapINI()
{
	return mapini;
}

void PlayerPool::setAlliances()
{
	for (Uint16 i=0; i < playerpool.size() ; ++i)
	{
		playerpool[i]->setAlliances();
	}
}

void PlayerPool::placeMultiUnits()
{
	for (Uint16 i=0; i < playerpool.size() ; ++i)
	{
		if (playerpool[i]->getPlayerStart() != 0)
		{
			playerpool[i]->placeMultiUnits();
		}
		else
		{
			printf("%s line %i: Failed to get player start\n",__FILE__ , __LINE__);
		}
	}
}

Uint16 PlayerPool::getAStart()
{
	Uint8 rnd, sze = 0;
	Uint16 rv;

	for (int i = 0; i < 8; i++)
	{
		if (playerstarts[i] != 0)
			sze++;
	}

	// pick a starting location at random
	rnd = (int) ((double)sze*rand()/(RAND_MAX+1.0));
	while (playerstarts[rnd] == 0)
	{
		rnd = (int) ((double)sze*rand()/(RAND_MAX+1.0));
	}
	rv = playerstarts[rnd];
	// ensure this starting location is not reused.
	playerstarts[rnd] = 0;

	return rv;
}

/**
 * Set the start position of a player in the map
 */
void PlayerPool::setPlayerStarts(Uint8 pos, Uint32 start)
{
	//
	if (pos < 8){
		playerstarts[pos] = start;
	}
}

bool PlayerPool::pollSidebar()
{
	if (updatesidebar)
	{
		updatesidebar = false;
		return true;
	}
	return false;
}

void PlayerPool::updateSidebar()
{
	updatesidebar = true;
}

/**
 * Get the current status of the radar
 * 
 * case 0: // do nothing
 * case 1: // got radar
 * case 2: // lost radar
 * case 3: // radar powered down
 */ 
Uint8 PlayerPool::statRadar()
{
	static Uint32 old_numRadarLocalPlayer = 0;
	static bool old_powerOk = false;
	Player* localPlayer = 0;
	Uint8 res;
	bool powerOk;
	
	// Get the localPlayer
	localPlayer = getLPlayer();
	if (localPlayer == 0) // check
		return 0;

	// Check if power is Ok
	powerOk = (localPlayer->getPower()>localPlayer->getPowerUsed());

	// If no change
	if (old_numRadarLocalPlayer == localPlayer->getNumberRadars() &&
		old_powerOk == powerOk) 
	{
		res = 0;
	}
	else if (localPlayer->getNumberRadars()>0)
	{
		if (powerOk == true)
		{
			res = 1;
		}
		else
			res = 3;
	}
	else
		res = 2;
	
	// Save olds
	old_numRadarLocalPlayer = localPlayer->getNumberRadars();
	old_powerOk = powerOk;
		
	// Return result
	return res;
}

Uint8 PlayerPool::getNumPlayers() const
{
	return static_cast<Uint8>(playerpool.size());
}

Uint8 PlayerPool::getLPlayerNum() const
{
	return localPlayer;
}

Player* PlayerPool::getLPlayer()
{
	if (localPlayer < playerpool.size())
	{
		return playerpool[localPlayer];
	}
	return 0;
}

Player* PlayerPool::getPlayer(Uint8 player)
{
	if (player < playerpool.size()){
		return playerpool[player];
	}
	return 0;
}

Uint8 PlayerPool::getUnitpalNum(Uint8 player) const
{
	if (player < playerpool.size()){
		return playerpool[player]->getUnitpalNum();
	}
	return 0;
}

Uint8 PlayerPool::getStructpalNum(Uint8 player) const
{
	if (player < playerpool.size())
		return playerpool[player]->getStructpalNum();
	return 0;
}

bool PlayerPool::hasWon() const
{
	return won;
}

bool PlayerPool::hasLost() const
{
	return lost;
}
