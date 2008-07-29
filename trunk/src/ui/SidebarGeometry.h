// SidebarGeometry.h
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

#ifndef SIDEBARGEOMETRY_H
#define SIDEBARGEOMETRY_H

#include "SDL/SDL_types.h"

/**
 * Information about Height and Width to manage drawing of sidebar buttons
 * 
 * @todo complete documentation about this class
 */
class SidebarGeometry {
public:   
	/** Width of sidebar buttons in the sidebar */
	Uint16 bw;
	/** Height of all sidebar buttons in the sidebar */
    Uint16 bh;
};

#endif //SIDEBARGEOMETRY_H
