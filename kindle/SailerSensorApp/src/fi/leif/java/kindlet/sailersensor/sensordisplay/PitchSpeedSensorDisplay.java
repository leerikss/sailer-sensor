package fi.leif.java.kindlet.sailersensor.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;

import fi.leif.java.kindlet.sailersensor.Config;


public class PitchSpeedSensorDisplay extends SensorDisplay
{
	KLabel pitch;
	KLabel speed;

	private final String MSG_PITCH_ID = "AP";
	private final String MSG_GPS_HEAD_ID = "GH";

	public PitchSpeedSensorDisplay(Config config)
	{
		super(config);
		
		setLayout(new BorderLayout());

		// Add header
		addHeader("PITCH & SPEED");

		// Pitch label
		pitch = new KLabel("", KLabel.CENTER );
		pitch.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.DATA_FONT_SIZE));
		add( pitch, BorderLayout.WEST );
		
		// Speed label
		speed = new KLabel("", KLabel.CENTER );
		speed.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.DATA_FONT_SIZE));
		add( speed, BorderLayout.EAST );
	}

	public void messageRetrieved(Map m) throws Exception
	{
		if(m.get(MSG_PITCH_ID) != null)
		{
			Double d = (Double)m.get(MSG_PITCH_ID);
			pitch.setText(d.toString() );
			pitch.repaint();
		}
		if(m.get(MSG_GPS_HEAD_ID) != null)
		{
			Double d = (Double)m.get(MSG_GPS_HEAD_ID);
			speed.setText(d.toString() + "\u00B0");
			speed.repaint();
		}
	}
}
