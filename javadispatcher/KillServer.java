/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
import java.util.*;
import jScope.*;

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
    }
	catch(Exception exc)
    {
      System.err.println("Usage: java KillServer <ip>, <port>");
      System.exit(0);
    }

    try {
		KillServer killServer = new KillServer(serverIp + ":"+serverPort);
		killServer.ConnectToMds(false);

		killServer.MdsValue("kill", new Vector(), false);

	
		killServer.DisconnectFromMds();
	}
	catch(Exception exc)
    {
      System.err.println("Cannot connect to server: " + serverIp + ":"+serverPort);
    }

	     
	System.exit(0);
 
  }



}