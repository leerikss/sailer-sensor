package fi.leif.java.kindlet.sailersensor.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;
import com.amazon.kindle.kindlet.ui.KPanel;

import fi.leif.java.kindlet.sailersensor.Config;

public abstract class SensorDisplay extends KPanel
{
	protected Config config;
	
	public SensorDisplay(Config config)
	{
		this.config = config;
	}

	protected void addHeader(String title)
	{
		addHeader(title, this);
	}
	
	protected void addHeader(String title, KPanel parent)
	{
		// Add header
		KLabel label = new KLabel(title);
		label.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.TITLE_FONT_SIZE));
		label.setBackground(Color.BLACK);
		label.setForeground(Color.WHITE);
		parent.add( label, BorderLayout.NORTH );
	}

        protected String getValue(Double d)
        {
	  return getValue(d,"","");
	}

         protected String getValue(Double d, String pre)
        {
	  return getValue(d,pre,"");
	}

        protected String getValue(Double d, String pre, String post)
        {
	  if(d == null || d.intValue() == -1)
	    return "---";
	  else 
	    return new StringBuffer(pre).append( d.intValue() )
	      .append(post).toString();
		 
        }

	public abstract void messageRetrieved(Map msg) throws Exception;
}
