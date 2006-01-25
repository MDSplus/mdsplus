import java.io.*;
import java.util.*;

class Convert
{
    public static void main(String args[])
    {
    	String path = args[0];
	String fileName = args[1];
	System.out.println(path);
    	Database rfx = new Database("rfx", -1);
	try {
	    rfx.open();
	    float []data = new float[192*192];
	    BufferedReader br = new BufferedReader(new FileReader(fileName));
	    for(int i = 0; i < 192; i++)
	    {
	    	System.out.println(i);
	    	String line = br.readLine();
		StringTokenizer st = new StringTokenizer(line, "[], ");
		for(int j = 0; j < 192; j++)
		    data[192*i+j] = Float.parseFloat(st.nextToken());
	    }
	    NidData nid = rfx.resolve(new PathData(path), 0);
	    FloatArray array = new FloatArray(data);
	    rfx.putData(nid, array, 0);
	    rfx.close(0);
	 }catch(Exception exc){System.err.println(exc);} 
    }
 }   
	
