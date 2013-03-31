package fi.leif.java.kindlet.milady.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;
import com.amazon.kindle.kindlet.ui.KPanel;

import fi.leif.java.kindlet.milady.Config;

public abstract class SensorDisplay extends KPanel
{
	protected Config config;
	
	public SensorDisplay(Config config)
	{
		this.config = config;
	}
	
	protected void addHeader(String title)
	{
		// Add header
		KLabel label = new KLabel("  "+title);
		label.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.TITLE_FONT_SIZE));
		label.setBackground(Color.BLACK);
		label.setForeground(Color.WHITE);
		add( label, BorderLayout.NORTH );
	}

	public abstract void messageRetrieved(Map msg) throws Exception;
}
