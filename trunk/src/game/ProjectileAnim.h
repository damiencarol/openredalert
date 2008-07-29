// ProjectileAnim.h
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

#ifndef PROJECTILEANIM_H
#define PROJECTILEANIM_H

#include <map>

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class UnitAndStructurePool;
class L2Overlay;
class Weapon;
class UnitOrStructure;

using std::multimap;

/**
 * Anim that manage a projectile course
 */
class ProjectileAnim : public ActionEvent
{
public:
    ProjectileAnim(Uint32 p, Weapon* weap, UnitOrStructure* owner, Uint16 dest, Uint8 subdest);
    ~ProjectileAnim();
    
    void run();
    
private:
    Weapon* weap;
    UnitOrStructure* owner;
    UnitOrStructure* target;
    Uint16 dest;
    Uint8 subdest;
    /** Fuel - how many ticks left until projectile is removed.*/
    Uint8 fuel;
    /** Seekfuel - how many ticks left until this projectile change course
     *to track its target before falling back to flying in a straight line.*/
    Uint8 seekfuel;
    Sint8 xoffset;
    Sint8 yoffset;
    //Sint32 xmod, ymod;
    L2Overlay* l2o;
    multimap<Uint16, L2Overlay*>::iterator l2entry;
    double xdiff, ydiff;
    double xmod, ymod, rxoffs, ryoffs;
    bool heatseek,inaccurate,fuelled;
    Uint8 facing;
};

#endif //PROJECTILEANIM_H
