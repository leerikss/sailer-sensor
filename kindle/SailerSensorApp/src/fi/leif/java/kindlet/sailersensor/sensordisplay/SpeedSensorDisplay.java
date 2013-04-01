package fi.leif.java.kindlet.sailersensor.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;

import fi.leif.java.kindlet.sailersensor.Config;


public class SpeedSensorDisplay extends SensorDisplay
{
	KLabel speed;

	private final String MSG_ID = "SP";
	private final int KN_FONT_SIZE = 75;

	public SpeedSensorDisplay(Config config)
	{
		super(config);
		
		setLayout(new BorderLayout());

		// Add header
		addHeader("SPEED");

		// Speed label
		speed = new KLabel("", KLabel.CENTER );
		speed.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.DATA_FONT_SIZE));
		add( speed, BorderLayout.CENTER );
		
		// Knots label
		KLabel kn = new KLabel("kn  ");
		kn.setFont(new Font(config.FONTFAMILY, Font.BOLD, KN_FONT_SIZE));
		add( kn, BorderLayout.EAST );
		
	}

	public void messageRetrieved(Map m) throws Exception
	{
		if(m.get(MSG_ID) != null)
		{
			Double d = (Double)m.get(MSG_ID);
			speed.setText(d.toString());
			speed.repaint();
		}
	}
}
