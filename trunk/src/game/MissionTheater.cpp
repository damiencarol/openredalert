// MissionTheater.cpp
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

#include "MissionTheater.h"

#include <string>

using std::string;

/** 
 * @return the name of the theater 
 */
string MissionTheater::getName()
{
	// return the name of the theater
	return name;
}

/**  */
MissionTheater::MissionTheater()
{
}

/**
 * @param aName Name of the MissionTheater
 */
MissionTheater::MissionTheater(string aName)
{
	// Set the name
	this->name = aName;

	// Check if it's interior
	if (this->name == "INTERIOR")
	{
		this->interior = true;
	}
}

/** 
 * @return <code>true</code> if it's the 'INTERIOR' theater 
 */
bool MissionTheater::isInterior()
{
	return this->interior;
}
