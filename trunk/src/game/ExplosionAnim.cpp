// ExplosionAnim.cpp
// 1.0

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

#include "ExplosionAnim.h"

#include "include/Logger.h"
#include "ActionEventQueue.h"
#include "L2Overlay.h"
#include "UnitAndStructurePool.h"

namespace p {
	extern ActionEventQueue * aequeue;
	extern UnitAndStructurePool * uspool;
}
extern Logger * logger;

/**
 * 
 */
ExplosionAnim::ExplosionAnim(Uint32 p, Uint16 pos, Uint32 startimage,
		Uint8 animsteps, Sint8 xoff, Sint8 yoff) :
	ActionEvent(p) 
{
	l2o = 0;

	l2o = new L2Overlay(1);
	l2o->imagenums[0] = startimage;
	l2o->xoffsets[0] = xoff;
	l2o->yoffsets[0] = yoff;
	l2o->cellpos = pos;
	l2entry = p::uspool->addL2overlay(pos, l2o);
	this->animsteps = animsteps;
	this->pos = pos;
	
	// Reschedule this anim
	p::aequeue->scheduleEvent(this);	
}

/**
 * 
 */
ExplosionAnim::~ExplosionAnim() 
{
	// Remove the overlay
	p::uspool->removeL2overlay(l2entry);
	
	if (l2o != 0) {
		delete l2o;
	}
	l2o = 0;
}

/**
 * 
 */
void ExplosionAnim::run() 
{
	// decrement the step of the anim
	if (animsteps > 0) {
		animsteps--;
	}
	if (animsteps == 0) {
		delete this;
		return;
	}
	++l2o->imagenums[0];

	// re-Schedule this event (to continue the animation)
	p::aequeue->scheduleEvent(this);
}

/**
 * Build an ExplosionAnim 
 * 
 * with Delay = 1
 */
ExplosionAnim::ExplosionAnim() :
	ActionEvent(1) {
}
