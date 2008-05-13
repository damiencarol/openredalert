#ifndef UHARVESTEVENT_H
#define UHARVESTEVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"
#include "Unit.h"

#define HARVEST_RETURN_STEP_1	1
#define HARVEST_RETURN_STEP_2	2
#define HARVEST_RETURN_STEP_3	3
#define HARVEST_RETURN_STEP_4	4

class UnitOrStructure;

class UHarvestEvent : public UnitAnimEvent {
public:
    UHarvestEvent(Uint32 p, Unit * un);
    virtual ~UHarvestEvent();
    void stop();
    void setHarvestingPos(Uint32 pos);
    virtual void update();
    virtual void run();
private:
	Uint16	RetryMoveCounter;
	bool	ForceEmpty;		// this is used to empty the truck when it is not full jet.
	Uint8	ReturnStep;
	bool	new_orgimage;
	Unit	*un;
	bool stopping;
    bool manual_pauze;
	UnitOrStructure * target;
	int index;
    int delay;
	int	facing;
	Uint32 MoveTargePos;
    Uint32 OrgImage;
	Uint8	NumbResources;		// Number of pieces resource in harvester
	Uint8	ResourceTypes[10];	// Resource type in harvester

	int GetBaseRefineryPos (void);
};

#endif
