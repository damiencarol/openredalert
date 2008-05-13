/* Generated by Together */

#include "LoopAnimEvent.h"
#include <cmath>
#include "include/ccmap.h"
#include "include/PlayerPool.h"
#include "include/ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "game/Unit.h"
#include "include/UnitAndStructurePool.h"
#include "include/weaponspool.h"
#include "anim_nfo.h"
#include "Structure.h"
LoopAnimEvent::LoopAnimEvent(Uint32 p, Structure* str) : BuildingAnimEvent(p,str,1)
{
    updateDamaged();
    framend = getaniminfo().loopend;
    frame = 0;
}
void LoopAnimEvent::anim_func(anim_nfo* data)
{
    updateDamaged();
    data->frame0 = frame;
    if ((frame-data->damagedelta) < framend) {
        ++frame;
    } else {
        frame = data->damagedelta;
    }
}