// WarheadData.h
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

#ifndef WARHEADDATA_H
#define WARHEADDATA_H

#include <string>

#include "SDL/SDL_types.h"

#include "misc/INIFile.h"


using std::string;

/**
 * This is what gives the "rock, paper, scissors" character to the game. It describes how the damage is to be applied to the target. The values should take into consideration the 'area of effect'.; example: Although an armor piercing tank round would instantly kill a soldier IF it hit, the anti-infantry rating is still very low because the tank round has such a limited area of effect, lacks pinpoint accuracy, and acknowledges the fact that tanks pose little threat to infantry that take cover. 
 */
class WarheadData {
public:

    Uint32 getSpread();

    void setSpread(Uint32 spread);

    Uint32 getWall();

    void setWall(Uint32 wall);

    Uint32 getWood();

    void setWood(Uint32 wood);

    Uint32 getOre();

    void setOre(Uint32 ore);

    Uint32 getVersusNone();

    void setVersusNone(Uint32 versusNone);

    Uint32 getVersusWood();

    void setVersusWood(Uint32 versusWood);

    Uint32 getVersusLight();

    void setVersusLight(Uint32 versusLight);

    Uint32 getVersusHeavy();

    void setVersusHeavy(Uint32 versusHeavy);

    Uint32 getVersusConcrete();

    void setVersusConcrete(Uint32 versusConcrete);

    Uint32 getExplosion();

    void setExplosion(Uint32 explosion);

    Uint32 getInfDeath();

    void setInfDeath(Uint32 infDeath);
    
    static WarheadData* loadWarheadData(INIFile * file, string name);
    void print();

private:
    Uint32 spread;
    Uint32 wall;
    Uint32 wood;
    Uint32 ore;
    Uint32 versusNone;
    Uint32 versusWood;
    Uint32 versusLight;
    Uint32 versusHeavy;
    Uint32 versusConcrete;
    Uint32 explosion;
    Uint32 infDeath;
};
#endif //WARHEADDATA_H
