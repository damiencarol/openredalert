#include "ProcAnimEvent.h"

/*
#include <cmath>

#include "include/ccmap.h"
#include "include/PlayerPool.h"
#include "include/ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "game/Unit.h"
#include "include/UnitAndStructurePool.h"
#include "include/weaponspool.h"
*/
#include "anim_nfo.h"

#include "Structure.h"

ProcAnimEvent::ProcAnimEvent(Uint32 delay, Structure* str) : BuildingAnimEvent(delay, str, 4)
{
    updateDamaged();
    framend = getaniminfo().loopend;
    frame = 0;
}

void ProcAnimEvent::anim_func(anim_nfo* data)
{
    updateDamaged();
    data->frame0 = frame;
    ++frame;
    if ((frame-data->damagedelta) > framend) {
        frame = data->damagedelta;
    }
}

void ProcAnimEvent::updateDamaged()
{
    BuildingAnimEvent::updateDamaged();
    
    if (anim_data.damaged) {
        anim_data.damagedelta = 30; // fixme: remove magic numbers
        if (frame < 30) {
            frame += 30;
        }
    }
}
