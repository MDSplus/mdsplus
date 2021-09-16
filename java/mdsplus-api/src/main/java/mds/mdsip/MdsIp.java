package mds.mdsip;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.function.IntConsumer;
import java.util.regex.Matcher;
import java.util.stream.IntStream;

import debug.DEBUG;
import mds.*;
import mds.MdsException.MdsAbortException;
import mds.data.CTX;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.*;

public class MdsIp extends Mds
{
	public interface Connection extends ReadableByteChannel, WritableByteChannel
	{
		static final long internal_timeout = 1000L; // ms
	}

	public abstract static class MdsIpIOStream implements Connection
	{
		protected InputStream dis;
		protected OutputStream dos;

		@Override
		public int read(final ByteBuffer b) throws IOException
		{
			final int rem = b.remaining();
			if (!this.isOpen())
				return -1;
			if (this.wait_available() == 0)
				return 0;
			assert (rem > 0);
			if (b.hasArray())
			{
				final int read = this.dis.read(b.array(), b.arrayOffset() + b.position(), b.remaining());
				if (read <= 0)
				{
					if (read < 0)
						return read;
					return read;
				}
				if (read > 0)
					b.position(b.position() + read);
				return read;
			}
			final byte buf[] = new byte[Math.min(0x8000, b.remaining())];
			while (b.hasRemaining())
			{
				final int read = this.dis.read(buf);
				if (read == buf.length)
					b.put(buf);
				else
				{
					if (read < 0)
						return read;
					b.put(buf, 0, read);
					break;
				}
			}
			return rem - b.remaining();
		}

		private final int wait_available() throws IOException
		{
			int i = 1, tot = 0;
			int avail = 0;
			synchronized (this.dis)
			{
				try
				{
					while ((avail = this.dis.available()) == 0 && tot < Connection.internal_timeout)
					{
						this.dis.wait(i);
						tot += i++;
					}
				}
				catch (final InterruptedException e)
				{
					// abort
				}
			}
			return avail;
		}

		@Override
		public int write(final ByteBuffer b) throws IOException
		{
			final int rem = b.remaining();
			assert (rem > 0);
			if (!this.isOpen())
				return -1;
			if (b.hasArray())
			{
				this.dos.write(b.array(), b.arrayOffset() + b.position(), b.remaining());
				b.position(b.limit());
			}
			else
			{
				while (b.hasRemaining())
					this.dos.write(b.get());
			}
			final int left = b.remaining();
			if (left == 0)
			{
				this.dos.flush();
				return rem;
			}
			return rem - left;
		}
	}

	private final class MRT extends Thread // mds Receive Thread
	{
		private boolean killed = false;
		private Message message;

		public MRT()
		{
			super(MdsIp.this.getName("MRT"));
			this.setDaemon(true);
			this.message = null;
		}

		public Message getMessage() throws MdsAbortException
		{
			if (DEBUG.D)
				System.out.println("getMessage()");
			long time;
			if (DEBUG.D)
				time = System.nanoTime();
			Message msg = null;
			try
			{
				synchronized (this)
				{
					for (;;)
					{
						if (this.killed)
							return null;
						msg = this.message;
						this.message = null;
						if (msg != null)
							break;
						this.wait(1000);
					}
				}
			}
			catch (final InterruptedException e)
			{
				MdsIp.this.lostConnection();
				throw new MdsException.MdsAbortException();
			}
			if (DEBUG.D)
				if (msg != null)
				{
					final int cap = msg.getBody().capacity();
					System.out.println(msg.getMsgLen() + "B in " + (System.nanoTime() - time) / 1e9 + "sec"
							+ (cap == 0 ? "" : " (" + msg.asString().substring(0, (cap < 64) ? cap : 64) + ")"));
				}
			return msg;
		}

		@Override
		public final void run()
		{
			try
			{
				for (;;)
				{
					final Message new_message = Message.receive(MdsIp.this.connection, MdsIp.this.translisteners, -1);
					if (new_message.getDType() == DTYPE.EVENT)
					{
						final PMET PmdsEvent = new PMET();
						PmdsEvent.setEventid(new_message.getBody().get(12));
						PmdsEvent.start();
					}
					else
						synchronized (this)
						{
							this.message = new_message;
							this.notifyAll();
						}
				}
			}
			catch (final IOException e)
			{
				synchronized (this)
				{
					this.killed = true;
					this.notifyAll();
				}
				if (MdsIp.this.connected)
				{
					MdsIp.this.lostConnection();
				}
			}
		}

		synchronized public final void waitExited()
		{
			this.interrupt();
			while (!this.killed)
				try
				{
					this.wait(300);
				}
				catch (final InterruptedException exc)
				{
					System.err.println(this.getName() + ": isInterrupted");
				}
		}
	} // End MRT class

	private final class PMET extends Thread // Process mds Event Thread
	{
		int eventId = -1;
		String eventName;

		public PMET()
		{
			super(MdsIp.this.getName("PMET"));
			this.setDaemon(true);
		}

		@Override
		public void run()
		{
			if (this.eventName != null)
				MdsIp.this.dispatchUpdateEvent(this.eventName);
			else if (this.eventId != -1)
				MdsIp.this.dispatchUpdateEvent(this.eventId);
		}

		public void setEventid(final int id)
		{
			if (DEBUG.M)
				System.out.println("Received Event ID " + id);
			this.eventId = id;
			this.eventName = null;
		}
		/*
		 * public void setEventName(final String name) { if(DEBUG.M)
		 * System.out.println("Received Event Name " + name); this.eventId = -1;
		 * this.eventName = name; }
		 */
	} // end PMET class

	public final static class Provider
	{
		// Must ensure host field is never null
		private static final String DEFAULT_LOCAL = "local";
		private static final int DEFAULT_PORT = 8000;
		private static final String DEFAULT_USER = System.getProperty("user.name");
		private static final String PREFIX_LOCAL = "local://";
		private static final String PREFIX_SSH = "ssh://";
		private static final String PREFIX_FILE = "file://";
		private final String prefix;
		private final String host;
		private final int port;
		private final String user;

		public Provider()
		{
			this(null);// local
		}

		public Provider(final String provider)
		{
			this(provider, false);
		}

		public Provider(String provider, final boolean use_ssh)
		{
			if (provider == null || provider.isEmpty())
			{
				this.prefix = Provider.PREFIX_LOCAL;
				this.user = null;
				this.host = null;
				this.port = 0;
			}
			else if (provider.toLowerCase().startsWith(Provider.PREFIX_LOCAL))
			{
				this.prefix = Provider.PREFIX_LOCAL;
				this.user = null;
				this.host = provider.substring(Provider.PREFIX_LOCAL.length());
				this.port = 0;
			}
			else if (provider.toLowerCase().startsWith(Provider.PREFIX_FILE))
			{
				this.prefix = Provider.PREFIX_FILE;
				this.user = null;
				this.host = provider.substring(Provider.PREFIX_FILE.length());
				this.port = 0;
			}
			else
			{
				final boolean sshstr = provider.toLowerCase().startsWith(Provider.PREFIX_SSH);
				if (sshstr)
				{
					provider = provider.substring(Provider.PREFIX_SSH.length());
				}
				this.prefix = sshstr || use_ssh ? Provider.PREFIX_SSH : "";
				final int at = provider.indexOf("@");
				final int cn = provider.indexOf(":");
				this.user = at < 0 ? null : provider.substring(0, at);
				this.host = cn < 0 ? provider.substring(at + 1).toLowerCase()
						: provider.substring(at + 1, cn).toLowerCase();
				this.port = cn < 0 ? 0 : Short.parseShort(provider.substring(cn + 1));
			}
		}

		public Provider(final String host, final int port)
		{
			this(host, port, null, false);
		}

		public Provider(final String host, final int port, final String user)
		{
			this(host, port, user, user != null);
		}

		public Provider(String host, final int port, final String user, final boolean use_ssh)
		{
			if (host == null)
			{
				this.prefix = Provider.PREFIX_LOCAL;
				this.user = null;
				this.host = null;
				this.port = 0;
			}
			else
			{
				this.user = user;
				host = host.toLowerCase();
				if (host.startsWith(Provider.PREFIX_LOCAL))
				{
					host = host.substring(Provider.PREFIX_LOCAL.length());
					this.prefix = Provider.PREFIX_LOCAL;
				}
				else
				{
					final boolean sshstr = host.startsWith(Provider.PREFIX_SSH);
					if (sshstr)
					{
						host = host.substring(Provider.PREFIX_SSH.length());
					}
					this.prefix = sshstr || use_ssh ? Provider.PREFIX_SSH : "";
				}
				this.host = host;
				this.port = port;
			}
		}

		@Override
		public final boolean equals(final Object obj)
		{
			if (this == obj)
				return true;
			if (obj == null || !(obj instanceof Provider))
				return false;
			final Provider provider = (Provider) obj;
			return this.getPrefix().equals(provider.getPrefix()) && this.getHost().equals(provider.getHost())
					&& this.getPort() == provider.getPort() && this.getUser().equals(provider.getUser());
		}

		public final MdsIp getConnection()
		{
			return new MdsIp(this);
		}

		public String getHost()
		{
			return this.host == null || this.host.isEmpty() ? Provider.DEFAULT_LOCAL : this.host;
		}

		public int getPort()
		{
			return this.port != 0 ? this.port : ("".equals(this.prefix) ? 0 : Provider.DEFAULT_PORT);
		}

		public String getPrefix()
		{
			return this.prefix;
		}

		public final String getUser()
		{
			return this.user == null || this.user.isEmpty() ? Provider.DEFAULT_USER : this.user;
		}

		@Override
		public int hashCode()
		{
			return this.getPrefix().hashCode() ^ this.getUser().hashCode() ^ this.getHost().hashCode() ^ this.getPort();
		}

		@Override
		public final String toString()
		{
			final StringBuilder sb = new StringBuilder();
			sb.append(this.getPrefix());
			if (this.user != null)
				sb.append(this.user).append('@');
			sb.append(this.getHost());
			if (this.port != 0)
				sb.append(':').append(this.port);
			return sb.toString();
		}
	}

	public static final int LOGIN_OK = 1, LOGIN_ERROR = 2, LOGIN_CANCEL = 3;
	private static final byte MAX_MSGS = 8;
	private static final String NOT_CONNECTED = "Not Connected.";
	private static final Set<MdsIp> open_connections = Collections.synchronizedSet(new HashSet<MdsIp>());

	public static final boolean addSharedConnection(final MdsIp con)
	{
		synchronized (MdsIp.open_connections)
		{
			return MdsIp.open_connections.add(con);
		}
	}

	public static final int closeSharedConnections()
	{
		for (final MdsIp con : MdsIp.open_connections)
			con.close();
		final int size = MdsIp.open_connections.size();
		MdsIp.open_connections.clear();
		return size;
	}

	private final static StringBuilder getMessageNestExpr(final StringBuilder cmd, final Descriptor<?>[] args,
			final Request<?> req)
	{
		if (req.args != null && req.args.length > 0)
		{
			/** replace $ with deserializer for non-atomic arguments **/
			final boolean[] notatomic = new boolean[args.length];
			for (int i = 0; i < req.args.length; i++)
				if (req.args[i] == null)
					args[i] = Missing.NEW;
				else
				{
					notatomic[i] = !req.args[i].isAtomic();
					if (notatomic[i])
						args[i] = req.args[i].serializeDsc();
					else
						args[i] = req.args[i];
				}
			final Matcher m = Mds.dollar.matcher(req.expr);
			int pos = 0;
			for (int i = 0; i < args.length && m.find(); i++)
			{
				cmd.append(req.expr.substring(pos, m.start())).append(notatomic[i] ? "`__$si(($;))" : "($;)");
				pos = m.end();
			}
			cmd.append(req.expr.substring(pos));
		}
		else
			cmd.append(req.expr);
		return cmd;
	}

	private static final StringBuilder getMessageNestSerial(final StringBuilder cmd, final Descriptor<?>[] args,
			final Request<?> req, final boolean serialize)
	{
		if (!serialize)
			return MdsIp.getMessageNestExpr(cmd, args, req);
		cmd.append("__$so((");
		MdsIp.getMessageNestExpr(cmd, args, req);
		cmd.append(";))");
		return cmd;
	}

	public static final boolean removeSharedConnection(final MdsIp con)
	{
		synchronized (MdsIp.open_connections)
		{
			return MdsIp.open_connections.remove(con);
		}
	}

	public static MdsIp sharedConnection(final Provider provider)
	{
		synchronized (MdsIp.open_connections)
		{
			for (final MdsIp con : MdsIp.open_connections)
				if (con.provider.equals(provider))
				{
					con.connect();
					return con;
				}
			final MdsIp con = new MdsIp(provider);
			if (con.connect())
				MdsIp.open_connections.add(con);
			else
				con.close();
			return con;
		}
	}

	private boolean connected = false;
	private Connection connection = null;
	private boolean isLowLatency = false;
	private String last_error;
	private byte msg_id = 0;
	private final Object mutex = new Object();
	private final Provider provider;
	private MRT receiveThread = null;
	private boolean use_compression = false;

	public MdsIp()
	{
		this(new Provider(), null, null);
		this.connect();
	}

	public MdsIp(final Provider provider)
	{
		this(provider, null, null);
	}

	/** main constructor of the Connection class **/
	public MdsIp(final Provider provider, final ContextEventListener cl, final TransferEventListener tl)
	{
		this.addContextEventListener(cl);
		this.addTransferEventListener(tl);
		this.provider = provider;
	}

	public MdsIp(final String provider)
	{
		this(new Provider(provider));
	}

	public MdsIp(final String provider, final ContextEventListener cl)
	{
		this(new Provider(provider), cl, null);
	}

	public MdsIp(final String provider, final TransferEventListener tl)
	{
		this(new Provider(provider), null, tl);
	}

	@Override
	@SuppressWarnings(
	{ "rawtypes", "unchecked" })
	protected final <T extends Descriptor> T _getDescriptor(final CTX ctx, final Request<T> req) throws MdsException
	{
		if (ctx == null)
		{
			final boolean serialize = ((req.props & Request.PROP_ATOMIC_RESULT) == 0);
			final Message msg = this.getMessage(req, serialize);
			if (serialize)
			{
				if (msg.getDType() == DTYPE.T)
					throw new MdsException(msg.toString());
				return Mds.bufferToClass(msg.getBody(), req.cls);
			}
			return (T) new Int32(msg.asIntArray()[0]);
		}
		final StringBuilder sb = new StringBuilder().append("List(*,EXECUTE(($;)");
		IntStream.range(0, req.args.length).forEach(new IntConsumer()
		{
			@Override
			public void accept(final int i)
			{
				sb.append(",($;)");
			}
		});
		sb.append("),__$sw(`_$c_=__$sw(($;))))");
		final Vector<Descriptor<?>> vec = new Vector<>();
		vec.add(Descriptor.valueOf(req.expr));
		vec.addAll(Arrays.asList(req.args));
		vec.add(ctx.getDbid());
		final Request<List> nreq = new Request<>(List.class, sb.toString(), vec.toArray(req.args));
		if (DEBUG.N)
			System.err.println(">>> " + nreq);
		long tictoc;
		if (DEBUG.N)
			tictoc = System.currentTimeMillis();
		final Message msg = this.getMessage(nreq, true);
		if (DEBUG.N)
			System.err.println(String.format(">>> %.3f <<<",
					Float.valueOf(((float) (System.currentTimeMillis() - tictoc)) / 1000)));
		if (msg.getDType() == DTYPE.T)
		{
			if (DEBUG.N)
				System.err.println("<<< Exc: " + msg.toString());
			final Message ans = this.getMessage(new Request<>(Pointer.class, "__$sw(_$c_)"), false);
			ctx.getDbid().setAddress(ans.getBody());
			throw new MdsException(msg.toString());
		}
		final Descriptor<?> dsc = Mds.bufferToClass(msg.getBody(), nreq.cls);
		if (DEBUG.N)
			System.err.println("<<< " + dsc.toString());
		final Pointer nctx = (Pointer) ((List) dsc).get(1);
		ctx.getDbid().setAddress(nctx.getAddress());
		return (T) ((List) dsc).get(0);
	}

	/** disconnect from server and close **/
	@Override
	public void close()
	{
		this.disconnectFromServer();
	}

	/**
	 * re-/connects to the servers mdsip service. returns connected state.
	 */
	public boolean connect()
	{
		try
		{
			this.connectToServer();
		}
		catch (final IOException e)
		{
			this.last_error = "connect: " + e.getMessage();
			return false;
		}
		return this.connected;
	}

	synchronized private final void connectToServer() throws IOException
	{
		if (this.connected)
			return;
		// connect to server
		this.defined_funs.clear();
		final String prefix = this.provider.getPrefix();
		if (Provider.PREFIX_LOCAL.equals(prefix))
		{
			this.connection = new MdsIpFile("mdsip -P tunnel");
		}
		else if (Provider.PREFIX_SSH.equals(prefix))
		{
			this.connection = new MdsIpJsch(this.provider.user, this.provider.host, this.provider.getPort());
		}
		else if (Provider.PREFIX_FILE.equals(prefix))
		{
			this.connection = new MdsIpFile(this.provider.host);
		}
		else
		{
			this.connection = new MdsIpTcp(this.provider.host, this.provider.getPort());
		}
		if (DEBUG.D)
			System.out.println(this.connection.toString());
		// connect to mdsip
		final Message message = new Message(this.provider.getUser(), this.getMsgId());
		message.useCompression(this.use_compression);
		long tictoc = -System.nanoTime();
		message.send(this.connection);
		final Message msg = Message.receive(this.connection, null, 10_000);
		tictoc += System.nanoTime();
		if (DEBUG.N)
			System.out.println(tictoc);
		// if response is faster than 50ms
		this.isLowLatency = tictoc < 50_000_000;
		if (msg.getStatus() == 0)
		{
			this.close();
			throw new IOException("Server responded status == 0");
		}
		this.receiveThread = new MRT();
		this.connected = true;
		this.receiveThread.start();
		this.setup();
		this.dispatchContextEvent("Connected to " + this.provider.toString(), false);
	}

	private final void disconnectFromServer()
	{
		try
		{
			if (this.connection != null)
				this.connection.close();
		}
		catch (final IOException e)
		{
			System.err.println("The closing of connection failed:\n" + e.getMessage());
		}
		if (this.receiveThread != null)
			this.receiveThread.waitExited();
		this.connected = false;
	}

	protected final void dispatchContextEvent(final String msg, final boolean ok)
	{
		if (this.ctxlisteners != null)
			synchronized (this.ctxlisteners)
			{
				for (final ContextEventListener l : this.ctxlisteners)
					l.handleContextEvent(this, msg, ok);
			}
	}

	protected final void dispatchTransferEvent(final String info, final int pos, final int total)
	{
		if (this.translisteners != null)
			synchronized (this.translisteners)
			{
				for (final TransferEventListener l : this.translisteners)
					l.handleTransferEvent(this.connection, info, pos, total);
			}
	}

	@Override
	public final void execute(final String expr, final Descriptor<?>... args) throws MdsException
	{
		this.getMessage(new Request<>(Int32.class, expr + ";1", args), false);
	}

	@Override
	protected void finalize() throws Throwable
	{
		if (this.connected)
			System.err.println(this + " was still connected.");
		this.close();
	}

	private final Message getAnswer() throws MdsException
	{
		final Message message = this.receiveThread.getMessage();
		if (message == null)
			throw new MdsException("Null response from server", 0);
		final int status = message.getStatus();
		if ((status & 1) == 0 && status != 0 && message.getDType() == DTYPE.T)
		{
			final String msg = message.asString();
			throw new MdsException((msg == null || msg.isEmpty()) ? "<empty>" : msg, status);
		}
		return message;
	}

	public final String getLastError()
	{
		final String out = this.last_error;
		this.last_error = null;
		return out;
	}

	private final Message getMessage(final Request<?> req, final boolean serialize) throws MdsException
	{
		if (DEBUG.M)
			System.out.println("MdsIp.getMessage(\"" + req.expr + "\", " + req.args + ", " + serialize + ")");
		if (!this.connected)
			throw new MdsException(MdsIp.NOT_CONNECTED);
		final Message msg;
		byte idx = 0;
		final StringBuilder cmd = new StringBuilder(req.expr.length() + 128);
		final Descriptor<?>[] args = new Descriptor<?>[req.args.length];
		MdsIp.getMessageNestSerial(cmd, args, req, serialize);
		synchronized (this.mutex)
		{
			final byte mid = this.getMsgId();
			final byte totalarg = (byte) (args.length + 1);
			try
			{ // enter exclusive communication
				if (totalarg > 1)
				{ // execute main request
					this.sendArg(idx++, DTYPE.T, totalarg, null, cmd.toString(), mid);
					for (final Descriptor<?> d : args)
						d.toMessage(idx++, totalarg, mid).send(this.connection);
				}
				else
					new Message(cmd.toString(), mid).send(this.connection);
				this.dispatchTransferEvent("waiting for server", 0, 0);
				msg = this.getAnswer();
				if (msg == null)
					throw new MdsException("Could not get IO for " + this.provider.host, 0);
			}
			finally
			{
				this.dispatchTransferEvent(null, 0, 0);
			}
		}
		return msg;
	}

	private final byte getMsgId()
	{
		if (++this.msg_id >= MdsIp.MAX_MSGS)
			this.msg_id = 1;
		return this.msg_id;
	}

	private final String getName(final String classname)
	{
		if (this.isLocal())
			return Provider.DEFAULT_LOCAL;
		return new StringBuilder(128).append(classname).append('(').append(this.provider.toString()).append(')')
				.toString();
	}

	public final Provider getProvider()
	{
		return this.provider;
	}

	public final boolean isConnected()
	{
		return this.connected;
	}

	@Override
	public boolean isLowLatency()
	{
		return this.isLowLatency;
	}

	@Override
	public final String isReady()
	{
		if (this.isConnected())
			return null;
		return MdsIp.NOT_CONNECTED;
	}

	private void lostConnection()
	{
		this.disconnectFromServer();
		this.dispatchContextEvent(this.provider.host, false);
	}

	synchronized public final void mdsRemoveEvent(final UpdateEventListener l, final String event)
	{
		int eventid;
		if ((eventid = this.removeEvent(l, event)) == -1)
			return;
		final byte mid = this.getMsgId();
		try
		{
			this.sendArg((byte) 0, DTYPE.T, (byte) 2, null, Message.EVENTCANREQUEST, mid);
			this.sendArg((byte) 1, DTYPE.BU, (byte) 2, null, (byte) eventid, mid);
		}
		catch (final IOException e)
		{
			System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
		}
	}

	@Override
	protected synchronized final void mdsSetEvent(final String event, final int eventid)
	{
		final byte mid = this.getMsgId();
		try
		{
			this.sendArg((byte) 0, DTYPE.T, (byte) 3, null, Message.EVENTASTREQUEST, mid);
			this.sendArg((byte) 1, DTYPE.T, (byte) 3, null, event, mid);
			this.sendArg((byte) 2, DTYPE.BU, (byte) 3, null, (byte) eventid, mid);
		}
		catch (final IOException e)
		{
			System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
		}
	}

	public final void removeFromShare()
	{
		if (MdsIp.open_connections.contains(this))
			MdsIp.open_connections.remove(this);
	}

	private void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final byte bodyb,
			final byte mid) throws MdsException
	{
		final ByteBuffer body = ByteBuffer.allocate(1).order(Descriptor.BYTEORDER);
		body.put(0, (bodyb));
		this.sendArg(descr_idx, bu, nargs, dims, body, mid);
	}

	private final void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[],
			final ByteBuffer body, final byte msgid) throws MdsException
	{
		final Message msg = new Message(descr_idx, bu, nargs, dims, body, msgid);
		try
		{
			msg.send(this.connection);
		}
		catch (final IOException e)
		{
			this.lostConnection();
			throw new MdsException("MdsIp.sendArg", e);
		}
	}

	private final void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[],
			final String body, final byte msgid) throws MdsException
	{
		final ByteBuffer payload = StandardCharsets.UTF_8.encode(body).order(Descriptor.BYTEORDER);
		this.sendArg(descr_idx, bu, nargs, dims, payload, msgid);
	}

	private final void setup() throws MdsException
	{
		this.defineFunctions(//
				// compatible with almost every server
				"public fun __$sw(in _in){return(TreeShr->TreeSwitchDbid:P(val(_in)));}", //
				// for results; 'optional' prefix to support $Missing
				"public fun __$so(optional in _in){_out=*;MdsShr->MdsSerializeDscOut(xd(_in),xd(_out));return(_out);}", //
				// for arguments
				"public fun __$si(in _in){_out=*;MdsShr->MdsSerializeDscIn(ref(_in),xd(_out));return(_out);}", //
				// built-in list() will override this fall-back implementation
				"public fun list(optional inout _l,"
						+ "optional in _a0,optional in _a1,optional in _a2,optional in _a3,"
						+ "optional in _a4,optional in _a5,optional in _a6,optional in _a7,"
						+ "optional in _a8,optional in _a9,optional in _a10,optional in _a11,"
						+ "optional in _a12,optional in _a13,optional in _a14,optional in _a15){"
						+ "fun i2bu(in _i){return(execute('serializeout(`_i)[8:12]'));};"
						+ "fun append(inout _l,in _a){_l=[_l,_a];};"
						+ "fun cat(in _p,in _i){return(_p//execute('decompile(`_i)'));};"
						+ "fun listlen(optional in _l){_n=0;if(present('_l'))if(class(_l)==196)for(;if_error(kind(_l[_n]),-1)>0;_n++);return(_n);};"
						+ "_o=byte_unsigned([4,0,214,196,16,0,0,0,0,0,0,1]);" + "_nl=listlen(_l);"
						+ "for(_na=0;_na<16;_na++){if(not(execute('present(_a'//execute('decompile(`_na)')//')')))break;};"
						+ "_offset=4*(_na+_nl);" + "append(_o,i2bu(_offset));" + "_offset+=4;"
						+ "for(_i=0;_i<_nl;_i++){_offset+=size(execute('equals('//cat('_l',_i)//',serializeout(`_l[_i]))'));"
						+ "append(_o,i2bu(_offset));};"
						+ "for(_i=0;_i<_na;_i++){_offset+=size(execute('equals('//cat('_a',_i)//',serializeout(`'//cat('_a',_i)//'))'));"
						+ "append(_o,i2bu(_offset));};"
						+ "for(_i=0;_i<_nl;_i++)execute('append(_o,'//cat('_l',_i)//')');"
						+ "for(_i=0;_i<_na;_i++)execute('append(_o,'//cat('_a',_i)//')');"
						+ "return(_l=serializein(_o));}");
		this.getAPI().treeUsePrivateCtx(true);
	}

	@Override
	public final String toString()
	{
		if (Provider.PREFIX_LOCAL.equals(this.provider.getPrefix()))
		{
			return this.provider.getHost();
		}
		return this.provider.toString();
	}

	public final void useCompression(final boolean compression)
	{
		this.use_compression = compression;
	}
}
