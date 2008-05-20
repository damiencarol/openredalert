#ifndef MISSIONDATA_H
#define MISSIONDATA_H

#include "SDL/SDL_types.h"

class MissionData {
public:
	~MissionData();
	MissionData();
	
	/** the name of the map */
	char* mapname;
	/** movie played after failed mission */
	char* losemov;
	/** movie played after completed mission */
	char* winmov;
	/** Specific music to play for this mission.*/
	char* theme;
	/** the players side (goodguy, badguy etc.) */
	char* player;
	/** the actionmovie */
	char* action;
	/** the briefing movie */
	char* brief;
	/** the theater of the map (eg. winter) */
	char* theater;
	/** True if it's the last mission */
	bool endOfGame; 
};

#endif //MISSIONDATA_H
