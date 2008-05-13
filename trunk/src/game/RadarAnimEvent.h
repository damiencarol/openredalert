#ifndef RADARANIMEVENT_H
#define RADARANIMEVENT_H

#include <vector>

#include "SDL/SDL_video.h"
#include "ActionEvent.h"

class RadarAnimEvent : public ActionEvent {
public:
    RadarAnimEvent(Uint8 mode, bool * minienable, Uint32 radar);
    void run();
private:
    Uint8 mode;
    Uint8 frame;
    Uint8 framend;
    bool * minienable;
    Uint32 radar;
    SDL_Surface * sdlRadar;
};


#endif
