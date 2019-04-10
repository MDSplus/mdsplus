package jScope;

import java.io.*;
import java.awt.image.*;
import java.awt.datatransfer.*;

class ImageTransferable implements Transferable, ClipboardOwner
{

    BufferedImage ri;

    ImageTransferable(BufferedImage img)
    {
        ri = img;
    }

    public DataFlavor[] getTransferDataFlavors() {
        return new DataFlavor[] { DataFlavor.imageFlavor};
    }

    public boolean isDataFlavorSupported(DataFlavor flavor) {
        return DataFlavor.imageFlavor.equals(flavor);
    }

    public Object getTransferData(DataFlavor flavor)
            throws IOException, UnsupportedFlavorException {

        if(ri == null)
            return null;

        if (!isDataFlavorSupported(flavor)) {
            throw new UnsupportedFlavorException(flavor);
        }
        return ri;
    }

    public void lostOwnership(Clipboard clipboard, Transferable contents)
    {
        /*
	    System.out.println ("ImageTransferable lost ownership of "  +clipboard.getName());
	    System.out.println ("data: " + contents);
	    */
    }
}