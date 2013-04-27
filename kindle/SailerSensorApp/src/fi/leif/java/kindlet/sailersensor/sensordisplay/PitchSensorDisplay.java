package fi.leif.java.kindlet.sailersensor.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;

import fi.leif.java.kindlet.sailersensor.Config;


public class PitchSensorDisplay extends SensorDisplay
{
	KLabel pitch;

	private final String MSG_ID = "AP";

	public PitchSensorDisplay(Config config)
	{
		super(config);
		
		setLayout(new BorderLayout());

		// Add header
		addHeader("ACC PITCH");

		// Speed label
		pitch = new KLabel("", KLabel.CENTER );
		pitch.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.DATA_FONT_SIZE));
		add( pitch, BorderLayout.CENTER );
	}

	public void messageRetrieved(Map m) throws Exception
	{
		if(m.get(MSG_ID) != null)
		{
			Double d = (Double)m.get(MSG_ID);
			pitch.setText(d.toString() + "\u00B0");
			pitch.repaint();
		}
	}
}
