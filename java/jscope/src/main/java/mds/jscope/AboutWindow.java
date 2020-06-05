package mds.jscope;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;

/* $Id$ */
import javax.swing.ImageIcon;
import javax.swing.JLabel;


public class AboutWindow extends JLabel  {
	private static final long serialVersionUID = 1L;
	static String javaVersion = System.getProperty("java.version");
	final ImageIcon io;

	public AboutWindow() {
		io = new ImageIcon(getClass().getClassLoader().getResource("mds/jscope/about.jpg"));
		if(io != null)
			setIcon(io);
	}

	public void update(Graphics g) {
		paint(g);
	}

	public void paint(Graphics gReal) {
		if(io == null)
			return;
		//Double buffering
		Image imageBuffer = createImage( getWidth(), getHeight() );
		Graphics g = imageBuffer.getGraphics();

		Image image = io.getImage();

		g.drawImage( image, 1, 1, null );
		Toolkit.getDefaultToolkit().sync();

		final int start  = 32+2;
		//final int top    = 102+1;
		final int botton = 268+1;
		final int delta  = 14;

		g.setColor( new Color( 128, 128, 128 ) );
		g.drawRect( -1, -1, getWidth(), getHeight() ); // makes a bevel border likeness

		g.drawString( jScopeFacade.VERSION, start, botton - 3*delta);
		g.drawString( "http://www.igi.cnr.it",  start, botton - 2*delta );
		g.drawString( "JVM used :" +  javaVersion, start, botton -  delta );

		gReal.drawImage( imageBuffer, 0, 0, this );
	}

}
