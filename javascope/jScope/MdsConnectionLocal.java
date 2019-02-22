package jScope;
import java.io.*;
import java.util.*;

public class MdsConnectionLocal extends MdsConnection
{
    Process p;
    public MdsConnectionLocal()
    {
        this.provider = "local";
    }
    public MdsConnectionLocal(String provider)
    {
        this.provider = "local";
    }
    public void connectToServer() throws IOException
    {
        host = "local";
        port = 0;
        user = "";
	ProcessBuilder pb = new ProcessBuilder("mdsip-client-local", "mdsip-client-local");
//	pb.redirectInput(ProcessBuilder.Redirect.PIPE);
//	pb.redirectOutput(ProcessBuilder.Redirect.PIPE);
        try {
            Process p = pb.start();
            dis = new BufferedInputStream(p.getInputStream());
            dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
        }catch(Exception e)   
        {
            pb = new ProcessBuilder("mdsip-client-local.bat", "mdsip-client-local");  //Windows
            Process p = pb.start();
            dis = new BufferedInputStream(p.getInputStream());
            dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
        }
    }  
    public String getProvider() { return "local";}
    public void setProvider(String provider){}
    public  int DisconnectFromMds()
    {
	try 
	{
//	    dos.close();   Closing pipes charshes JVM!!!!!!!!!!!!!!!!!!!!!!!!11
//          dis.close();
	    dis = null;
	    dos = null;
	    p.destroy();
            receiveThread.waitExited();
	}
	catch(Exception e)
	{
	   // System.out.println("Error disconnecting From local mdsip: " + e);
	}
	return 1;
    }
}
