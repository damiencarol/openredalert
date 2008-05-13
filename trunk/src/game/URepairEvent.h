#ifndef UREPAIREVENT_H
#define UREPAIREVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"
#include "Unit.h"

class UnitOrStructure;

class URepairEvent : public UnitAnimEvent {
public:
    URepairEvent(Uint32 p, Unit *un);
    virtual ~URepairEvent();
    void stop();
    virtual void update();
    virtual void run();
private:
	Uint32 fix_str_num;
	Uint16 fix_str_pos;
	Uint8	moveCounter;
	Uint16 dmg_cost; // Total cost of damage remaining

	Uint8	ReturnStep;
	Unit	*un;
	bool	stopping;
	UnitOrStructure * FixStr;
	int index;
    int delay;
	int	facing;
	Uint32 MoveTargePos;
    Uint32 OrgImage;
};

#endif
