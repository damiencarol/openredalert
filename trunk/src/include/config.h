#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#include "SDL/SDL_video.h"
#include "SDL/SDL_types.h"
#include "SDL/SDL_keysym.h"

#include "common.h"
#include "misc/gametypes.h"

namespace {
    const Uint8 NUMBINDABLE = 3;
}

enum KEY_TYPE {
    KEY_SIDEBAR = 0, 
    KEY_STOP, 
    KEY_ALLY
};

typedef struct _ConfigType
{
	Uint32	videoflags,
			startMoney;

	Uint16	width,
			height,
			bpp,
			serverport;

	Uint8		intro;
	/* Game mode */
	Uint8 gamemode;
	Uint8	totalplayers,
			playernum,
			scrollstep,
			scrolltime,
			maxscroll,
			finaldelay,
			dispatch_mode;

	bool nosound;
	bool pause;
	bool quit_mission;
	bool 	playvqa,
			allowsandbagging,
			debug,
			demo;
	/** Enable/disable the fog of war */
	bool UseFogOfWar;
	bool use_opengl;		

	gametypes gamenum;

	SDL_GrabMode	grabmode;

	SDLKey		bindablekeys[NUMBINDABLE];

	Uint8		bindablemods[NUMBINDABLE];

	Uint8		buildable_radius;

	std::string	mapname;
	std::string	vqamovie;
	std::string	nick;
	std::string	side_colour;
	/** side of human player */
	std::string	mside;
	/** side of computer player */
	std::string	cside;
	std::string	serveraddr;
	/** vfs binpath */
	std::string	binpath;
	double buildable_ratio;


	// Some sounds
	char		// Structures
			*StructureDestroyed,
			*StructureHalfDestroyed,
			*StructureSold,
			*StructurePlaced,
			*StructureStartBuild,
			*StructureReady,
			*RepairStructure,
			*PrimairyStructureSelected,
			// Base
			*CmdCentreUnderAttack,
			*BaseUnderAttack,
			// Unit
			*UnitFull,
			*UnitLost,
			*RepairUnit,
			*UnitRepaired,
			*UnitSold,
			*UnitReady,
			*UnitDeployed,
			*TrainUnit,
			*AirUnitLost;
	/** Naval unit lost */
	char *NavalUnitLost;
	/** General */
	char 	*BattleControlTerm,
			*BuildingCanceled,
			*BuildingOnHold,
			*Newoptions,
			*NoDeploy,
			*NoFunds,
			*Nopower,
			*Reinforcements,
			*RadarUp,
			*RadarDown,
			*MissionWon,
			*MissionLost;
	/** Money Up sound */
	char *MoneyCountUp;
	/**
	 * Money Down Sound
	 */
	char *MoneyCountDown;
} ConfigType;

/**
 *	this getConfig() is located in args.cpp
 */
const ConfigType& getConfig();

#endif //CONFIG_H
