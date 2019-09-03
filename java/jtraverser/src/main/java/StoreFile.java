//Stores a file as a bonary array into a pulse file
//Arguments:
//1) File name
//2) Esperiment
//3) shot
//4) Node name
import java.io.*;
public class StoreFile
{
    public static void main(String args[])
    {
	if(args.length < 3 || args.length > 5)
	{
	    System.err.println("Usage: java StoreFile <filename> <nodename> <experiment> [< shot> ");
	    System.exit(0);
	}
	String fileName = args[0];
	String nodeName = args[1];
	String experiment = args[2];

	int shot = -1;
	if(args.length == 4)
	{
	    try
	    {
	        shot = Integer.parseInt(args[3]);
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Invalid shot number");
	        System.exit(0);
	    }
	}
	Database tree = new Database(experiment, shot);
	try {
	    tree.open();
	}catch(Exception exc)
	{
	    System.err.println("Cannot open experiment "+ experiment + " shot " + shot + ": " + exc);
	    System.exit(0);
	}
	NidData nid = null;
	try {
	    nid = tree.resolve(new PathData(nodeName), 0);
	}catch(Exception exc)
	{
	    System.err.println("Cannot find node "+ nodeName);
	    System.exit(0);
	}

	byte [] serialized = null;
	try {
	    RandomAccessFile raf = new RandomAccessFile(fileName, "r");
	    serialized = new byte[(int)raf.length()];
	    raf.read(serialized);
	    raf.close();
	}catch(Exception exc)
	{
	    System.err.println("Cannot read file "+ fileName + ": " + exc);
	    System.exit(0);
	}
	try {
	    tree.putData(nid, new ByteArray(serialized), 0);
	}catch(Exception exc)
	{
	    System.err.println("Error writing data in"+ nodeName + ": " + exc);
	    System.exit(0);
	}
    }
}

