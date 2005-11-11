import java.util.*;

public class KillServer extends MdsConnection
{
  KillServer(String server)
  {
    super(server);
  }


  public static void main(String args[])
  {
    String serverIp = "";
    int serverPort = 0;
    try {
      serverIp = args[0];
      serverPort = Integer.parseInt(args[1]);
    }catch(Exception exc)
    {
      System.err.println("Usage: java KillServer <ip>, <port>");
      System.exit(0);
    }
    KillServer killServer = new KillServer(serverIp + ":"+serverPort);
    killServer.ConnectToMds(false);
    killServer.MdsValue("kill", new Vector());

  }



}