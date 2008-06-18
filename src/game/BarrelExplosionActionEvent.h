#ifndef BARRELEXPLOSIONANIMEVENT_H
#define BARRELEXPLOSIONANIMEVENT_H

#include "ActionEvent.h"

class BarrelExplosionActionEvent : public ActionEvent
{
public:
	/**
	 * @param p the priority of this event
	 * @param pos The position of the explosion
	 */
	BarrelExplosionActionEvent(Uint32 p, Uint32 pos);
	virtual ~BarrelExplosionActionEvent();
	
    /** */
    virtual void run();
private:
	Uint32 position;
};

#endif //BARRELEXPLOSIONANIMEVENT_H
