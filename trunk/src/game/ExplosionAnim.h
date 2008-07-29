// ExplosionAnim.h
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

#ifndef EXPLOSIONANIM_H
#define EXPLOSIONANIM_H

#include <map>

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class L2Overlay;

using std::multimap;

/**
 * Anim to create an Explosion Animation
 */
class ExplosionAnim : public ActionEvent
{
public:
    ExplosionAnim(Uint32 p, Uint16 pos, Uint32 startimage, Uint8 animsteps,
                  Sint8 xoff, Sint8 yoff);
    ~ExplosionAnim();

    void run();
    
private:
    ExplosionAnim() ; // Declare private constructor to force parameter in constructor

    L2Overlay* l2o;
    /** The position of the animation */
    Uint16 pos;
    Uint8 animsteps;
    /** iterator to the overlay in the multimap */
    multimap<Uint16, L2Overlay*>::iterator l2entry;
};

#endif //EXPLOSIONANIM_H

