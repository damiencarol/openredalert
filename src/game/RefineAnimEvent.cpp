#include "RefineAnimEvent.h"

#include "SDL/SDL_types.h"

#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "include/ProjectileAnim.h"
#include "audio/SoundEngine.h"
#include "Unit.h"
#include "Player.h"
#include "anim_nfo.h"
#include "Structure.h"

//extern Logger * logger;
namespace p {
	extern PlayerPool* ppool;
}

RefineAnimEvent::RefineAnimEvent(Uint32 p, Structure* str, Uint8 bails) : BuildingAnimEvent(p, str, 7)
{
	updateDamaged();
	
    this->bails = bails;
    this->str = str;
    frame = framestart;
    
    //logger->error ("%s line %i: Start refine animation\n", __FILE__, __LINE__);
}

void RefineAnimEvent::anim_func(anim_nfo* data)
{
    updateDamaged();
    
    if(bails>0) {
        if (frame < framend) {
            ++frame;
        } else {
            frame = framestart;
            --bails;
            // TODO CHANGE IT TO GET FROM STRUCTURE
            p::ppool->getPlayer(str->getOwner())->changeMoney(100);
        }
    } else {
        data->done = true;
    }
    data->frame0 = frame;
}

void RefineAnimEvent::updateDamaged()
{
    BuildingAnimEvent::updateDamaged();
    
    if (anim_data.damaged) {
        if (frame < getaniminfo().dmgoff) {
            frame += getaniminfo().dmgoff;
        }
    }
    
    // TODO fixme: avoid hardcoded values
    framestart = getaniminfo().loopend+1+anim_data.damagedelta;
    framend = framestart + 17; 
}
