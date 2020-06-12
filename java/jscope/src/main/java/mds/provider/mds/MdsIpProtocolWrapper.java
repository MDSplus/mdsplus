package mds.provider.mds;

import java.io.*;

/**
 *
 * MdsProtocolWrapper handles mdstcpip management for protocol plugin
 */
public class MdsIpProtocolWrapper
{
	class MdsIpInputStream extends InputStream
	{
		@Override
		public int read() throws IOException
		{
			if (connectionIdx == -1)
				throw new IOException("Not Connected");
			final byte[] readBuf = recv(connectionIdx, 1);
			if (readBuf == null)
				throw new IOException("Cannot Read Data");
			return readBuf[0];
		}

		@Override
		public int read(byte buf[]) throws IOException
		{
			if (connectionIdx == -1)
				throw new IOException("Not Connected");
			final byte[] readBuf = recv(connectionIdx, buf.length);
			if (readBuf == null)
				throw new IOException("Cannot Read Data");
			System.arraycopy(readBuf, 0, buf, 0, buf.length);
			return buf.length;
		}

		@Override
		public int read(byte buf[], int offset, int len) throws IOException
		{
			if (connectionIdx == -1)
				throw new IOException("Not Connected");
			final byte[] readBuf = recv(connectionIdx, len);
			if (readBuf == null || readBuf.length == 0)
				throw new IOException("Cannot Read Data");
			System.arraycopy(readBuf, 0, buf, offset, readBuf.length);
			return readBuf.length;
		}
	}
	class MdsIpOutputStream extends OutputStream
	{
		/*
		 * public void flush() throws IOException { System.out.println("FLUSH..");
		 * if(connectionIdx == -1) throw new IOException("Not Connected");
		 * MdsIpProtocolWrapper.this.flush(connectionIdx);
		 * System.out.println("FLUSH FATTO"); }
		 */ @Override
		public void close() throws IOException
		{
			if (connectionIdx != -1)
			{
				disconnect(connectionIdx);
				connectionIdx = -1;
			}
		}

		@Override
		public void write(byte[] b) throws IOException
		{
			if (connectionIdx == -1)
				throw new IOException("Not Connected");
			final int numSent = send(connectionIdx, b, false);
			if (numSent == b.length)
				throw new IOException("Incomplete write");
		}

		@Override
		public void write(int b) throws IOException
		{
			if (connectionIdx == -1)
				throw new IOException("Not Connected");
			final int numSent = send(connectionIdx, new byte[]
			{ (byte) b }, false);
			if (numSent == -1)
				throw new IOException("Cannot Write Data");
		}
	}

	static
	{
		try
		{
			System.loadLibrary("JavaMds");
		}
		catch (final UnsatisfiedLinkError e)
		{
			javax.swing.JOptionPane.showMessageDialog(null, "Can't load data provider class LocalDataProvider : " + e,
					"Alert LocalDataProvider", javax.swing.JOptionPane.ERROR_MESSAGE);
		}
	}

	public static void main(String args[])
	{
		final MdsIpProtocolWrapper mpw = new MdsIpProtocolWrapper("tcp");
		final int idx = mpw.connectToMds("tcp://ra22.igi.cnr.it:8100");
		System.out.println("Connected: " + idx);
	}

	int connectionIdx = -1;

	public MdsIpProtocolWrapper(String url)
	{
		connectionIdx = connectToMds(url);
	}

	public native int connectToMds(String url);

	public native void disconnect(int connectionId);

	public native void flush(int connectionId);

	InputStream getInputStream()
	{ return new MdsIpInputStream(); }

	OutputStream getOutputStream()
	{ return new MdsIpOutputStream(); }

	public native byte[] recv(int connectionId, int len);

	public native int send(int connectionId, byte[] sendBuf, boolean nowait);
}
