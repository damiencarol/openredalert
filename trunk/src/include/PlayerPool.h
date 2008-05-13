#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#include <vector>

#include "SDL/SDL_types.h"
#include "misc/INIFile.h"

class Player;

using std::vector;

/** 
 * TODO Currently the player starts are shuffled randomly without 
 * any way of accepting a preshuffled list.
 */
class PlayerPool
{
public:
    explicit PlayerPool(INIFile *inifile, Uint8 gamemode);
    ~PlayerPool();
    const int MultiColourStringToNumb(const char* colour);
    Uint8 getNumPlayers() const ;
    Uint8 getLPlayerNum() const ;
    Player *getLPlayer();
    void setLPlayer(const char *pname);
    void setLPlayer(Uint8 number, const char* nick, const char* colour, const char* mside);
    void setPlayer(Uint8 number, const char* nick, const int colour, const char* mside);
    Player *getPlayer(Uint8 player);
    Uint8 getPlayerNum(const char *pname);
    Player* getPlayerByName(const char* pname);

    int getPlayerNumByHouseNum(Uint8 House);
    int getHouseNumByPlayerNum(Uint8 Player);

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

    /** Called by the local player to update the radar status*/
    void updateRadar(Uint8 status);
private:
    PlayerPool();
    PlayerPool(const PlayerPool&);

    Uint32 playerstarts[10];

    std::vector< Player *> playerpool;
    std::vector< Uint16 > player_starts;
    Uint8 localPlayer, gamemode, radarstatus;
    bool won, lost, updatesidebar;
    INIFile * mapini;
};

#endif //PLAYERPOOL_H
