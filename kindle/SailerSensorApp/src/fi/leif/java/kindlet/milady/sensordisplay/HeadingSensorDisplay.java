package fi.leif.java.kindlet.milady.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;

import fi.leif.java.kindlet.milady.Config;

public class HeadingSensorDisplay extends SensorDisplay
{
	private final String MSG_ID = "HD";
	
	KLabel heading;
	
	public HeadingSensorDisplay(Config config)
	{
		super(config);
		
		setLayout(new BorderLayout());
		
		// Add header
		addHeader("HEADING");

		// Heading label
		heading = new KLabel("", KLabel.CENTER );
		heading.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.DATA_FONT_SIZE));
		add( heading, BorderLayout.CENTER );
	}

	public void messageRetrieved(Map m) throws Exception
	{
		if(m.get(MSG_ID) != null)
		{
			Double d = (Double)m.get(MSG_ID);
			heading.setText(d.intValue() + "\u00B0");
			heading.repaint();
		}
	}
}
