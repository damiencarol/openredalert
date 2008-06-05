#ifndef UNITANIMEVENT_H
#define UNITANIMEVENT_H

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class Unit;

/**
 * Base Class for all Unit anim
 */
class UnitAnimEvent : public ActionEvent {
public:
	UnitAnimEvent(Uint32 p, Unit* un);
	virtual ~UnitAnimEvent();
	void setSchedule(UnitAnimEvent* e);
	void stopScheduled();
	virtual void stop() = 0;
	virtual void update();
	virtual void run() = 0;
private:
	Unit* un;
	UnitAnimEvent* scheduled;
};

#endif //UNITANIMEVENT_H
