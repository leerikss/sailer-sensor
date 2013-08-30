package fi.leif.java.kindlet.sailersensor.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Font;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KLabel;

import fi.leif.java.kindlet.sailersensor.Config;


public class MagHeadSensorDisplay extends SensorDisplay
{
  KLabel head;

  private final String MSG_ID = "MH";

  public MagHeadSensorDisplay(Config config)
  {
    super(config);
		
    setLayout(new BorderLayout());

    // Add header
    addHeader("ACC HEAD");

    // Speed label
    head = new KLabel("", KLabel.CENTER );
    head.setFont(new Font(config.FONTFAMILY, Font.BOLD, config.DATA_FONT_SIZE));
    add( head, BorderLayout.CENTER );
  }

  public void messageRetrieved(Map m) throws Exception
  {
    if(m.get(MSG_ID) != null)
    {
      Double d = (Double)m.get(MSG_ID);
      head.setText(d.toString() + "\u00B0");
      head.repaint();
    }
  }
}
