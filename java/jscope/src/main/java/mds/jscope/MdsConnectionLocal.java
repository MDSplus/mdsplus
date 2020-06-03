package mds.jscope;
import java.io.*;

import mds.wavedisplay.MdsConnection;

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
		this.connected = true;
	}
	public String getProvider() { return "local";}
	public void setProvider(String provider){}
	public  int DisconnectFromMds()
	{
		dis = null;
		dos = null;
		p.destroy();
		return super.DisconnectFromMds();
	}
}
