package mds.jscope;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.IndexColorModel;

import javax.swing.ImageIcon;
import javax.swing.JLabel;

public class SplashScreen extends JLabel
{
	private static final long serialVersionUID = 1L;
	private static final String IMAGE_PATH = "mds/jscope/splash.jpg";
	private static final String URL = "https://www.mdsplus.org";
	private final static String JAVE_VERSION = System.getProperty("java.version");
	private static final String VERSION;
	static
	{
		final String version = SplashScreen.class.getPackage().getImplementationVersion();
		VERSION = version == null ? "unknown" : version;
	}
	final ImageIcon io;

	public SplashScreen()
	{
		final java.net.URL url = SplashScreen.class.getClassLoader().getResource(IMAGE_PATH);
		if (url == null)
		{
			System.err.println("SplashScree image " + IMAGE_PATH + " not found");
			final byte rgb[] = new byte[]
			{ -16 };
			final IndexColorModel cm = new IndexColorModel(1, 1, rgb, rgb, rgb);
			io = new ImageIcon(new BufferedImage(320, 200, BufferedImage.TYPE_BYTE_BINARY, cm));
		}
		else
		{
			io = new ImageIcon(url);
		}
		setIcon(io);
	}

	@Override
	public void paint(Graphics gReal)
	{
		final Image imageBuffer = createImage(getWidth(), getHeight());
		final Graphics g = imageBuffer.getGraphics();
		final Image image = this.io.getImage();
		g.drawImage(image, 1, 1, null);
		Toolkit.getDefaultToolkit().sync();
		final int delta = 14;
		final int start = 32 + 2;
		final int botton = io.getIconHeight() - 2 * delta;
		g.setColor(Color.GRAY);
		g.drawRect(-1, -1, getWidth(), getHeight()); // makes a bevel border likeness
		g.setColor(Color.BLACK);
		g.drawString(VERSION, start, botton - 3 * delta);
		g.drawString(URL, start, botton - 2 * delta);
		g.drawString("JVM used :" + JAVE_VERSION, start, botton - delta);
		gReal.drawImage(imageBuffer, 0, 0, this);
	}
}
