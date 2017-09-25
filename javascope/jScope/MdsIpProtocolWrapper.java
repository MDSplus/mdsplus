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
package jScope;
import java.io.*;

/**
 *
 * MdsProtocolWrapper handles mdstcpip management for protocol plugin
 */
public class MdsIpProtocolWrapper 
{
    static {
        try
        {
          System.loadLibrary("JavaMds");
        }
        catch(UnsatisfiedLinkError  e)
        {
          javax.swing.JOptionPane.showMessageDialog(null, "Can't load data provider class LocalDataProvider : " + e, "Alert LocalDataProvider",
                                          javax.swing.JOptionPane.ERROR_MESSAGE);
        }
    }
    int connectionIdx = -1;
    class MdsIpInputStream extends InputStream
    {
        public int read() throws IOException
        {
            if(connectionIdx == -1)  throw new IOException("Not Connected");
            byte [] readBuf = recv(connectionIdx, 1);
            if(readBuf == null) throw new IOException("Cannot Read Data");
            return readBuf[0];
        }
        public int read(byte buf[]) throws IOException
        {
            if(connectionIdx == -1)  throw new IOException("Not Connected");
            byte [] readBuf = recv(connectionIdx, buf.length);
            if(readBuf == null) throw new IOException("Cannot Read Data");
	    System.arraycopy(readBuf, 0, buf, 0, buf.length);
            return buf.length;
        }
        public int read(byte buf[], int offset, int len) throws IOException
        {
            if(connectionIdx == -1)  throw new IOException("Not Connected");
            byte [] readBuf = recv(connectionIdx, len);
            if(readBuf == null || readBuf.length == 0) throw new IOException("Cannot Read Data");
	    System.arraycopy(readBuf, 0, buf, offset, readBuf.length);
            return readBuf.length;
        }

    }
    
    InputStream getInputStream()
    {
        return new MdsIpInputStream();
    }
    
    class MdsIpOutputStream extends OutputStream
    {
        public void write(int b) throws IOException
        {
            if(connectionIdx == -1)  throw new IOException("Not Connected");
            int numSent = send(connectionIdx, new byte[]{(byte)b}, false);
            if(numSent == -1) throw new IOException("Cannot Write Data");
        }
        public void write(byte[] b) throws IOException
        {
            if(connectionIdx == -1)  throw new IOException("Not Connected");
            int numSent = send(connectionIdx, b, false);
            if(numSent == b.length) throw new IOException("Incomplete write");
        }
/*        public void flush() throws IOException
        {
System.out.println("FLUSH..");
            if(connectionIdx == -1)  throw new IOException("Not Connected");
            MdsIpProtocolWrapper.this.flush(connectionIdx);
System.out.println("FLUSH FATTO");
        }
*/        public  void close() throws IOException
        {
            if(connectionIdx != -1)
            {
                disconnect(connectionIdx);
                connectionIdx = -1;
            }
        }
        
    }
    
    OutputStream getOutputStream()
    {
        return new MdsIpOutputStream();
    }
    
    public native int connectToMds(String url);
    public native int send(int connectionId, byte[] sendBuf, boolean nowait);
    public native byte[] recv(int connectionId, int len);
    public native void flush(int connectionId);
    public native void disconnect(int connectionId);
    
    public MdsIpProtocolWrapper(String url)
    {
        connectionIdx = connectToMds(url);
    }
    
    
    
    public static void main(String args[])
    {
        MdsIpProtocolWrapper mpw = new MdsIpProtocolWrapper("tcp");
        int idx = mpw.connectToMds("tcp://ra22.igi.cnr.it:8100");
        System.out.println("Connected: " + idx);
    }
}
