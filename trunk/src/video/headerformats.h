// HeaderFormats.h
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

#ifndef HEADERFORMATS_H
#define HEADERFORMATS_H

/**
 * Compression format of some files. 
 * 
 * The format can have one of the three values : 80h, 40h, 20h.
 * Call 'Format80', 'Format40' and 'Format20'.
 */
enum headerformats {
    FORMAT_20 = 0x20, FORMAT_40 = 0x40, FORMAT_80 = 0x80
};

#endif //HEADERFORMATS_H
