package mds.wave;

import java.io.*;
import java.awt.image.*;
import java.awt.datatransfer.*;

public class ImageTransferable implements Transferable, ClipboardOwner
{
	BufferedImage ri;

	public ImageTransferable(BufferedImage img)
	{
		ri = img;
	}

	@Override
	public Object getTransferData(DataFlavor flavor) throws IOException, UnsupportedFlavorException
	{
		if (ri == null)
			return null;
		if (!isDataFlavorSupported(flavor))
		{
			throw new UnsupportedFlavorException(flavor);
		}
		return ri;
	}

	@Override
	public DataFlavor[] getTransferDataFlavors()
	{
		return new DataFlavor[]
		{ DataFlavor.imageFlavor };
	}

	@Override
	public boolean isDataFlavorSupported(DataFlavor flavor)
	{
		return DataFlavor.imageFlavor.equals(flavor);
	}

	@Override
	public void lostOwnership(Clipboard clipboard, Transferable contents)
	{
		/*
		 * System.out.println ("ImageTransferable lost ownership of "
		 * +clipboard.getName()); System.out.println ("data: " + contents);
		 */
	}
}
