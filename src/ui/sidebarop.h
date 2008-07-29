// sidebarop.h
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

#ifndef SIDEBAROP_H
#define SIDEBAROP_H

enum sidebarop
{
    sbo_null = 0,
    sbo_build = 1,
    sbo_scroll = 2,
    sbo_unit = 4,
    sbo_structure = 8,
    sbo_up = 16,
    sbo_down = 32
};

#endif //SIDEBAROP_H
