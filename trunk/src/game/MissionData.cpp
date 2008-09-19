// MissionData.cpp
// 1.2

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

#include "MissionData.h"

/**
 */
MissionData::MissionData() 
{
    theater = 0;
    brief = 0;
    action = 0;
    player = 0;
    theme = 0;
    winmov = 0;
    losemov = 0;
}

/**
 */
MissionData::~MissionData() 
{
    delete[] theater;
    delete[] brief;
    delete[] action;
    delete[] player;
    delete[] theme;
    delete[] winmov;
    delete[] losemov;
}
