import java.io.*;
import java.util.*;
import java.net.*;


class ReaderConfig extends Reader
{
    Vector line_buf = new Vector();
    int curr_line = 0;
    
    
    ReaderConfig(URL file) throws IOException
    {
        String str;
        InputStream ins = file.openConnection().getInputStream();
        InputStreamReader inr = new InputStreamReader(ins);
        BufferedReader in = new BufferedReader(inr);
        while((str=in.readLine()) != null)
            line_buf.addElement(new String(str));
        in.close();
        
    }
            
    ReaderConfig(FileReader fr) throws IOException
    {
        StringBuffer buf = new StringBuffer();
        String str;
        
        BufferedReader in = new BufferedReader(fr);
        while((str=in.readLine()) != null)
            line_buf.addElement(new String(str));
        in.close();
    }

    ReaderConfig(String in)
    {
        StringTokenizer token_buf = new StringTokenizer(in, "\r\t\f\n", false);
        while(token_buf.hasMoreTokens()) {
            String s = new String(token_buf.nextToken());
            System.out.println(s);
            line_buf.addElement(s);
        }
    }

    public String readLine()
    {
        if(curr_line < line_buf.size())
            return (String)line_buf.elementAt(curr_line++);
        else
            return null;
    }
    
    public void close()
    {
    }
    
    public void mark(int i)
    {
    }
    
    public boolean markSupported()
    {
        return false;
    }
    public int read()
    {
        return 0;
    }
    public int read(char c[], int i, int l)
    {
        return 0;
    }
    public boolean ready()
    {
        return true;
    }
    public void reset()
    {
        curr_line = 0;
    }
    public long skip(long l)
    {
        return 0;
    }
    
}