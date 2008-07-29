// AiRules.h
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

#ifndef AIRULES_H
#define AIRULES_H

#include <vector>

#include "SDL/SDL_types.h"

/**
 * Params of the Ai
 */
class AiRules 
{
public:
    /** Average delay between computer attacks. (defaut = 3) */
	Uint8 AttackInterval;
	/** Average delay time before computer begins first attack (defaut = 5) */
	Uint8 AttackDelay;
	/** Minute interval between scanning for enemys while patrolling. (defaut = 0.016) */
	float PatrolScan;
	/** Structure repair will not begin if available cash falls below this amount. (defaut = 100) */
	Uint8 CreditReserve;
	float PathDelay;	//=.01		; Delay (minutes) between retrying when path is blocked.
	Uint8 OreNearScan;	//=6		; cell radius to scan when harvesting a single patch of ore
	Uint8 OreFarScan;	//=48		; cells radius to scan when looking for a new ore patch to harvest
	Uint8 AutocreateTime;	//=5		; average minutes between creating an 'autocreate' team
	Uint32 InfantryReserve;	//=3000		; always build infantry if cash reserve is greater than this
	Uint8 InfantryBaseMult;	//=1		; build infantry if building count times this number is less than current infantry quantity (should be more?)
	Uint8 PowerSurplus;	//=50		; build power plants until power surplus is at least this amount
	Uint8 BaseSizeAdd;	//=3		; computer base size can be no larger than the largest human opponent, plus this quantity
	float RefineryRatio;	//=.16		; ratio of base that should be composed of refineries
	Uint8 RefineryLimit;	//=4		; never build more than this many refineries
	float BarracksRatio;	//=.16		; ratio of base that should be composed of barracks
	Uint8 BarracksLimit;	//=2		; never build more than this many barracks
	float WarRatio;		//=.1		; ratio of base that should be composed of war factories
	Uint8 WarLimit;		//=2		; never build more than this many war factories
	float DefenseRatio;	//=.4		; ratio of base that should be defensive structures
	Uint8 DefenseLimit;	//=40		; maximum number of defensive buildings to build
	float AARatio;		//=.14		; ratio of base that should be anti-aircraft defense
	Uint8 AALimit;		//=10		; maximum number of anti-aircraft buildings to build
	float TeslaRatio;	//=.16		; ratio of base that should be telsa coils
	Uint8 TeslaLimit;	//=10		; maximum number of tesla coils to build
	float HelipadRatio;	//=.12		; ratio of base that should be composed of helipads
	Uint8 HelipadLimit;	//=5		; maximum number of helipads to build
	/** Ratio of base that should be composed of airstrips (defaut = 0.12) */
	float AirstripRatio;
	Uint8 AirstripLimit;	//=5		; maximum number of airstrips to build
	bool CompEasyBonus;	//=yes		; When more than one human in game, computer player goes to "easy" mode?
	bool Paranoid;		//=yes		; Will computer players ally with each other if the situation looks bleak?
	Uint8 PowerEmergency;	//=75%		; sell buildings to raise power level if it falls below this percentage
	bool AlwaysAlly;	//		; new setting, force ally with other ai players

};

#endif //AIRULES_H
