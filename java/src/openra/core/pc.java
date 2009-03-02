package openra.core;

import java.util.Vector;

public class pc {

	public static ConfigType Config = new ConfigType();
	public static Renderer renderer;
	/** SoundEngine of the game */
	public static SoundEngine sfxeng;
	public static GraphicsEngine gfxeng;
	public static MessagePool msg;
	public static Vector<SHPImage> imagepool;
	public static ImageCache imgcache = new ImageCache();
	public static Sidebar sidebar;
	public static Cursor cursor;
	public static Input input;
	// MissionMapsClass *MissionsMapdata = 0;
	public static boolean quit = false;

	public static Ai ai;
}
