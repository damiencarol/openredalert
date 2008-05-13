// UnitAndStructureMat.h
// 1.3

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef UNITANDSTRUCTUREMAT_H
#define UNITANDSTRUCTUREMAT_H

class UnitAndStructureMat
{
public:
	Uint32 flags;				// For now don't change flag handling
//	Uint16 unitorstructnumb;	//
	Uint16 unitnumb;
	Uint16 airunitnumb;
	Uint16 structurenumb;
};

#endif //UNITANDSTRUCTURMAT_H
