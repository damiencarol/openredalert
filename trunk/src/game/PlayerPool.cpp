// PlayerPool.cpp
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

#include "PlayerPool.h"

#include <vector>

#include "Logger.hpp"
#include "ActionEventQueue.h"
#include "Player.h"
#include "misc/config.h"
#include "misc/INIFile.h"
#include "GameMode.h"

using std::vector;

namespace pc
{
    extern ConfigType Config;
}

/**
 */
PlayerPool::PlayerPool()
{
	// RAZ of players starts
	for (unsigned int i = 0; i < 8; i++)
    {
        playerstarts[i] = 0;
    }
    
    // By default no update of the sidebar
    updatesidebar = false;
    
    // Create the default player list
    playerpool.push_back(new Player("Spain"));
    playerpool.push_back(new Player("Greece"));
    playerpool.push_back(new Player("USSR"));
    playerpool.push_back(new Player("England"));
    playerpool.push_back(new Player("Germany"));
    playerpool.push_back(new Player("France"));
    playerpool.push_back(new Player("Turkey"));
    
    playerpool.push_back(new Player("GoodGuy"));
    playerpool.push_back(new Player("BadGuy"));
    playerpool.push_back(new Player("Neutral"));
    playerpool.push_back(new Player("Special"));
    
    // Create players for scirmish
    playerpool.push_back(new Player("Multi1"));
    playerpool.push_back(new Player("Multi2"));
    playerpool.push_back(new Player("Multi3"));
    playerpool.push_back(new Player("Multi4"));
    playerpool.push_back(new Player("Multi5"));
    playerpool.push_back(new Player("Multi6"));
    playerpool.push_back(new Player("Multi7"));
    playerpool.push_back(new Player("Multi8"));    
}

/**
 * @param gamemode xxx
 */
void PlayerPool::Init(Uint8 gamemode)
{
}

/**
 * @param inifile Ini file where to laod the info
 */
void PlayerPool::LoadIni(INIFile *inifile)
{
	// Load ini informations
	for (unsigned int i = 0; i < playerpool.size(); i++)
	{
		Player* thePlayer = playerpool.at(i);
		thePlayer->LoadIni(inifile);
	}
	
	// Set the local player
	if  (gamemode == GAME_MODE_SINGLE_PLAYER)
	{
		// Test if the key exist
		if (inifile->isKeyInSection("Basic", "Player") == true)
		{
			// Set the local player with info n the ini file
			setLPlayer(inifile->readString("Basic", "Player"));
		}
	}
	
	// Set the alliances between players
	setAlliances(inifile);
}

/**
 */
PlayerPool::~PlayerPool()
{
	// Free all Player in the vector
	for (unsigned int i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i] != 0)
		{
			delete playerpool[i];
		}
	}
}

/**
 * @param colour Color
 * @return Number of the color
 */
const int PlayerPool::MultiColourStringToNumb(const string& colour)
{
    if (colour == "yellow")
    {
        return 0;
    }
    else if (colour == "blue")
    {
        return 1;
    }
    else if (colour == "red")
    {
        return 2;
    }
    else if (colour == "green")
    {
        return 3;
    }
    else if (colour == "yellow")
    {
        return 4;
    }
    else if (colour == "orange")
    {
        return 5;
    }
    else if (colour  == "gray")
    {
        return 6;
    }
    else if (colour == "turquoise")
    {
        return 7;
    }
    else if (colour == "darkred")
    {
        return 8;
    }
    else
    {
        // Error
        // @todo do something better than this :)
        return 1;
    }
}

void PlayerPool::setLPlayer(const string& pname)
{
    for (unsigned int i = 0; i < playerpool.size(); i++)
    {
        if (playerpool.at(i)->getName() == pname)
        {
            localPlayer = i;
            return;
        }
    }
    
    Logger::getInstance()->Warning("Tried to set local player to non-existing player "
        + pname);

    /*playerpool.push_back(new Player(pname.c_str(), mapini));
    localPlayer = static_cast<Uint8>(playerpool.size()-1);
    playerpool[localPlayer]->setPlayerNum(localPlayer);*/
}

/*void PlayerPool::setLPlayer(Uint8 number, const char* nick, const char* colour,
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
	//  playerpool.push_back(new Player("multi", mapini, this));
	//localPlayer = playerpool.size()-1;
	// playerpool[localPlayer]->setSettings(nick,colour,mside);
	 
}*/

/*void PlayerPool::setPlayer(Uint8 number, const char* nick, const int colour,
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
	//  playerpool.push_back(new Player("multi", mapini, this));
	// localPlayer = playerpool.size()-1;
	// playerpool[localPlayer]->setSettings(nick,colour,mside);
	
}*/

/**
 * Return the number of the player with this name
 */
int PlayerPool::getPlayerNum(const string& pname)
{
    // Parse each players to found it
    for (unsigned int i = 0; i < playerpool.size(); i++)
    {
        if (playerpool.at(i)->getName() == pname)
        {
            return i;
        }
    }

    //logger->error("%s line %i: Create new player: %s\n", __FILE__, __LINE__, pname.c_str());
    //playerpool.push_back(new Player(pname.c_str(), mapini));
    //playerpool[playerpool.size() - 1]->setPlayerNum(playerpool.size() - 1);
    //return playerpool.size() - 1;
    
    return -1;
}

char RA_house[20][10] =
{ "Spain", "Greece", "Ussr", "England", "Ukraine", "Germany", "France",
		"Turkey", "Goodguy", "Badguy", "Special", "Neutral", "Multi1",
		"Multi2", "Multi3", "Multi4", "Multi5", "Multi6", "Multi7", "Multi8" 
};

/**
 * Decode player number coded in ini files
 * 
 * @param house number of the house
 */
int PlayerPool::getPlayerNumByHouseNum(int house) const
{
    // Check if num <19 (because their are only 20 houses)
    if (house > 19 || house < 0)
    {
        return -1;
    }

    for (unsigned int i = 0; i < playerpool.size(); i++)
    {
        if (playerpool[i]->getName() == RA_house[house])
        {
            return i;
        }
    }
    
    // No player found
    return -1;
}

/**
 * @param playerNumber number of the player
 */
int PlayerPool::getHouseNumByPlayerNum(unsigned int playerNumber) const
{
    // If the player is not in the pool
    if (playerNumber >= playerpool.size())
    {
        // Return -1 (WRONG)
        return -1;
    }

    // Parse the vector
    for (unsigned int i = 0; i < playerpool.size(); i++)
    {
        if (string(playerpool[playerNumber]->getName()) == RA_house[i])
        {
            return i;
        }
    }

    // Return -1 (WRONG)
    return -1;
}

Player* PlayerPool::getPlayer(const string& pname)
{
    return getPlayerByName(pname.c_str());
}

Player* PlayerPool::getPlayerByName(const char* pname)
{
	return playerpool[getPlayerNum(pname)];
}

/*
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
*//*
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
	
	
	// If it's not single player mission
	if (gamemode == GAME_MODE_SKIRMISH ||
		gamemode == GAME_MODE_MULTI_PLAYER)
	{
		logger->gameMsg("Player \"%s\" defeated", pl->getName().c_str());
	}
}

void PlayerPool::playerUndefeated(Player* pl)
{
	// @todo check that
	//pl->setAlliances();
	
	unsigned int i = 0;
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
	if (gamemode == 0)//GAME_MODE_SINGLE_PLAYER)
	{
		if (!lost)
		{
			won = false;
		}
	}
}
*/
/**
 * 
 *//*
void PlayerPool::playerVictorious(Player* pl)
{
	Uint8 i;

	//pl->setAlliances();
	for (i = 0; i < playerpool.size(); i++)
	{
		if (playerpool[i] == pl)
		{
			pl->setVictorious(true);
			break;
		}
	}
	
	if (gamemode == 0)//GAME_MODE_SINGLE_PLAYER)
	{
		if (i == localPlayer)
		{
			lost = false;
			won = true;
		}
	}
}
*/
/*INIFile* PlayerPool::getMapINI()
{
	return mapini;
}*/

void PlayerPool::setAlliances(INIFile* mapini)
{
	// @todo refactor this to set by player the ally
	for (unsigned int i = 0; i < playerpool.size(); i++)
	{
		playerpool[i]->setAlliances(mapini);
	}
}
/*
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
}*/

/**
 * @return Random start in the map
 */
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

	// Check if power is Ok
	powerOk = (localPlayer->getPower()>localPlayer->getPowerUsed());

	// by default
	res = 0;
	
	// If same number of radars
	if (old_numRadarLocalPlayer == localPlayer->getNumberRadars()) 
	{
		// if old their was radars
		if (old_numRadarLocalPlayer >0) 
		{
			if (powerOk == false && old_powerOk == true)
			{
				res = 3;
			} else if (powerOk == true && old_powerOk == false)
			{
				res = 1;
			}
		}
	}
	else
	// If more radar
	if (localPlayer->getNumberRadars() > old_numRadarLocalPlayer)
	{
		// if old their was no radars
		if (old_numRadarLocalPlayer == 0) 
		{			
			if (powerOk == true)
			{
				res = 1;
			} else {
				res = 3;
			}
			
		}
	}
	else
	// There are less radar
	if (localPlayer->getNumberRadars() < old_numRadarLocalPlayer)
	{
		// if there are no radar
		if (localPlayer->getNumberRadars() == 0) 
		{
			res = 2;
		} 
		else 
		{
			if (powerOk == true && old_powerOk == false)
			{
				res = 1;
			} else if (powerOk == false && old_powerOk == true) 
			{
				res = 2;			
			}
		}
	}
	
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

Player* PlayerPool::getPlayer(int playerNumber) const
{
    if (playerNumber >=0 && playerNumber < playerpool.size())
    {
        return playerpool[playerNumber];
    }
    // Return NULL
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

/**
 * Return if the Local player has WON the mission
 */
/*bool PlayerPool::hasWon() const
{
	return won;
}*/

/**
 * Return if the Local player has LOST the mission
 */
/*bool PlayerPool::hasLost() const
{
	return lost;
}*/
