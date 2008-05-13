#include "BTurnAnimEvent.h"
#include <cmath>
#include "include/ccmap.h"
#include "include/common.h"
#include "include/PlayerPool.h"
#include "include/ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "game/Unit.h"
#include "include/UnitAndStructurePool.h"
#include "include/weaponspool.h"

#include "anim_nfo.h"
#include "Structure.h"

BTurnAnimEvent::BTurnAnimEvent(Uint32 p, Structure* str, Uint8 face) : BuildingAnimEvent(p,str,6)
{
    Uint8 layerface;
    updateDamaged();
    targetface = face;

    // layerface = (str->getImageNums()[0]&0x1f);
    layerface = str->getRealImageNum(0);
    if (layerface == face) {
        delete this;
        return;
    }
    if( ((layerface-face)&0x1f) < ((face-layerface)&0x1f) ) {
        turnmod = -1;
    } else {
        turnmod = 1;
    }
    this->str = str;
}

void BTurnAnimEvent::anim_func(anim_nfo* data)
{

    Uint8 layerface;
    layerface = (str->getImageNums()[0]&0x1f);
    if( abs((layerface-targetface)&0x1f) > abs(turnmod) ) {
        layerface += turnmod;
        layerface &= 0x1f;
    } else {
        layerface = targetface;
    }
    data->frame0 = layerface+data->damagedelta;
    if( layerface == targetface) {
        data->done = true;
    }
}
