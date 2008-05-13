#ifndef TURNANIMEVENT_H
#define TURNANIMEVENT_H

#include "UnitAnimEvent.h"
#include "Unit.h"

class TurnAnimEvent : public UnitAnimEvent {
public:
    TurnAnimEvent(Uint32 p, Unit *un, Uint8 dir, Uint8 layer);
    virtual ~TurnAnimEvent();
    virtual void run();
    virtual void stop() ;
    void update() ;
    virtual void changedir(Uint8 ndir) ;
private:
    bool stopping;
    bool runonce;
    Sint8 turnmod;
    Unit * un;
    Uint8 dir;
    Uint8 layer;
};

#endif //TURNANIMEVENT_H
