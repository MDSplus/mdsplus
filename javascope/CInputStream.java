import java.lang.*;
import java.io.*;

public class CInputStream extends FilterInputStream
{
    private StringBuffer sb;
    private boolean string_done;
    private byte buffer[];
    private int idx, bufsize, curr_dim; 

    public CInputStream(InputStream in)
    {
	super(in);
	string_done = false;
	sb = new StringBuffer(200);
	buffer = new byte[2048];
	bufsize = 2048;
	try {
	    curr_dim = in.read(buffer, 0, bufsize);
	} catch(Exception e) { curr_dim = 0;}
	System.out.println("Letti " + curr_dim);
	idx = 0;
    }

    public int read() throws IOException
    {
	int b;
	if(idx == curr_dim)
	{
	    curr_dim = in.read(buffer, 0, bufsize);
//	    System.out.println("Letto");
	    idx = 0;
	    if(curr_dim <= 0)
	    {
		string_done = true;
		return -1;
	    }
	}
	b = (int)buffer[idx++];
//System.out.println(b);

	if(b == ' ' || b == '\n' || b == '\t'
	  || b == '\r')
	    string_done = true;
	else
	    sb.append((char)b);
	return b;
    }
    public int skip() throws IOException
    {
	int b;
	sb = new StringBuffer(512);
	do {
	    b = read();
	    if(b == -1) return -1;
	} while (b == ' ' || b == '\n' || b == '\t'
		|| b == '\r');
	string_done = false;
	return b;
    }


    public int readInt()throws IOException
    {
	int b;
	Integer in;

	if(skip() == -1) 
	    throw (new IOException("End of File encountered"));
	while(!string_done)
	    b = read();
	if(sb.length() == 0)
	    throw (new IOException("End of File encountered"));

	in = new Integer(sb.toString());
	return in.intValue();
    }
	
    public float readFloat() throws IOException
    {
	int i, b;
	Float fn;

	if(skip() == -1) 
	    throw (new IOException("End of File encountered"));

	while(!string_done)
	    b = read();
	if(sb.length() == 0)
	    throw (new IOException("End of File encountered"));
//System.out.println(sb.toString());
	fn = new Float(sb.toString());
	return fn.floatValue();
   }
}


