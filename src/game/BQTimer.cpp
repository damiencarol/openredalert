#include "BQTimer.h"
/*
 #include <cassert>
 #include <stdexcept>
 #include <string>

 #include "include/dispatcher.h"
 #include "include/Logger.h"
 #include "include/PlayerPool.h"
 #include "include/UnitAndStructurePool.h"
 #include "audio/SoundEngine.h"
 #include "include/buildqueue.h"
 */
#include "ActionEvent.h"
#include "ActionEventQueue.h"
#include "BQueue.h"

namespace p
{
 	extern ActionEventQueue* aequeue;
}
 /*
 extern Logger * logger;
 */


BQTimer::BQTimer(BQueue* queue, BQTimer** backref) :
	ActionEvent(1), queue(queue), scheduled(false),
			backref(backref)
{
	Destroy = false;
}

BQTimer::~BQTimer()
{
	*backref = 0;
}

void BQTimer::run()
{
	scheduled = false;
	if (Destroy)
	{
		delete this;
		return;
	}

	if (this->queue->tick()==true)
	{
		scheduled = true;
		p::aequeue->scheduleEvent(this);
		return;
	}
}

void BQTimer::Reshedule()
{
	if (!scheduled)
	{
		scheduled = true;
		p::aequeue->scheduleEvent(this);
	}
}

void BQTimer::destroy()
{
	Destroy = true;
	if (!scheduled){
		delete this;
	}
}
