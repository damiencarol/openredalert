#include "ExplosionAnim.h"

#include "include/Logger.h"
#include "ActionEventQueue.h"
#include "L2Overlay.h"
#include "include/UnitAndStructurePool.h"

namespace p {
    extern ActionEventQueue * aequeue;
    extern UnitAndStructurePool * uspool;
}

extern Logger * logger;

ExplosionAnim::ExplosionAnim(Uint32 p, Uint16 pos, Uint32 startimage, Uint8 animsteps, Sint8 xoff, Sint8 yoff)
   : ActionEvent(p) {
       l2o = new L2Overlay(1);
       l2o->imagenums[0] = startimage;
       l2o->xoffsets[0] = xoff;
       l2o->yoffsets[0] = yoff;
       l2o->cellpos = pos;
       l2entry = p::uspool->addL2overlay(pos, l2o);
       this->animsteps = animsteps;
       p::aequeue->scheduleEvent(this);
       this->pos = pos;
}

ExplosionAnim::~ExplosionAnim() 
{
    p::uspool->removeL2overlay(l2entry);
    if (l2o != NULL) {
    	delete l2o;
    }
    l2o = NULL;
}

void ExplosionAnim::run() {
    if (animsteps > 0) {
    	animsteps--;
    }
    if (animsteps == 0) {
        delete this;
        return;
    }
    ++l2o->imagenums[0];
    p::aequeue->scheduleEvent(this);
}

/**
 * Build an ExplosionAnim 
 * 
 * with Delay = 1
 */
ExplosionAnim::ExplosionAnim() : ActionEvent(1) 
{
}
