// armor_t.h
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

#ifndef ARMOR_T_H
#define ARMOR_T_H

/** Armor type in RedAlert */
enum armor_t {
    /** none, no armor (default) */
    AC_none = 0,

    /** Wood Armor */
    AC_wood = 1,

    /** Light Armor */
    AC_light = 2,

    /** Heavy Armor */
    AC_heavy = 3,

    /** Concrete Armor */
    AC_concrete = 4
};

#endif //ARMOR_T_H
