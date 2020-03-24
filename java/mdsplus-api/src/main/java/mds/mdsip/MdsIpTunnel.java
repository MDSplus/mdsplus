package mds.mdsip;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import mds.mdsip.MdsIp.Connection;

public class MdsIpTunnel implements Connection{
	final private Process process;
	final private InputStream dis;
	final private OutputStream dos;

	@Override
	public void close() throws IOException {
		if(this.isOpen()) this.process.destroy();
	}

	public MdsIpTunnel() throws IOException {
		process = new ProcessBuilder("mdsip", "-P", "tunnel").start();
		dis = this.process.getInputStream();
		dos = this.process.getOutputStream();
	}

	@Override
	public boolean isOpen() {
		return this.process.isAlive();
	}

	private final int wait_available() throws IOException {
		int i = 1, tot = 0;
		int avail = 0;
		synchronized (dis) {
			try {
				while ((avail = dis.available()) == 0 && tot < internal_timeout)
				{
					dis.wait(i);
					tot += i++;
				}
			} catch (InterruptedException e) {
			}
		}
		return avail;
	}

	@Override
	public int read(final ByteBuffer b) throws IOException {
		final int rem = b.remaining();
		assert (rem > 0);
		if(!this.isOpen()) return -1;
		if (wait_available() == 0) return 0;
		while (b.hasRemaining()) {
			final int read = dis.read();
			if (read<0) return read;
			b.put((byte)read);
		}
		return rem-b.remaining();
	}

	@Override
	public int write(final ByteBuffer b) throws IOException {
		final int rem = b.remaining();
		assert (rem > 0);
		if(!this.isOpen()) return -1;
		while (b.hasRemaining())
			dos.write(b.get());
		final int left = b.remaining();
		if (left == 0) {
			dos.flush();
			return rem;
		}
		return rem - left;
	}
}
