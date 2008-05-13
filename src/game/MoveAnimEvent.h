#ifndef MOVEANIMEVENT_H
#define MOVEANIMEVENT_H

#include "UnitAnimEvent.h"
#include "Unit.h"

class Path;

class MoveAnimEvent : public UnitAnimEvent {
public:
    MoveAnimEvent(Uint32 p, Unit * un);
    virtual ~MoveAnimEvent();
    virtual void stop();
    virtual void run();
    virtual void update();
    virtual void setRange(Uint32 nr) ;
private:
	Uint32 DefaultDelay;
	Uint16 BlockedCounter;
    bool stopping;
    void startMoveOne(bool wasblocked);
    void moveDone();
    Uint16 dest;
    Uint16 newpos;
    bool blocked;
    bool moved_half;
    bool pathinvalid;
    bool waiting;
    Sint8 xmod;
    Sint8 ymod;
    Sint8 b_xmod;
    Sint8 b_ymod;
    Unit* un;
    Path* path;
    Uint8 istep;
    Uint8 dir;
    Uint32 range;
};


#endif
