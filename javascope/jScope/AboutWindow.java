package jScope;

/* $Id$ */
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;


public class AboutWindow extends JLabel  {
    static String javaVersion = System.getProperty("java.version");
    ImageIcon io = null;

    public AboutWindow() {
	String icon_file = jScopeFacade.findFileInClassPath("jdocs/about_jscope.jpg");
	if (icon_file != null)
	io = new ImageIcon(icon_file);
	else
	io = new ImageIcon(getClass().getClassLoader().getResource("jdocs/about_jscope.jpg"));

	if (jScopeFacade.is_debug)
	System.out.println("about_jscope image path " + icon_file + io);
	if(io != null) setIcon(io);
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

	int start  = 32+2;
	int top    = 102+1;
	int botton = 268+1;
	int delta  = 14;

	g.setColor( new Color( 128, 128, 128 ) );
	g.drawRect( -1, -1, getWidth(), getHeight() ); // makes a bevel border likeness

	g.drawString( jScopeFacade.VERSION, start, botton - 3*delta);
	g.drawString( "http://www.igi.cnr.it",  start, botton - 2*delta );
	g.drawString( "JVM used :" +  javaVersion, start, botton -  delta );

	gReal.drawImage( imageBuffer, 0, 0, this );
	}

    }
