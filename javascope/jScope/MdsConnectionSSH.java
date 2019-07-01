package jScope;
import java.io.*;
import java.util.*;

public class MdsConnectionSSH extends MdsConnection
{
    Process p;
    public MdsConnectionSSH()
    {
//        this.provider = "MdsConnectionSSH";
    }
    public MdsConnectionSSH(String provider)
    {
	this.provider = provider;
    }
    public void connectToServer() throws IOException
    {
	host = getProviderHost();
	port = 0;
	user = "";
//        ProcessBuilder pb = new ProcessBuilder("cmd.exe", "/C", "start", "mdsip-client-ssh", host, "mdsip-server-ssh");
	ProcessBuilder pb = new ProcessBuilder("mdsip-client-ssh", host, "mdsip-server-ssh");
//	pb.redirectInput(ProcessBuilder.Redirect.PIPE);
//	pb.redirectOutput(ProcessBuilder.Redirect.PIPE);
//	pb.redirectError(ProcessBuilder.Redirect.PIPE);
	try {
	    Process p = pb.start();
	    dis = new BufferedInputStream(p.getInputStream());
	    dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
	}catch(Exception exc){
	    pb = new ProcessBuilder("mdsip-client-ssh.bat", host, "mdsip-server-ssh");  //Windows
	    Process p = pb.start();
	    dis = new BufferedInputStream(p.getInputStream());
	    dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
       }
   }
    public String getProvider() { return "ssh";}
 //   public void setProvider(String provider){}
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
