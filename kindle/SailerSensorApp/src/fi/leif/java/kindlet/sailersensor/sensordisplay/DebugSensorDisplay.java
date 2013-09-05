package fi.leif.java.kindlet.sailersensor.sensordisplay;

import java.awt.BorderLayout;
import java.util.Map;

import com.amazon.kindle.kindlet.ui.KTextArea;

import fi.leif.java.kindlet.sailersensor.Config;

public class DebugSensorDisplay extends SensorDisplay
{
  KTextArea ta;
  
  public DebugSensorDisplay(Config config)
  {
    super(config);
    setLayout(new BorderLayout());
    addHeader("   DEBUG");
    ta = new KTextArea();
    add(ta, BorderLayout.CENTER);
  }

  public void messageRetrieved(Map m) throws Exception
  {
    ta.setText( getRawMessage() );
    ta.repaint();
  }
}