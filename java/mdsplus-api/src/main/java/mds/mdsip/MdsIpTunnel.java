package mds.mdsip;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import mds.mdsip.MdsIp.Connection;

public class MdsIpTunnel implements Connection{
	private Process process = null;

	@Override
	public void close() throws IOException {
		if(this.isOpen()) this.process.destroy();
	}

	public MdsIpTunnel() throws IOException {
		process = new ProcessBuilder("mdsip", "-P", "tunnel").start();
	}

	@Override
	public boolean isOpen() {
		return process != null && this.process.isAlive();
	}

	@SuppressWarnings("resource")
	@Override
	public int read(final ByteBuffer b) throws IOException {
		if(!this.isOpen()) return -1;
		final int rem = b.remaining();
		int read;
		InputStream is = this.process.getInputStream();
		while (b.hasRemaining()) {
			read = is.read();
			if (read<0) return read;
			b.put((byte)read);
		}
		return rem-b.remaining();
	}

	@SuppressWarnings("resource")
	@Override
	public int write(final ByteBuffer b) throws IOException {
		if(!this.isOpen()) return -1;
		final int rem = b.remaining();
		OutputStream os = this.process.getOutputStream();
		while (b.hasRemaining())
			os.write(b.get());
		final int left = b.remaining();
		if (left == 0) {
			os.flush();
			return rem;
		}
		return rem-left;
	}
}
