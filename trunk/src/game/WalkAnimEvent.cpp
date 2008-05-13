#include "WalkAnimEvent.h"
#include <cmath>
#include "include/ccmap.h"
#include "include/common.h"
#include "include/Logger.h"
#include "include/ProjectileAnim.h"
#include "game/Unit.h"
#include "include/UnitAndStructurePool.h"
#include "include/weaponspool.h"
#include "include/PlayerPool.h"
#include "UnitOrStructure.h"
#include "game/Unit.h"
#include "game/unittypes.h"
#include "game/TurnAnimEvent.h"
#include "game/UAttackAnimEvent.h"
#include "Path.h"
#include "Unit.h"
WalkAnimEvent::WalkAnimEvent(Uint32 p, Unit *un, Uint8 dir, Uint8 layer) : UnitAnimEvent(p,un)
{
    //fprintf(stderr,"debug: WalkAnim constructor\n");
    this->un = un;
    this->dir = dir;
    this->layer = layer;
    stopping = false;
    istep = 0;
    calcbaseimage();
}
WalkAnimEvent::~WalkAnimEvent()
{
#ifdef LOOPEND_TURN
    un->setImageNum((((UnitType*)un->type)->getAnimInfo().loopend+1)*dir/8,layer);
#else
	un->setImageNum( dir>>2, layer );
#endif
    if (un->walkanim == this)
        un->walkanim = NULL;
}
void WalkAnimEvent::run()
{
    Uint8 layerface;
    if (!stopping) {
        layerface = baseimage + istep;
        // XXX: Assumes 6 frames to loop over
        istep = (istep + 1)%6;
		//printf ("%s line %i: Baseimg = %i, istep = %i\n", __FILE__, __LINE__, baseimage, istep);
		un->setImageNum(layerface,layer);
        p::aequeue->scheduleEvent(this);
    } else {
        delete this;
        return;
    }
}
void WalkAnimEvent::calcbaseimage()
{
    // XXX: this is really nasty, will be taken care of after the rewrite
    baseimage = 16 + 3*(dir/2);
}
void WalkAnimEvent::stop() {stopping = true;}
void WalkAnimEvent::changedir(Uint8 ndir) {
        stopping = false;
        dir = ndir;
        calcbaseimage();
    }
void WalkAnimEvent::update() {}
