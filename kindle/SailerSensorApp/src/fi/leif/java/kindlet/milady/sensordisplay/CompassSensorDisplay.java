package fi.leif.java.kindlet.milady.sensordisplay;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Polygon;
import java.util.Map;

import fi.leif.java.kindlet.milady.Config;

public class CompassSensorDisplay extends SensorDisplay
{
	private Compass compass;

	private final String MSG_GPS_ID = "HD";
	private final String MSG_MAG_ID = "MN";
	
	private final int 		POINT_W = 60;
	private final int 		POINT_H = 40;

	private final int 		DIRS_FONT_SIZE = 170;
	private final int 		DIRS_MARG = 60;
	private final String[] 	DIRS = {"W","N","E","S"};
	private final int 		DIRS_SHOW=3;
	
	public CompassSensorDisplay(Config config)
	{
		super(config);
		
		setLayout(new BorderLayout());
		
		// Add header
		addHeader("COMPASS");
		
		// Add compass
		compass = new Compass();
		add( compass, BorderLayout.CENTER );
	}

	private class Compass extends Component
	{
		private String[] dirs;
		private double degree;

		public Compass()
		{
			super();
			dirs = concat( DIRS, slice( DIRS, DIRS_SHOW) );
		}
		
		public void setDegree(double degree)
		{
			this.degree = degree;
		}
		
		public void paint( Graphics g ) 
		{
			super.paint(g);
			
			/*
			g.setColor(Color.BLACK);
			g.setFont( new Font(config.FONTFAMILY, Font.BOLD, DIRS_FONT_SIZE) );
			g.drawString(""+degree, 0 ,150);
			*/
			
			drawPointer(g);
			drawDirs(g);
		}
		
		private void drawPointer(Graphics g)
		{
			int w = getWidth();
			int x = w/2;
			int h = getHeight();
			Polygon pointer = new Polygon(new int[]{x-POINT_W,x,x+POINT_W}, new int[]{h,h-POINT_H,h}, 3);
			
			// Paint pointer
			g.setColor(Color.BLACK);
			g.fillPolygon(pointer);
			g.drawLine(x, 0, x, h);
			g.drawLine(x-1, 0, x-1, h);
			g.drawLine(x+1, 0, x+1, h);
		}
		
		private void drawDirs(Graphics g) {

			// Init cardinal directions values
			int w = getWidth();
			int h = getHeight();
			int dirs_y = (h-POINT_H);
			double dirs_step = ( w - (DIRS_MARG*2) ) / (DIRS_SHOW-1);
			double dirs_w = dirs_step * DIRS.length;
			
			g.setColor(Color.BLACK);
			g.setFont( new Font(config.FONTFAMILY, Font.BOLD, DIRS_FONT_SIZE) );

			double x = DIRS_MARG - ( (degree/360) * dirs_w );
			
			for(int i=0; i<dirs.length; i++) {
				String c = dirs[i];
				FontMetrics fm = g.getFontMetrics();
				int cw = fm.stringWidth(c);
				g.drawString(c, (int)(x-(cw/2)), dirs_y);
				x += dirs_step;
			}
		}		
		
		private String[] slice(String[] a, int l)
		{
			String[] b = new String[l];
			System.arraycopy(a, 0, b, 0, l);			
			return b;
		}
		
		private String[] concat(String[] a, String[] b)
		{
			String[] c= new String[a.length+b.length];
			System.arraycopy(a, 0, c, 0, a.length);
			System.arraycopy(b, 0, c, a.length, b.length);
			return c;			
		}
	}
	
	public void messageRetrieved(Map m) throws Exception
	{
		if(m.get(MSG_GPS_ID) != null)
		{
			Double d = (Double)m.get(MSG_GPS_ID);
			compass.setDegree(d.doubleValue());
			compass.repaint();
		}
		
		// TODO: Magnetic compass
	}
}
