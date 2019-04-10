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
