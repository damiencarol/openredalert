// PlayerPool.h
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

#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#include <vector>
#include <string>

#include "SDL/SDL_types.h"
#include "misc/INIFile.h"

class Player;

using std::vector;
using std::string;

/** 
 * @todo Currently the player starts are shuffled randomly without 
 * any way of accepting a preshuffled list.
 */
class PlayerPool
{
public:
    explicit PlayerPool(INIFile *inifile, Uint8 gamemode);
    ~PlayerPool();
    /** Get the number of a color in string */
    const int MultiColourStringToNumb(const string& colour);

    Uint8 getNumPlayers() const ;
    Uint8 getLPlayerNum() const ;
    Player *getLPlayer();
    void setLPlayer(const string& pname);
    void setLPlayer(Uint8 number, const char* nick, const char* colour, const char* mside);
    void setPlayer(Uint8 number, const char* nick, const int colour, const char* mside);
    Player *getPlayer(Uint8 player);
    int getPlayerNum(const string& pname);
    Player* getPlayerByName(const char* pname);

    int getPlayerNumByHouseNum(int House) const;
    int getHouseNumByPlayerNum(int Player) const;

    Uint8 getUnitpalNum(Uint8 player) const ;
    Uint8 getStructpalNum(Uint8 player) const ;
    std::vector<Player*> getOpponents(Player* pl);
    void playerDefeated(Player * pl);
    void playerUndefeated(Player * pl);
    bool hasWon() const ;
    bool hasLost() const ;
    void setAlliances();
    void placeMultiUnits();
    INIFile * getMapINI();
    Uint16 getAStart();
    void setPlayerStarts(Uint8 pos, Uint32 start);

    /** Called by input to see if sidebar needs updating*/
    bool pollSidebar();

    /** Called by the local player when sidebar is to be updated*/
    void updateSidebar();

    /** Called by input to see if radar status has changed.*/
    Uint8 statRadar();

	/** Set a player victorious */
	void playerVictorious(Player* pl);

private:
    PlayerPool();
    PlayerPool(const PlayerPool&);

    Uint32 playerstarts[10];

    vector< Player *> playerpool;
    vector< Uint16 > player_starts;
    
    Uint8 localPlayer;
    Uint8 gamemode;
    bool won;
    bool lost;
    bool updatesidebar;
    INIFile * mapini;
};

#endif //PLAYERPOOL_H
