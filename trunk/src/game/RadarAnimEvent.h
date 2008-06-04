#ifndef RADARANIMEVENT_H
#define RADARANIMEVENT_H

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class SHPImage;
class Sidebar;

/**
 * 
 */
class RadarAnimEvent : public ActionEvent 
{
public:
	/** 
	 * if mode = 0 => RADAR ON 
	 * if mode = 1 => RADAR OFF
	 */
    RadarAnimEvent(Uint8 mode, Sidebar* sidebar);
    void run();
    
private:
    Uint8 mode;
    Uint8 frame;
    Uint8 framend;

    Sidebar* sidebar;
    
    SHPImage* animImagesBad;
    SHPImage* logoRadarBad;
    
    SHPImage* animImagesGood;
    SHPImage* logoRadarGood;       
};

#endif //RADARANIMEVENT_H
