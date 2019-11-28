package mds.mdsip;

import java.io.Closeable;
import java.io.EOFException;
import java.io.IOException;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import debug.DEBUG;
import mds.ContextEventListener;
import mds.Mds;
import mds.MdsException;
import mds.MdsException.MdsAbortException;
import mds.TransferEventListener;
import mds.UpdateEventListener;
import mds.data.CTX;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Pointer;

public class MdsIp extends Mds{


	public interface Connection extends Closeable, ReadableByteChannel, WritableByteChannel{}
	private final class MRT extends Thread // mds Receive Thread
	{
		private boolean	killed	= false;
		private Message	message;

		public MRT(){
			super(MdsIp.this.getName("MRT"));
			this.setDaemon(true);
			this.message = null;
		}

		public Message getMessage() throws MdsAbortException {
			if(DEBUG.D) System.out.println("getMessage()");
			long time;
			if(DEBUG.D) time = System.nanoTime();
			Message msg = null;
			try{
				synchronized(this){
					for(;;){
						if(this.killed) return null;
						msg = this.message;
						this.message = null;
						if(msg != null) break;
						this.wait(1000);
					}
				}
			}catch(final InterruptedException e){
				MdsIp.this.lostConnection();
				throw new MdsException.MdsAbortException();
			}
			if(DEBUG.D) if(msg != null){
				final int cap = msg.getBody().capacity();
				System.out.println(msg.getMsgLen() + "B in " + (System.nanoTime() - time) / 1e9 + "sec" + (cap == 0 ? "" : " (" + msg.asString().substring(0, (cap < 64) ? cap : 64) + ")"));
			}
			return msg;
		}

		@Override
		public final void run() {
			try{
				for(;;){
					final Message new_message = Message.receive(MdsIp.this.connection, MdsIp.this.translisteners);
					if(new_message.getDType() == DTYPE.EVENT){
						final PMET PmdsEvent = new PMET();
						PmdsEvent.setEventid(new_message.getBody().get(12));
						PmdsEvent.start();
					}else synchronized(this){
						this.message = new_message;
						this.notifyAll();
					}
				}
			}catch(final IOException e){
				synchronized(this){
					this.killed = true;
					this.notifyAll();
				}
				if(MdsIp.this.connected){
					MdsIp.this.lostConnection();
					if(!(e instanceof SocketException || e instanceof EOFException))// EOF if socket shuts down on abort
						e.printStackTrace();
				}
			}
		}

		synchronized public final void waitExited() {
			this.interrupt();
			while(!this.killed)
				try{
					this.wait(3000);
				}catch(final InterruptedException exc){
					System.err.println(this.getName() + ": isInterrupted");
				}
		}
	} // End MRT class
	private final class PMET extends Thread // Process mds Event Thread
	{
		int		eventId	= -1;
		String	eventName;

		public PMET(){
			super(MdsIp.this.getName("PMET"));
			this.setDaemon(true);
		}

		@Override
		public void run() {
			if(this.eventName != null) MdsIp.this.dispatchUpdateEvent(this.eventName);
			else if(this.eventId != -1) MdsIp.this.dispatchUpdateEvent(this.eventId);
		}

		public void setEventid(final int id) {
			if(DEBUG.M) System.out.println("Received Event ID " + id);
			this.eventId = id;
			this.eventName = null;
		}
		/*
		public void setEventName(final String name) {
			if(DEBUG.M) System.out.println("Received Event Name " + name);
			this.eventId = -1;
			this.eventName = name;
		}*/
	}// end PMET class
	public final static class Provider{
		private static final String DEFAULT_LOCAL   = "local";
		private static final int	DEFAULT_PORT	= 8000;
		private static final String	DEFAULT_USER	= System.getProperty("user.name");
		private static final String PREFIX_LOCAL	= "local://";
		private static final String PREFIX_SSH		= "ssh://";
		private final String		host;
		private final int			port;
		private boolean				use_local;
		private boolean				use_ssh;
		private final String		user;

		public Provider(final String provider){
			this(provider, false);
		}

		public Provider(String provider, final boolean use_ssh){
			if(provider == null || provider.length() == 0){
				this.user = Provider.DEFAULT_USER;
				this.host = Provider.DEFAULT_LOCAL;
				this.port = 0;
				this.use_ssh = false;
				this.use_local = true;
			}else{
				this.use_local = provider.toLowerCase().startsWith(PREFIX_LOCAL);
				if (this.use_local) {
					provider = provider.substring(8);
					this.use_ssh = false;
				} else {
					final boolean sshstr = provider.toLowerCase().startsWith(PREFIX_SSH);
					if(sshstr) provider = provider.substring(6);
					this.use_ssh = sshstr || use_ssh;
				}
				final int at = provider.indexOf("@");
				final int cn = provider.indexOf(":");
				this.user = at < 0 ? Provider.DEFAULT_USER : provider.substring(0, at);
				this.host = cn < 0 ? provider.substring(at + 1) : provider.substring(at + 1, cn);
				this.port = cn < 0 ? 0 : Short.parseShort(provider.substring(cn + 1));
			}
		}

		public Provider(final String host, final int port){
			this(host, port, Provider.DEFAULT_USER, false);
		}

		public Provider(final String host, final int port, final String user){
			this(host, port, user, user != null);
		}

		public Provider(String host, final int port, final String user, final boolean use_ssh){
			this.user = user;
			if(host == null){
				this.use_ssh = false;
				this.use_local = true;
			}else{
				this.use_local = host.toLowerCase().startsWith(PREFIX_LOCAL);
				if(this.use_local){
					this.use_ssh = false;
					host = host.substring(8);
				}else{
					final boolean sshstr = host.toLowerCase().startsWith("ssh://");
					if(sshstr){
						host = host.substring(6);
						this.use_ssh = true;
					}else this.use_ssh = use_ssh;
				}
			}
			this.host = host;
			this.port = port;
		}

		public Provider(){
			// local
			this(null);
		}

		@Override
		public final boolean equals(final Object obj) {
			if(this == obj) return true;
			if(obj == null || !(obj instanceof Provider)) return false;
			final Provider provider = (Provider)obj;
			return this.host.equalsIgnoreCase(provider.host) && this.port == provider.port && this.user.equals(provider.user) && this.use_ssh == provider.use_ssh;
		}

		public final MdsIp getConnection() {
			return new MdsIp(this);
		}

		public int getPort() {
			return this.port != 0 ? this.port : (this.use_ssh ? 22 : Provider.DEFAULT_PORT);
		}

		@Override
		public final int hashCode() {
			return this.host.toLowerCase().hashCode() + this.port;
		}

		@Override
		public final String toString() {
			if (this.use_local) return this.host;
			final StringBuilder sb = new StringBuilder(this.user.length() + this.host.length() + 16);
			if(this.use_ssh) sb.append("ssh://");
			sb.append(this.user).append('@').append(this.host);
			if(this.port != 0) sb.append(':').append(this.port);
			return sb.toString();
		}

		public boolean useLocal() {
			return use_local;
		}

		public final boolean useSSH() {
			return this.use_ssh;
		}

		public final void useSSH(final boolean usessh) {
			this.use_ssh = usessh;
		}
	}
	private static final Pattern	dollar				= Pattern.compile("\\$");
	public static final int			LOGIN_OK			= 1, LOGIN_ERROR = 2, LOGIN_CANCEL = 3;
	private static final byte		MAX_MSGS			= 8;
	private static final String		NOT_CONNECTED		= "Not Connected.";
	private static final Set<MdsIp>	open_connections	= Collections.synchronizedSet(new HashSet<MdsIp>());

	public static final boolean addSharedConnection(final MdsIp con) {
		synchronized(MdsIp.open_connections){
			return MdsIp.open_connections.add(con);
		}
	}

	public static final int closeSharedConnections() {
		for(final MdsIp con : MdsIp.open_connections)
			con.close();
		final int size = MdsIp.open_connections.size();
		MdsIp.open_connections.clear();
		return size;
	}

	private final static StringBuilder getMessageNestExpr(final StringBuilder cmd, final Descriptor<?>[] args, final Request<?> req) {
		if(req.args != null && req.args.length > 0){
			/** replace $ with deserializer for non-atomic arguments **/
			final boolean[] notatomic = new boolean[args.length];
			for(int i = 0; i < req.args.length; i++)
				if(req.args[i] == null) args[i] = Missing.NEW;
				else{
					notatomic[i] = !req.args[i].isAtomic();
					if(notatomic[i]) args[i] = req.args[i].serializeDsc();
					else args[i] = req.args[i];
				}
			final Matcher m = MdsIp.dollar.matcher(req.expr);
			int pos = 0;
			for(int i = 0; i < args.length && m.find(); i++){
				cmd.append(req.expr.substring(pos, m.start())).append(notatomic[i] ? "`__$si($)" : "$");
				pos = m.end();
			}
			cmd.append(req.expr.substring(pos));
		}else cmd.append(req.expr);
		return cmd;
	}

	private static final StringBuilder getMessageNestSerial(final StringBuilder cmd, final Descriptor<?>[] args, final Request<?> req, final boolean serialize) {
		if(!serialize) return MdsIp.getMessageNestExpr(cmd, args, req);
		cmd.append("__$so((");
		MdsIp.getMessageNestExpr(cmd, args, req);
		cmd.append(";))");
		return cmd;
	}

	public static final boolean removeSharedConnection(final MdsIp con) {
		synchronized(MdsIp.open_connections){
			return MdsIp.open_connections.remove(con);
		}
	}

	public static MdsIp sharedConnection(final Provider provider) {
		synchronized(MdsIp.open_connections){
			for(final MdsIp con : MdsIp.open_connections)
				if(con.provider.equals(provider)){
					con.provider.use_ssh = provider.use_ssh;
					con.connect();
					return con;
				}
			final MdsIp con = new MdsIp(provider);
			if(con.connect()) MdsIp.open_connections.add(con);
			else con.close();
			return con;
		}
	}
	private boolean			connected		= false;
	private Connection		connection		= null;
	private boolean			isLowLatency	= false;
	private String			last_error;
	private byte			msg_id			= 0;
	private final Object	mutex			= new Object();
	private final Provider	provider;
	private MRT				receiveThread	= null;
	private boolean			use_compression	= false;

	public MdsIp(){
		this(new Provider(), null, null);
		this.connect();
	}

	public MdsIp(final Provider provider){
		this(provider, null, null);
	}

	/** main constructor of the Connection class **/
	public MdsIp(final Provider provider, final ContextEventListener cl, final TransferEventListener tl){
		this.addContextEventListener(cl);
		this.addTransferEventListener(tl);
		this.provider = provider;
	}

	public MdsIp(final String provider){
		this(new Provider(provider));
	}

	public MdsIp(final String provider, final ContextEventListener cl){
		this(new Provider(provider), cl, null);
	}

	public MdsIp(final String provider, final TransferEventListener tl){
		this(new Provider(provider), null, tl);
	}

	@Override
	@SuppressWarnings({"rawtypes", "unchecked"})
	protected final <T extends Descriptor> T _getDescriptor(final CTX ctx, final Request<T> req) throws MdsException {
		if(ctx == null){
			final boolean serialize = ((req.props & Request.PROP_ATOMIC_RESULT) == 0);
			final Message msg = this.getMessage(req, serialize);
			if(serialize){
				if(msg.getDType() == DTYPE.T) throw new MdsException(msg.toString());
				return Mds.bufferToClass(msg.getBody(), req.cls);
			}
			return (T)new Int32(msg.asIntArray()[0]);
		}
		final StringBuilder sb = new StringBuilder().append("List(*,EXECUTE($");
		for(int i = 0; i < req.args.length; i++)
			sb.append(",$");
		sb.append("),__$sw(`_$c_=__$sw($)))");
		final Vector<Descriptor<?>> vec = new Vector<Descriptor<?>>();
		vec.add(Descriptor.valueOf(req.expr));
		vec.addAll(Arrays.asList(req.args));
		vec.add(ctx.getDbid());
		final Request<List> nreq = new Request<List>(List.class, sb.toString(), vec.toArray(req.args));
		if(DEBUG.N) System.err.println(">>> " + nreq);
		long tictoc;
		if(DEBUG.N) tictoc = System.currentTimeMillis();
		final Message msg = this.getMessage(nreq, true);
		if(DEBUG.N) System.err.println(String.format(">>> %.3f <<<", Float.valueOf(((float)(System.currentTimeMillis() - tictoc)) / 1000)));
		if(msg.getDType() == DTYPE.T){
			if(DEBUG.N) System.err.println("<<< Exc: " + msg.toString());
			final Message ans = this.getMessage(new Request<Pointer>(Pointer.class, "__$sw(_$c_)"), false);
			ctx.getDbid().setAddress(ans.getBody());
			throw new MdsException(msg.toString());
		}
		final Descriptor<?> dsc = Mds.bufferToClass(msg.getBody(), nreq.cls);
		if(DEBUG.N) System.err.println("<<< " + dsc.toString());
		final Pointer nctx = (Pointer)((List)dsc).get(1);
		ctx.getDbid().setAddress(nctx.getAddress());
		return (T)((List)dsc).get(0);
	}

	/** disconnect from server and close **/
	@Override
	public final boolean close() {
		this.disconnectFromServer();
		return true;
	}

	/** re-/connects to the servers mdsip service **/
	private final boolean connect() {
		if(this.connected) return true;
		try{
			this.connectToServer();
		}catch(final IOException e){
			this.last_error = "connect: " + e.getMessage();
			return false;
		}
		return this.connected;
	}

	synchronized private final void connectToServer() throws IOException {
		if(this.connected) return;
		/* connect to server */
		this.defined_funs.clear();
		if(this.provider.useLocal()){
			this.connection = new MdsIpTunnel();
		}else if(this.provider.useSSH()){
			this.connection = new MdsIpJsch(this.provider.user, this.provider.host, provider.port);
		}else{
			this.connection = new MdsIpTcp(this.provider.host, this.provider.getPort());
		}
		if(DEBUG.D) System.out.println(this.connection.toString());
		/* connect to mdsip */
		final Message message = new Message(this.provider.user, this.getMsgId());
		message.useCompression(this.use_compression);
		long tictoc = -System.nanoTime();
		message.send(this.connection);
		final Message msg = Message.receive(this.connection, null);
		tictoc += System.nanoTime();
		if(DEBUG.N) System.out.println(tictoc);
		this.isLowLatency = tictoc < 50000000;// if response is faster than 50ms
		if(msg.getStatus() == 0){
			this.close();
			throw new IOException("Server responded status == 0");
		}
		this.receiveThread = new MRT();
		this.connected = true;
		this.receiveThread.start();
		this.setup();
		this.dispatchContextEvent("Connected to " + this.provider.toString(), false);
	}

	private final void disconnectFromServer() {
		try{
			if(this.connection != null) this.connection.close();
		}catch(final IOException e){
			System.err.println("The closing of connection failed:\n" + e.getMessage());
		}
		if(this.receiveThread != null) this.receiveThread.waitExited();
		this.connected = false;
	}

	protected final void dispatchContextEvent(String msg, boolean ok) {
		if(this.ctxlisteners != null) synchronized(this.ctxlisteners){
			for(final ContextEventListener l : this.ctxlisteners)
				l.handleContextEvent(this, msg, ok);
		}
	}

	protected final void dispatchTransferEvent(final String info, int pos, int total) {
		if(this.translisteners != null) synchronized(this.translisteners){
			for(final TransferEventListener l : this.translisteners)
				l.handleTransferEvent(this.connection, info, pos, total);
		}
	}

	@Override
	public final void execute(final String expr, final Descriptor<?>... args) throws MdsException {
		this.getMessage(new Request<Int32>(Int32.class, expr + ";1", args), false);
	}

	@Override
	protected void finalize() throws Throwable {
		if(this.connected) System.err.println(this + " was still connected.");
		this.close();
	}

	private final Message getAnswer() throws MdsException {
		final Message message = this.receiveThread.getMessage();
		if(message == null) throw new MdsException("Null response from server", 0);
		final int status = message.getStatus();
		if((status & 1) == 0 && status != 0 && message.getDType() == DTYPE.T){
			final String msg = message.asString();
			throw new MdsException((msg == null || msg.isEmpty()) ? "<empty>" : msg, status);
		}
		return message;
	}

	public final String getLastError() {
		final String out = this.last_error;
		this.last_error = null;
		return out;
	}

	private final Message getMessage(final Request<?> req, final boolean serialize) throws MdsException {
		if(DEBUG.M) System.out.println("MdsIp.getMessage(\"" + req.expr + "\", " + req.args + ", " + serialize + ")");
		if(!this.connected) throw new MdsException(MdsIp.NOT_CONNECTED);
		final Message msg;
		byte idx = 0;
		final StringBuilder cmd = new StringBuilder(req.expr.length() + 128);
		final Descriptor<?>[] args = new Descriptor<?>[req.args.length];
		MdsIp.getMessageNestSerial(cmd, args, req, serialize);
		synchronized(this.mutex){
			final byte mid = this.getMsgId();
			final byte totalarg = (byte)(args.length + 1);
			/** enter exclusive communication **/
			try{
				if(totalarg > 1){ /** execute main request **/
					this.sendArg(idx++, DTYPE.T, totalarg, null, cmd.toString(), mid);
					for(final Descriptor<?> d : args)
						d.toMessage(idx++, totalarg, mid).send(this.connection);
				}else new Message(cmd.toString(), mid).send(this.connection);
				this.dispatchTransferEvent("waiting for server", 0, 0);
				msg = this.getAnswer();
				if(msg == null) throw new MdsException("Could not get IO for " + this.provider.host, 0);
			}finally{
				this.dispatchTransferEvent(null, 0, 0);
			}
		}
		return msg;
	}

	private final byte getMsgId() {
		if(++this.msg_id >= MdsIp.MAX_MSGS) this.msg_id = 1;
		return this.msg_id;
	}

	private final String getName(final String classname) {
		if (this.isLocal()) return Provider.DEFAULT_LOCAL;
		return new StringBuilder(128).append(classname).append('(').append(this.provider.toString()).append(')').toString();
	}

	public final Provider getProvider() {
		return this.provider;
	}

	public final boolean isConnected() {
		return this.connected;
	}

	@Override
	public boolean isLowLatency() {
		return this.isLowLatency;
	}

	@Override
	public final String isReady() {
		if(this.isConnected()) return null;
		return MdsIp.NOT_CONNECTED;
	}

	private void lostConnection() {
		this.disconnectFromServer();
		this.dispatchContextEvent(this.provider.host, false);
	}

	synchronized public final void mdsRemoveEvent(final UpdateEventListener l, final String event) {
		int eventid;
		if((eventid = this.removeEvent(l, event)) == -1) return;
		final byte mid = this.getMsgId();
		try{
			this.sendArg((byte)0, DTYPE.T, (byte)2, null, Message.EVENTCANREQUEST, mid);
			this.sendArg((byte)1, DTYPE.BU, (byte)2, null, (byte)eventid, mid);
		}catch(final IOException e){
			System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
		}
	}

	@Override
	protected synchronized final void mdsSetEvent(final String event, final int eventid) {
		final byte mid = this.getMsgId();
		try{
			this.sendArg((byte)0, DTYPE.T, (byte)3, null, Message.EVENTASTREQUEST, mid);
			this.sendArg((byte)1, DTYPE.T, (byte)3, null, event, mid);
			this.sendArg((byte)2, DTYPE.BU, (byte)3, null, (byte)eventid, mid);
		}catch(final IOException e){
			System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
		}
	}

	public final void removeFromShare() {
		if(MdsIp.open_connections.contains(this)) MdsIp.open_connections.remove(this);
	}

	private void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final byte bodyb, final byte mid) throws MdsException {
		ByteBuffer body = ByteBuffer.allocate(1).order(Descriptor.BYTEORDER);
		body.put(0, (bodyb));
		this.sendArg(descr_idx, bu, nargs, dims, body, mid);
	}

	private final void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final ByteBuffer body, final byte msgid) throws MdsException {
		final Message msg = new Message(descr_idx, bu, nargs, dims, body, msgid);
		try{
			msg.send(this.connection);
		}catch(final IOException e){
			this.lostConnection();
			throw new MdsException("MdsIp.sendArg", e);
		}
	}

	private final void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final String body, final byte msgid) throws MdsException {
		this.sendArg(descr_idx, bu, nargs, dims, StandardCharsets.UTF_8.encode(body).order(Descriptor.BYTEORDER), msgid);
	}

	private final void setup() throws MdsException {
		this.defineFunctions(//
				"public fun __$sw(in _in){return(TreeShr->TreeSwitchDbid:P(val(_in)));}", // compatible with almost every server
				"public fun __$so(optional in _in){_out=*;MdsShr->MdsSerializeDscOut(xd(_in),xd(_out));return(_out);}", // 'optional' to support $Missing
				"public fun __$si(in _in){_out=*;MdsShr->MdsSerializeDscIn(ref(_in),xd(_out));return(_out);}");
		this.getAPI().treeUsePrivateCtx(true);
	}

	@Override
	public final String toString() {
		if (this.provider.use_local) {
			return provider.host;
		}
		final String provider_str = this.provider.toString();
		return new StringBuilder(provider_str.length() + 12).append("MdsIp(").append(provider_str).append(")").toString();
	}
}