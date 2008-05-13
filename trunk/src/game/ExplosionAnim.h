#ifndef EXPLOSIONANIM_H
#define EXPLOSIONANIM_H

#include <map>

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

struct L2Overlay;

class ExplosionAnim : public ActionEvent
{
public:
    ExplosionAnim(Uint32 p, Uint16 pos, Uint32 startimage, Uint8 animsteps,
                  Sint8 xoff, Sint8 yoff);
    ~ExplosionAnim();
    void run();
private:
    ExplosionAnim() ;

    L2Overlay *l2o;
    Uint16 pos;
    Uint8 animsteps;
    std::multimap<Uint16, L2Overlay*>::iterator l2entry;
};


#endif //EXPLOSIONANIM_H
