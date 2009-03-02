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

import java.io.File;
import java.util.prefs.BackingStoreException;

import org.ini4j.IniFile;

public class ORAIniFile extends IniFile{
	
	public ORAIniFile(String fileName) throws BackingStoreException {
		super(new File(fileName));
	}

/*	public ORAIniFile(File file) throws BackingStoreException {
		super(file);
		// TODO Auto-generated constructor stub
	}*/

	/**
	 * 
	 */
	private static final long serialVersionUID = 145390873810423731L;

}
