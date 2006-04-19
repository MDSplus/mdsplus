import java.io.*;
import java.util.*;

class Convert
{
    String path, fileName;
    public Convert(String path, String fileName)
    {
    	this.path = path;
	this.fileName = fileName;
    }
    
    
    public static void main(String args[])
    {
    	Convert conv = new Convert(args[0], args[1]);
    	conv.convertMatrix();
    }
    
    public void convertMatrix()
    {
	System.out.println(path);
    	Database rfx = new Database("rfx", -1);
	float [] data = new float[192*192];
	if(fileName.equalsIgnoreCase("diagonal"))
	{
	    for(int i = 0; i < 192; i++)
	    {
	    	for(int j = 0; j < 192; j++)
		{
		    if(i == j)
		    	data[192*i+j] = 1;
		    else
		    	data[192*i+j] = 0;
		}
	    }
	}
	else
	{
	    try {
	    	BufferedReader br = new BufferedReader(new FileReader(fileName));
	    	for(int i = 0; i < 192; i++)
	    	{
	    	    System.out.println(i);
	    	    String line = br.readLine();
		    StringTokenizer st = new StringTokenizer(line, "[], ");
		    for(int j = 0; j < 192; j++)
		    	data[192*i+j] = Float.parseFloat(st.nextToken());
	    	}
	    }catch(Exception exc){System.err.println(exc);} 	
	 }
	 try {
	    rfx.open();
	    NidData nid = rfx.resolve(new PathData(path), 0);
	    FloatArray array = new FloatArray(data);
	    rfx.putData(nid, array, 0);
	    rfx.close(0);
	 }catch(Exception exc){System.err.println(exc);} 
    }
 }   
	
