/**
 * 
 */
package openra.core;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.prefs.BackingStoreException;

import org.ini4j.IniFile;

/**
 * @author D.Carol
 *
 */
public class Main {

	/**
	 * 
	 */
	public Main() {
		// TODO Auto-generated constructor stub
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		/*BufferedReader r = new BufferedReader(
				 new InputStreamReader(
				getClass().getResourceAsStream("openra.jar!/data/general/mission.ini") ));
*/
		File theFile = new File("data/main/general/scg03ea.ini");
		try {
			IniFile toto = new IniFile(theFile);
		} catch (BackingStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
