#ifndef WALKANIMEVENT_H
#define WALKANIMEVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"
#include "Unit.h"

class WalkAnimEvent : public UnitAnimEvent {
public:
    WalkAnimEvent(Uint32 p, Unit * un, Uint8 dir, Uint8 layer);
    virtual ~WalkAnimEvent();
    virtual void stop();
    virtual void run();
    virtual void changedir(Uint8 ndir);
    void update();
private:
    bool stopping;
    void calcbaseimage(void);
    Unit * un;
    Uint8 dir;
    Uint8 istep;
    Uint8 layer;
    Uint8 baseimage;
};


#endif
