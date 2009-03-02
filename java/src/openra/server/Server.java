package openra.server;

import java.io.File;

import openra.core.CncMap;
import openra.core.GameMode;
import openra.core.LoadMapError;
import sdljava.SDLTimer;

public class Server {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		
	}
	
	
	public void manageSession() throws LoadMapError
	{
		// Load by default the ALLIES 1 MAP		
		Game theGame = new Game();
		CncMap theMap = CncMap.loadMap(new File("data/main/general/SGC01EA.INI"));
		theMap.Init(null, GameMode.GAME_MODE_SINGLE_PLAYER);
		ActionEventQueue aequeue = new ActionEventQueue();
		ActionEvent event1 = new ActionEvent(1);
		
		long startTime = SDLTimer.getTicks();
		boolean finished = false;
		
		while (finished == false)
		{
			
			
			
			
			
			
			
			long currentTime = SDLTimer.getTicks();
			if (currentTime > startTime + 10000)
			{
				finished = true;
			}
		}
	}

}
