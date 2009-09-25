/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
package openra.core;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import org.ini4j.Ini;
import org.ini4j.InvalidIniFormatException;

public class IniFile extends Ini {
	/**
	 * 
	 */
	private static final long serialVersionUID = 145390873810423731L;

	public IniFile(String fileName) throws InvalidIniFormatException, FileNotFoundException, IOException {
		super(new FileReader(fileName));
	}

	public static void putYesNo(Ini mapIniFile, String string, String string2,
			boolean value) {
		if (value) {
			mapIniFile.get(string).put(string2, "yes");
		} else {
			 mapIniFile.get(string).put(string2, "no");
		}
	}

/*	public ORAIniFile(File file) throws BackingStoreException {
		super(file);
		// TODO Auto-generated constructor stub
	}*/

	
}
