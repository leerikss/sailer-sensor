package fi.leif.java.kindlet.sailersensor;

import fi.leif.java.kindlet.sailersensor.sensordisplay.GpsHeadSensorDisplay;
import fi.leif.java.kindlet.sailersensor.sensordisplay.GpsSpeedSensorDisplay;
import fi.leif.java.kindlet.sailersensor.sensordisplay.MagHeadSensorDisplay;
import fi.leif.java.kindlet.sailersensor.sensordisplay.PitchSensorDisplay;
import fi.leif.java.kindlet.sailersensor.sensordisplay.SensorDisplay;

public class Config 
{
	public final SensorDisplay[][] SENSOR_DISPLAY_PAGES = new SensorDisplay[][]
	{
			// Page 1
			{
				// new CompassSensorDisplay(this),
				new GpsHeadSensorDisplay(this),
				new MagHeadSensorDisplay(this),
				new GpsSpeedSensorDisplay(this),
				new PitchSensorDisplay(this)
			}/*,
			
			// Page 2
			{
				new HeadingSensorDisplay(),
				new HeadingSensorDisplay(),
				new HeadingSensorDisplay()
			}*/
			
	};

	public final int MAX_PAGE_ROWS = 2;
	public final int PAGE_SHOW_MS = 3000;

	public final int TITLE_FONT_SIZE = 40;
	public final int DATA_FONT_SIZE = 90;
	public final String FONTFAMILY = null;
	
	public final int SOCKET_PORT = 9000;
	public final int SOCKET_SLEEP_ON_ERROR = 5000;
	
	public final String MSG_SEPARATOR = " ";
	public final int MSG_ID_LEN = 2;
	
	public Config() {}
}
