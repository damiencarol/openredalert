// Warhead.h
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

#ifndef WARHEAD_H
#define WARHEAD_H

#include <string>

#include "SDL/SDL_types.h"

#include "armour_t.h"
#include "WeaponData.h"

class WarheadDataList;
class WarheadData;
class SHPImage;

using std::string;

/** 
 * Warhead Characteristics
 * 
 * This is what gives the "rock, paper, scissors" character to the game.
 * It describes how the damage is to be applied to the target. The values should take into consideration the 'area of effect'. example: Although an armor piercing tank round would instantly kill a soldier IF it hit, the anti-infantry rating is still very low because the tank round has such a limited area of effect, lacks pinpoint accuracy, and acknowledges the fact that tanks pose little threat to infantry that take cover.
 */
class Warhead
{
public:
	Warhead(string whname, WarheadDataList* data);
	~Warhead();

	bool getWall();
	Uint8 getVersus(armor_t armour);
	WarheadData* getType();

	/** Return a ref */
	SHPImage* getExplosionImages();
	
private:
	/** Images of the explosion animation */
	SHPImage* explosionImages;

	/** @link aggregation */
	WarheadData* lnkWarheadData;
};

#endif //WARHEAD_H
