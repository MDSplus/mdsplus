package mds.jscope;
import java.io.*;

import mds.wavedisplay.MdsConnection;

public class MdsConnectionSSH extends MdsConnection
{
	Process p;
	public MdsConnectionSSH()
	{
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
		ProcessBuilder pb = new ProcessBuilder("mdsip-client-ssh", host, "mdsip-server-ssh");
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
		this.connected = true;
	}
	public String getProvider() { return "ssh";}
	public int DisconnectFromMds()
	{
		// Closing pipes crashes JVM!!!
		dis = null;
		dos = null;
		p.destroy();
		return super.DisconnectFromMds();
	}
}
