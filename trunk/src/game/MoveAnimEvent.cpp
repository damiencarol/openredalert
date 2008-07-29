// MoveAnimEvent.cpp
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

#include "MoveAnimEvent.h"

#include <cmath>

#include "include/Logger.h"
#include "UnitOrStructure.h"
#include "unittypes.h"
#include "TurnAnimEvent.h"
#include "UAttackAnimEvent.h"
#include "Path.h"
#include "WalkAnimEvent.h"
#include "ActionEventQueue.h"
#include "UnitAndStructurePool.h"
#include "Unit.h"
#include "InfantryGroup.h"

namespace p {
	extern ActionEventQueue * aequeue;
	extern UnitAndStructurePool* uspool;
}
extern Logger * logger;

MoveAnimEvent::MoveAnimEvent(Uint32 p, Unit* un) : UnitAnimEvent(p,un)
{
    //logger->debug("MoveAnim cons (this %p un %p)\n",this,un);
    stopping = false;
    blocked = false;
    range = 0;
    this->dest = un->getTargetCell();
    this->un = un;
    path = NULL;
    newpos = 0xffff;
    istep = 0;
    moved_half = true;
    waiting = false;
    pathinvalid = true;
}

MoveAnimEvent::~MoveAnimEvent()
{
    if (un->moveanim == this)
        un->moveanim = NULL;
    //logger->debug("MoveAnim dest (this %p un %p dest %u)\n",this,un,dest);
    if( !moved_half && newpos != 0xffff) {
        p::uspool->abortMove(un,newpos);
    }
    delete path;
    if (un->walkanim != NULL) {
        un->walkanim->stop();
    }
    if (un->turnanim1 != NULL) {
        un->turnanim1->stop();
    }
    if (un->turnanim2 != NULL) {
        un->turnanim2->stop();
    }
}

void MoveAnimEvent::stop()
{
    stopping = true;
    //stopScheduled();
}

void MoveAnimEvent::run()
{
    Sint8 uxoff, uyoff;
    Uint8 oldsubpos;
	Uint8 NewSubpos;

    waiting = false;
    if( !un->isAlive() ) {
        delete this;
        return;
    }

    if (path == 0) {
        p::uspool->setCostCalcOwnerAndType(un->owner, 0);
        path = new Path(un, un->getPos(), dest, range);
        if( !path->empty() ) {
            startMoveOne(false);
        } else {
            if (un->attackanim != NULL) {
                un->attackanim->stop();
            }
	    //printf ("Empty path --> stop anim??\n");
            delete this;
        }
        return;
    }

    if( blocked ) {
	//printf ("We are blocked\n");
        blocked = false;
        startMoveOne(true);
        return;
    }
    /* if distance left is smaller than xmod we're ready */

    un->xoffset += xmod;
    un->yoffset += ymod;

    if( !moved_half && (abs(un->xoffset) >= 12 || abs(un->yoffset) >= 12) ) {
        oldsubpos = un->subpos;
        NewSubpos = p::uspool->postMove(un, newpos);
		if (NewSubpos > 5){
			logger->error ("%s line %i: ****************** Move failed ****************", __FILE__, __LINE__);
			p::aequeue->scheduleEvent(this);
			return;
		}
		un->subpos = NewSubpos;
        un->cellpos = newpos;
        un->xoffset = -un->xoffset;
        un->yoffset = -un->yoffset;
        if( un->type->isInfantry() ) {
            un->infgrp->GetSubposOffsets(oldsubpos, un->subpos, &uxoff, &uyoff);
            un->xoffset += uxoff;
            un->yoffset += uyoff;
            xmod = 0;
            ymod = 0;
            if( un->xoffset < 0 )
                xmod = 1;
            else if( un->xoffset > 0 )
                xmod = -1;
            if( un->yoffset < 0 )
                ymod = 1;
            else if( un->yoffset > 0 )
                ymod = -1;
        }
        moved_half = true;
    }

    if( abs(un->xoffset) < abs(xmod) )
        xmod = 0;
    if( abs(un->yoffset) < abs(ymod) )
        ymod = 0;

    if( xmod == 0 && ymod == 0 ) {
        un->xoffset = 0;
        un->yoffset = 0;
        moveDone();
	//printf ("Move done, dest = %u, ourpos = %u\n", this->dest, un->getPos());
        return;
    }

    p::aequeue->scheduleEvent(this);
}
void MoveAnimEvent::update()
{
    //logger->debug("Move updating\n");
    dest = un->getTargetCell();
    pathinvalid = true;
    stopping = false;
    range = 0;
}

void MoveAnimEvent::startMoveOne(bool wasblocked)
{
    Unit *BlockingUnit = NULL;
    Uint8 face;
#ifdef LOOPEND_TURN
    //@todo: transport boat is jerky (?)
    Uint8 loopend=((UnitType*)un->type)->getAnimInfo().loopend;
#endif
    newpos = p::uspool->preMove(un, path->top(), &xmod, &ymod, &BlockingUnit);

#if 0
	if ( un->getOwner() == p::ppool->getLPlayerNum() ){
		Uint16 x, y;
		p::ccmap->translateFromPos(newpos, &x, &y);
		printf ("%s line %i: Newpos = %i [%i:%i]\n", __FILE__, __LINE__, newpos, x, y);
	}
#endif
    if( newpos == 0xffff ) {
        delete path;
        path = NULL;
        p::uspool->setCostCalcOwnerAndType(un->owner, 0);
        path = new Path(un, un->getPos(), dest, range);
        pathinvalid = false;
        if( path->empty() ) {
            logger->note ("%s line %i: path is empty\n", __FILE__, __LINE__);
            xmod = 0;
            ymod = 0;
            p::aequeue->scheduleEvent(this);
            return;
        }
        newpos = p::uspool->preMove(un, path->top(), &xmod, &ymod, &BlockingUnit);
        if( newpos == 0xffff ) {
            if (wasblocked) {
                xmod = 0;
                ymod = 0;
                if (un->attackanim != NULL) {
                    un->attackanim->stop();
                }
                this->stop();
                p::aequeue->scheduleEvent(this);
                return;
            } else {
                // @todo: tell the blocking unit to move here
                // logger->note ("%s line %i: We are blocked\n", __FILE__, __LINE__);

                blocked = true;

                if (un->walkanim != NULL) {
                    un->walkanim->stop();
                }

                p::aequeue->scheduleEvent(this);
                return;
            }
        }
    }

#ifdef LOOPEND_TURN
    face = ((Sint8)((loopend+1)*(8-path->top())/8))&loopend;
#else
    face = (32-(path->top() << 2))&0x1f;

//	if (un->getOwner() == p::ppool->getLPlayerNum())
//		printf ("%s line %i: TurnAnimEvent face(dir) = %i, path->top = %i, (path->top() << 2) = %i\n", __FILE__, __LINE__, face, path->top(), (path->top() << 2));
#endif
    path->pop();

	//printf ("%s line %i: Face = %u\n", __FILE__, __LINE__, face);

    moved_half = false;

    if (((UnitType *)un->getType())->isInfantry()) {
        if (un->walkanim != NULL) {
            un->walkanim->changedir(face);
        } else {
            un->walkanim = new WalkAnimEvent(((UnitType *)un->getType())->getSpeed(), un, face, 0);
            p::aequeue->scheduleEvent(un->walkanim);
        }
        p::aequeue->scheduleEvent(this);

    } else {
#ifdef LOOPEND_TURN
        Uint8 curface = (un->getImageNum(0)&loopend);
        Uint8 delta = (abs(curface-face))&loopend;
//        if( curface != face ) {
//            if( (delta <= (Sint8)((loopend+1)/8)) || (delta >= (Sint8)(loopend*7/8))) {
		// Don't try to turn if we are already turning
        if( curface != face && un->turnanim1 == NULL) {
            if( ((delta <= (Sint8)((loopend+1)/8)) || (delta >= (Sint8)(loopend*7/8))) || un->getType()->getType() == UN_PLANE) {
#else
        Uint8 curface = (un->getImageNum(0)&0x1f);
        Uint8 delta = (abs(curface-face))&0x1f;
        if( (un->getImageNum(0)&0x1f) != face ) {
            if( (delta < 5) || (delta > 27) ) {
#endif
				//printf ("%s line %i: Turn 1\n", __FILE__, __LINE__);
                un->turn(face,0);
                p::aequeue->scheduleEvent(this);
            } else {
				//printf ("%s line %i: Turn 2\n", __FILE__, __LINE__);
                waiting = true;
                un->turn(face,0);
                un->turnanim1->setSchedule(this);
            }
            if (un->getType()->getNumLayers() > 1) {
                un->turn(face,1);
            }
        } else {
            p::aequeue->scheduleEvent(this);
        }
    }
}

void MoveAnimEvent::moveDone()
{
    un->xoffset = 0;
    un->yoffset = 0;

    if (pathinvalid) {
        delete path;
        p::uspool->setCostCalcOwnerAndType(un->owner, 0);
        path = new Path(un, un->getPos(), dest, range);
        pathinvalid = false;
    }
    if( !path->empty() && !stopping ) {
        startMoveOne(false);
    } else {
        if( dest != un->getPos() && !stopping ) {
            delete path;
            p::uspool->setCostCalcOwnerAndType(un->owner, 0);
            path = new Path(un, un->getPos(), dest, range);
            pathinvalid = false;
        }
        if( path->empty() || stopping ) {
            delete this;
        } else {
            startMoveOne(false);
        }
    }
}

void MoveAnimEvent::setRange(Uint32 nr)
{
	range = nr;
}
