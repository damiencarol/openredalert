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
 * 
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
