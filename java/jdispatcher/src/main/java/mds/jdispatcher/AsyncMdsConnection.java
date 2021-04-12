/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package mds.jdispatcher;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.StandardSocketOptions;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.concurrent.*;
import mds.connection.MdsConnection;

/**
 *
 * @author mdsplus
 */
public class AsyncMdsConnection extends MdsConnection
{
    static final long WRITE_TIMEOUT_SECS = 60;
    class WriteCompletionHandler implements CompletionHandler<Integer, Void>
    {
	boolean done = false;
	int numWritten;
	synchronized int getNumWritten()
	{
	    if(!done)
	    {
		try {
		    wait();
		}catch(InterruptedException exc){}
	    }
	    return numWritten;
	}
        public synchronized void completed(Integer result, Void attachment)
	{
	    numWritten = result.intValue();
	    done = true;
	    notify();
	}
	public synchronized void failed(Throwable exc, Void attachment)
	{
	    numWritten = -1;
	    done = true;
	    notify();
	}
    }
    class AsyncSocket {
        class AsynInputStream extends InputStream
        {
		
	    public int available() {return 1;}
            public int read() throws IOException
            {
                ByteBuffer bb = ByteBuffer.allocate(1);
		int bytesRead = 0;
		while(bytesRead == 0)
		{
                    Future<Integer> f = asc.read(bb);
                    try {
                        bytesRead = f.get().intValue();
                    }catch(Exception exc){}
		}
		if(bytesRead == -1)
		    throw new IOException();
                return bb.array()[0];
            }
        }
        class AsynOutputStream extends OutputStream
        {
            public void write(int b) throws IOException
            {
                byte[] bb = new byte[1];
                bb[0] = (byte)b;
		WriteCompletionHandler wch = new WriteCompletionHandler();
		asc.write(ByteBuffer.wrap(bb), WRITE_TIMEOUT_SECS, TimeUnit.SECONDS, null, wch);
		int numWritten = wch.getNumWritten();
		if(numWritten != 1)
		    throw new IOException();
            }

        }
        AsynchronousSocketChannel asc;
        public AsyncSocket(String host, int port) throws IOException
        {
            try {
                asc = AsynchronousSocketChannel.open();
                InetSocketAddress hostAddress = new InetSocketAddress(host, port);
                Future<Void> future = asc.connect(hostAddress);
                if(future.get() != null)
		    throw new IOException();
            }catch(Exception exc){System.out.println(exc);}
        }
        public InputStream getInputStream()
        {
            return new AsynInputStream();
        }
        public OutputStream getOutputStream()
        {
            return new AsynOutputStream();
        }
        public void setTcpNoDelay(boolean noDelay) throws IOException
        {
            asc.setOption(StandardSocketOptions.TCP_NODELAY, noDelay);
        }
    }  //End Inner class AsyncSocket
    public AsyncMdsConnection(String server)
    {
        super(server);
    }
    public void connectToServer() throws IOException
    {
            host = getProviderHost();
            port = getProviderPort();
            user = getProviderUser();
            AsyncSocket asock = new AsyncSocket(host, port);
            asock.setTcpNoDelay(true);
            dis = asock.getInputStream();
            dos = new DataOutputStream(asock.getOutputStream());
    } 
	
}
