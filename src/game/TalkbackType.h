// TalkbackType.h
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

#ifndef TALKBACKTYPE_H
#define TALKBACKTYPE_H

enum TalkbackType
{
    TB_report,
    TB_ack,
    TB_atkun,
    TB_atkst,
    TB_die,
    TB_postkill,
    TB_invalid
};

#endif //TALKBACKTYPE_H
