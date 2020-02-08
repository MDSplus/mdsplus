package mds.mdsip;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.StandardSocketOptions;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import mds.mdsip.MdsIp.Connection;

public class MdsIpTcp implements Connection{
	private final SelectionKey select;
	private final SocketChannel socket;

	public MdsIpTcp(final String host, final int port) throws IOException{
		socket = SocketChannel.open();
		try {
			socket.configureBlocking(false);
			socket.setOption(StandardSocketOptions.SO_KEEPALIVE, Boolean.TRUE);
			this.select = socket.register(Selector.open(), SelectionKey.OP_CONNECT);
			socket.connect(new InetSocketAddress(host, port));
		} catch (IOException e) {
			socket.close();
			throw e;
		}
	}

	@Override
	final public void close() throws IOException {
		synchronized(select){
			select.cancel();
			select.channel().close();
			socket.close();
		}
	}

	final private boolean ensure_open() throws IOException {
		if(!select.isValid()) return false;
		while(select.interestOps() == SelectionKey.OP_CONNECT){
			if(select.selector().select(100L) > 0 && select.isConnectable()){
				synchronized(select){
					select.selector().selectedKeys().clear();
					if(!select.isValid()) return false;
					if(!((SocketChannel)select.channel()).finishConnect()) return false;
					select.interestOps(SelectionKey.OP_WRITE | SelectionKey.OP_READ);
				}
			}
		}
		return true;
	}

	@Override
	final public boolean isOpen() {
		return select.isValid();
	}

	@Override
	final public int read(ByteBuffer buffer) throws IOException {
		synchronized (select) {
			if(!this.ensure_open()) return -1;
			final int rem = buffer.remaining();
			if(rem == 0) return 0;
			if(select.selector().select(100L) >= 0 && select.isReadable()){
				final int read = ((SocketChannel)select.channel()).read(buffer);
				if(read == -1) return read;
			}
			return rem - buffer.remaining();
		}
	}

	@Override
	final public int write(final ByteBuffer buffer) throws IOException {
		synchronized (select) {
			if(!this.ensure_open()) return -1;
			final int rem = buffer.remaining();
			if(rem == 0) return 0;
			if(select.selector().select(100L) >= 0 && select.isWritable()){
				if(!select.isValid()) return -1;
				final int sent = ((SocketChannel)select.channel()).write(buffer);
				if(sent == -1) return sent;
			}
			return rem - buffer.remaining();
		}
	}
}