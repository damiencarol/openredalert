#include "LoopAnimEvent.h"

#include "BuildingAnimEvent.h"
#include "anim_nfo.h"
#include "Structure.h"

LoopAnimEvent::LoopAnimEvent(Uint32 p, Structure* str) : BuildingAnimEvent(p, str, 1)
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
