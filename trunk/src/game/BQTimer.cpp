// BTimer.cpp
// 1.2

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#include "BQTimer.h"

#include "ActionEvent.h"
#include "ActionEventQueue.h"
#include "BQueue.h"

namespace p
{
 	extern ActionEventQueue* aequeue;
}

/**
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
