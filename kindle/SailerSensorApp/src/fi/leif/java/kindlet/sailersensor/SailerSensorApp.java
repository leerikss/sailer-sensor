package fi.leif.java.kindlet.sailersensor;

import java.awt.CardLayout;
import java.awt.Container;
import java.awt.GridLayout;

import com.amazon.kindle.kindlet.AbstractKindlet;
import com.amazon.kindle.kindlet.KindletContext;
import com.amazon.kindle.kindlet.ui.KPanel;

import fi.leif.java.kindlet.sailersensor.messagehandler.MessageHandler;
import fi.leif.java.kindlet.sailersensor.messagehandler.SensorDisplayMessageHandler;
import fi.leif.java.kindlet.sailersensor.sensordisplay.SensorDisplay;


public class SailerSensorApp extends AbstractKindlet
{
  private Config config;
  private CardLayout layout;
  private SwitchPageThread switchPageThread;
  private SocketServerThread socketThread;
  // private DatagramSocketThread socketThread;
  private MessageHandler msgHandler;
  private Container container;

  public void create(KindletContext context)
  {
    container = context.getRootContainer();

    config = new Config();

    // Construct page switching thread
    switchPageThread = new SwitchPageThread();
    socketThread = new SocketServerThread(config, msgHandler);

    // Init message handler
    msgHandler = new SensorDisplayMessageHandler(config.SENSOR_DISPLAY_PAGES,
						 config.MSG_SEPARATOR, config.MSG_ID_LEN);

    // Build ui
    layout = buildUi();
  }

  private CardLayout buildUi()
  {
    CardLayout cl = new CardLayout();
    container.setLayout(cl);

    // Iterate pages
    for(int p=0; p< config.SENSOR_DISPLAY_PAGES.length; p++)
    {
      SensorDisplay[] disps = config.SENSOR_DISPLAY_PAGES[p];

      // Create a GridLayout panel for current CardLayout page
      KPanel pagePanel = new KPanel();
      int rows = getPageRows(disps.length);
      int cols = getPageCols(disps.length);
      GridLayout gl = new GridLayout( rows, cols );
      pagePanel.setLayout(gl);

      // Iterate page displays
      for(int d = 0; d < disps.length; d++)
      {
	// Add SensorDisplay to GridLayout
	SensorDisplay disp = config.SENSOR_DISPLAY_PAGES[p][d];
	pagePanel.add( disp );
      }

      container.add( pagePanel, "page"+p );
    }

    return cl;
  }

  private int getPageRows(int l)
  {
    return (l <= config.MAX_PAGE_ROWS) ? l : config.MAX_PAGE_ROWS;
  }

  private int getPageCols(int l)
  {
    return (int) Math.ceil( l/config.MAX_PAGE_ROWS );
  }

  public void start()
  {
    // If more than one page, start page switching thread
    if( config.SENSOR_DISPLAY_PAGES.length > 1)
    {
      switchPageThread = new SwitchPageThread();
      switchPageThread.start();
    }

    // Start socket server
    socketThread = new SocketServerThread(config, msgHandler);
    // socketThread = new DatagramSocketThread(config, msgHandler);
    socketThread.start();
  }

  public void stop()
  {
    // Stop SocketServer
    if(switchPageThread != null)
    {
      socketThread.stopServer();
      socketThread.interrupt();
    }

    // Stop SwitchPageThread
    switchPageThread.stopThread();
    switchPageThread.interrupt();
  }

  public void destroy()
  {
    stop();
  }

  private class SwitchPageThread extends Thread
  {
    private boolean running = false;
    private int page = 0;

    public void stopThread()
    {
      running = false;
    }

    public void run()
    {
      running = true;
      while(running)
      {
	try { SwitchPageThread.sleep(config.PAGE_SHOW_MS); }
	catch(Exception e) {}

	page = (page == config.SENSOR_DISPLAY_PAGES.length) ? 0 : page+1;
	layout.show(container, "page"+page);
      }
    }
  }	
}
