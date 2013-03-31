package fi.leif.java.kindlet.milady.messagehandler;

import java.util.HashMap;
import java.util.Map;

import fi.leif.java.kindlet.milady.sensordisplay.SensorDisplay;

public class SensorDisplayMessageHandler implements MessageHandler 
{
	private SensorDisplay[][] sensorDisplayPages;
	private String msgSeparator;
	private int msgIdLen;
	
	public SensorDisplayMessageHandler(SensorDisplay[][] sensorDisplayPages,
			String msgSeparator, int msgIdLen) 
	{
		this.sensorDisplayPages = sensorDisplayPages;
		this.msgSeparator = msgSeparator;
		this.msgIdLen = msgIdLen;
	}

	public void sendMessage(String msg) 
	{
		Map m = parseMessage(msg);
		for(int p=0; p<sensorDisplayPages.length; p++)
		{
			SensorDisplay[] disps = sensorDisplayPages[p];
			for(int d=0; d<disps.length; d++)
			{
				SensorDisplay display = disps[d];
				try { display.messageRetrieved(m); }
				catch(Exception e) { e.printStackTrace(); }
			}
		}
	}
	
	private Map parseMessage(String msg)
	{
		Map m = new HashMap();
		
		// No split() method, or regexps exists...
		int index = msg.indexOf(msgSeparator);
		
		while(index >= 0)			
		{
			try
			{
				// Set current message
		    	String curMsg = msg.substring(0,index).trim();
		        setMessage(m,curMsg);

				// Next match
		        msg = msg.substring(index + msgSeparator.length());
		        index = msg.indexOf(msgSeparator);
				
			}
			catch(Exception e)
			{
				// Abort while loop if somethings fails
				e.printStackTrace();
				index = -1;
			}
		}

		// Set final bit of string
        setMessage(m,msg);
		
		return m;
	}	
	
	private void setMessage(Map m, String s)
	{
		if( m == null || s == null || s.length() <= msgIdLen)
			return;
		
		String k = s.substring(0,msgIdLen);
		String v = s.substring(msgIdLen);
		try 
		{ 
			double d = Double.parseDouble(v); 
			m.put( k, new Double(d) );		}	
		catch(Exception e) { e.printStackTrace(); }
	}

}
