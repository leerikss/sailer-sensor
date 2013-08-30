package fi.leif.java.kindlet.sailersensor;

import java.io.IOException;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.SocketException;

import fi.leif.java.kindlet.sailersensor.messagehandler.MessageHandler;

public class DatagramSocketThread extends Thread
{
  private boolean running = false;
  private Config config;
  private DatagramSocket server = null;  
  private MessageHandler msgHandler;

  public DatagramSocketThread(Config config, MessageHandler msgHandler)
  {
    this.config = config;
    this.msgHandler = msgHandler;
  }

  public void run()
  {
    startServer();
  }
	
  private void startServer()
  {
    try
    {
      server = new DatagramSocket(config.SOCKET_PORT);
      msgHandler.sendMessage("1");
      this.running = true;
    }
    // Unable to listen to port. Sleep & try again.
    catch(IOException e1)
    {
      msgHandler.sendMessage("Jes");

      try 
      {
        DatagramSocketThread.sleep(config.SOCKET_SLEEP_ON_ERROR); 
        startServer();
      }
      // Sleep might be interrupted, quit here
      catch(InterruptedException e2) { return; }
    }

    // Start socket loop
    while(this.running)
    {
      msgHandler.sendMessage("3");
      try
      {
	byte[] data = new byte[1024];
	DatagramPacket packet = new DatagramPacket(data,
						 data.length);
	server.receive(packet);

	String msg = new String(packet.getData(),0,0
				 ,packet.getLength());      
	msgHandler.sendMessage(msg);

      }
      // close() was called, end while loop
      catch(SocketException se)
      {
        running = false;
      }
			
      // Any other exception, sleep and continue while loop
      catch(Exception e)
      {
        try 
        { 
          DatagramSocketThread.sleep(config.SOCKET_SLEEP_ON_ERROR); 
        }
        // Sleep might be interrupted, end while loop
        catch(InterruptedException ie) 
        {
          running = false;
        }
      }
    }
  }  

  public void stopServer()
  {
    if(server != null)
    {
      try 
      { 
        running = false;
        server.close();
      }
      catch(Exception e) {}
    }
  }

}