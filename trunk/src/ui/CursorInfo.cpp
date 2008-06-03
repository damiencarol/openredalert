// CursorInfo.cpp
// 1.0

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

#include "CursorInfo.h"

Uint16 CursorInfo::getAnStart()
{
	return anStart; 
}

void CursorInfo::setAnStart(Uint16 anStart)
{
	this->anStart = anStart; 
}

Uint16 CursorInfo::getAnEnd()
{
	return anEnd; 
}

void CursorInfo::setAnEnd(Uint16 anEnd)
{
	this->anEnd = anEnd; 
}
