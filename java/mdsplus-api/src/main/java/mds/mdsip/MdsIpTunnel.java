package mds.mdsip;

import java.io.IOException;
import java.nio.ByteBuffer;
import mds.mdsip.MdsIp.Connection;

public class MdsIpTunnel implements Connection{
	private Process process = null;

	@Override
	public void close() throws IOException {
		if(this.isOpen()) this.process.destroy();
	}

	private boolean ensure_open() throws IOException {
		if (this.isOpen()) return true;
		process = new ProcessBuilder("mdsip", "-P", "tunnel").start();
		return process.isAlive();
	}

	@Override
	public boolean isOpen() {
		return process != null && this.process.isAlive();
	}

	@Override
	public int read(final ByteBuffer b) throws IOException {
		if(!this.ensure_open()) return -1;
		final int rem = b.remaining();
		final int tord = rem < 1024 ? rem : 1024;
		byte a[] = new byte[tord];
		final int read = this.process.getInputStream().read(a);
		if(read > 0) b.put(a, 0, read);
		return read;
	}

	@Override
	public int write(final ByteBuffer b) throws IOException {
		if(!this.ensure_open()) return -1;
		final int rem = b.remaining();
		final int send = rem < 1024 ? rem : 1024;
		byte a[] = new byte[send];
		b.get(a);
		this.process.getOutputStream().write(a);
		if(rem == send) this.process.getOutputStream().flush();
		return send;
	}
}
