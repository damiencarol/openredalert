// MissionTheater.h
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

#ifndef MISSIONTHEATER_H
#define MISSIONTHEATER_H

#include <string>

using std::string;

/**
 * Theater of a map
 * 
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r340
 */
class MissionTheater
{
public:
	/** Return the name of the theater */
	string getName();
	
	/** Return true if it's interior */
	bool isInterior();

private:
	MissionTheater();
	/** Name of the theater */
	string name;
	bool interior;
	
protected:
	MissionTheater(string name);
};

#endif //MISSIONTHEATER_H
