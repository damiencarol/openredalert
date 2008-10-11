#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#include "SDL/SDL_video.h"
#include "SDL/SDL_types.h"
#include "SDL/SDL_keysym.h"

#include "misc/common.h"
#include "misc/gametypes.h"

using std::string;

namespace
{
const Uint8 NUMBINDABLE = 3;
}

enum KEY_TYPE
{
    KEY_SIDEBAR = 0,
    KEY_STOP,
    KEY_ALLY
};

typedef struct _ConfigType
{
    Uint32 videoflags;
    Uint32 startMoney;

    Uint16 width,
            height,
            bpp,
            serverport;

    Uint8 intro;
    /* Game mode */
    Uint8 gamemode;
    Uint8 totalplayers,
            playernum,
            scrollstep,
            scrolltime,
            maxscroll,
            finaldelay,
            dispatch_mode;

    bool nosound;
    bool pause;
    bool quit_mission;
    bool playvqa,
            allowsandbagging,
            debug,
            demo;
    /** Enable/disable the fog of war */
    bool UseFogOfWar;
    bool use_opengl;

    gametypes gamenum;

    SDL_GrabMode grabmode;

    SDLKey bindablekeys[NUMBINDABLE];

    Uint8 bindablemods[NUMBINDABLE];

    Uint8 buildable_radius;

    std::string mapname;
    std::string vqamovie;
    std::string nick;
    std::string side_colour;
    /** side of human player */
    std::string mside;
    /** side of computer player */
    std::string cside;
    std::string serveraddr;
    /** vfs binpath */
    std::string binpath;
    double buildable_ratio;


    // Some sounds

    //
    // Structures
    //
    string StructureDestroyed;
    string StructureHalfDestroyed;
    string StructureSold;
    string StructurePlaced;
    string StructureStartBuild;
    string StructureReady;
    string RepairStructure;
    string PrimairyStructureSelected;
    // Base
    string CmdCentreUnderAttack;
    string BaseUnderAttack;
    // Unit
    string UnitFull;
    string UnitLost;
    string RepairUnit;
    string UnitRepaired;
    string UnitSold;
    string UnitReady;
    string UnitDeployed;
    string TrainUnit;
    string AirUnitLost;
    /** Naval unit lost */
    string NavalUnitLost;
    /** General */
    string BattleControlTerm,
            BuildingCanceled,
            BuildingOnHold,
            Newoptions,
            NoDeploy,
            NoFunds,
            Nopower,
            Reinforcements,
            RadarUp,
            RadarDown,
            MissionWon,
            MissionLost;
    /** Money Up sound */
    string MoneyCountUp;
    /**
     * Money Down Sound
     */
    string MoneyCountDown;
} ConfigType;

/**
 *	this getConfig() is located in args.cpp
 */
const ConfigType& getConfig();

#endif //CONFIG_H
