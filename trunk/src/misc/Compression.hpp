// Compression.hpp
// 2.0

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

#ifndef COMPRESSION_H
#define COMPRESSION_H

/**
 * Various decompression routines for format80, format40, and
 * format20 type graphics
 *
 * @author Olaf van der spek (XCC mixer)
 * @author Kareem Dana (FreeCNC)
 */
class Compression
{
public:
	/** Decompress format 80 compressed data. */
	static int decode80(const unsigned char image_in[], unsigned char image_out[]);
	/** Decompress format 40 compressed data. */
	static int decode40(const unsigned char image_in[], unsigned char image_out[]);
	/** Decompress format 40 compressed data. */
	static int decode20(const unsigned char* s, unsigned char* d, int cb_s);
	/** Decodes base64 data */
	static int dec_base64(const unsigned char* src, unsigned char* target, const unsigned int length);
};

#endif //COMPRESSION_H
