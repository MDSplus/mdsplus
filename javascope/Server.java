import java.io.*;
import java.net.*;


class MultiServer extends Thread
{
    Socket sock = null;

    public MultiServer(Socket socket)
    {
	super("MultiServer");
	this.sock = socket;
    }

    public void run()
    {
	String name;
	int n_points, i, msglen, tot_bytes, read_bytes;
	float  curr_f,
	       signal[];
	Socket s = null;
	byte b[], lenbuf[] = new byte[4];
	boolean communicating = true;
	DataInputStream dis, sdis;
	DataOutputStream dos, sdos;
	try
	{
	    dis = new DataInputStream(new BufferedInputStream(
		sock.getInputStream()));
	    dos = new DataOutputStream(new BufferedOutputStream(
		sock.getOutputStream()));

	    s = new Socket("pdigi1.igi.pd.cnr.it",8000);
	    sdos = new DataOutputStream(new BufferedOutputStream(
		s.getOutputStream()));
	    sdis = new DataInputStream(new BufferedInputStream(
		s.getInputStream()));
	} catch(IOException e){System.out.println("Error creacting connections\n"); return;}
	while(communicating)
	try{
	    msglen = dis.readInt();
	    sdos.writeInt(msglen);
	    msglen -= 4;
	    b = new byte[msglen];
	    tot_bytes = 0;
	    while(tot_bytes < msglen)
	    {
		read_bytes = dis.read(b, 0, msglen - tot_bytes);
		if(read_bytes > 0)
		    sdos.write(b, 0, read_bytes);
		tot_bytes += read_bytes;
	    }

System.out.println("Messaggio spedito: "+tot_bytes+" bytes\n");
	    sdos.flush();



	    ReadBuf(lenbuf, sdis);
	    dos.write(lenbuf, 0, 4);
	    Swap4(lenbuf);
	    ByteArrayInputStream bis = new ByteArrayInputStream(lenbuf);
	    DataInputStream bbis = new DataInputStream(bis);
	    msglen = bbis.readInt();

    	    msglen -= 4;
	    b = new byte[msglen];
    	    tot_bytes = 0;
	    while(tot_bytes < msglen)
	    {
		read_bytes = sdis.read(b, 0, msglen - tot_bytes);
		if(read_bytes > 0)
		    dos.write(b, 0, read_bytes);
		tot_bytes += read_bytes;
	    }
System.out.println("Messaggio ricevuto: "+tot_bytes+" bytes\n");
	    dos.flush();
	} catch(IOException e)
	{    communicating = false; System.out.println("Connection ended\n"); }
	try { 
	    s.close(); 
	    sock.close();
	}catch(IOException e){System.out.println("Error closing connection"); }
    }

private void ReadBuf(byte buf[], DataInputStream dis) throws IOException
{
    int bytes_to_read = buf.length, read_bytes = 0, curr_offset = 0;
    while(bytes_to_read > 0)
    {
	read_bytes = dis.read(buf, curr_offset, bytes_to_read);
	curr_offset += read_bytes;
	bytes_to_read -= read_bytes;
    }
}
private void Swap4(byte buf[])
{
    byte tmp;
    tmp = buf[0];
    buf[0] = buf[3];
    buf[3] = tmp;
    tmp = buf[1];
    buf[1] = buf[2];
    buf[2] = tmp;
}


}
	    
	    


class Server
{
    public static void main(String argv[])
    {
	boolean listening = true;
	ServerSocket server_sock = null;
	Socket client_sock = null;
	try {
	    server_sock = new ServerSocket(8000);
	} catch(IOException e){
	    System.err.println("Cannot listen on port "+ "8000" + e);
	    System.exit(1);
	}
	while(listening)
	{
	    try {
	     client_sock = server_sock.accept();
	    } catch(IOException e){
	    	System.err.println("Accept failed "+ 8000+", " + e);
		continue;
	    }
	    System.out.println(
			"Accept from " + (client_sock.getInetAddress()).toString() + "\n");
	    new MultiServer(client_sock).start();
	}

	try
	{
   	    server_sock.close();
	} catch(IOException e){
	    System.err.println("Cannot close port "+ 3333+", " + e);
	    System.exit(1);
	}	
    }
}


