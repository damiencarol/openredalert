#ifndef UATTACKANIMEVENT_H
#define UATTACKANIMEVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"

class Unit;
class UnitOrStructure;

/**
 * 
 */
class UAttackAnimEvent : public UnitAnimEvent
{
public:
	UAttackAnimEvent(Uint32 p, Unit *un);
	virtual ~UAttackAnimEvent();
	void stop();
	virtual void update();
	virtual void run();
private:
	Unit* un;
	bool stopping;
	Uint8 waiting;
	UnitOrStructure* target;
	bool UsePrimaryWeapon;
};

#endif //UATTACKANIMEVENT_H
