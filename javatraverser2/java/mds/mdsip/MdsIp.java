package mds.mdsip;

import java.awt.Window;
import java.io.Closeable;
import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.event.AncestorEvent;
import javax.swing.event.AncestorListener;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Logger;
import com.jcraft.jsch.OpenSSHConfig;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.UIKeyboardInteractive;
import com.jcraft.jsch.UserInfo;
import debug.DEBUG;
import mds.Mds;
import mds.MdsEvent;
import mds.MdsException;
import mds.MdsException.MdsAbortException;
import mds.MdsListener;
import mds.UpdateEventListener;
import mds.data.CTX;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Pointer;

public class MdsIp extends Mds{
    private final class MRT extends Thread // mds Receive Thread
    {
	private boolean killed = false;
	private Message message;

	public MRT(){
	    super(MdsIp.this.getName("MRT"));
	    this.setDaemon(true);
	    this.message = null;
	}

	public Message getMessage(final byte mid) throws MdsAbortException {
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
	    if(DEBUG.D) if(msg != null) System.out.println(msg.msglen + "B in " + (System.nanoTime() - time) / 1e9 + "sec" + (msg.body.capacity() == 0 ? "" : " (" + msg.asString().substring(0, (msg.body.capacity() < 64) ? msg.body.capacity() : 64) + ")"));
	    return msg;
	}

	@Override
	public final void run() {
	    try{
	        for(;;){
	            final Message new_message = Message.receive(MdsIp.this.dis, MdsIp.this.mdslisteners);
	            if(new_message.dtype == DTYPE.EVENT){
	                final PMET PmdsEvent = new PMET();
	                PmdsEvent.setEventid(new_message.body.get(12));
	                PmdsEvent.start();
	            }else{
	                synchronized(this){
	                    this.message = new_message;
	                    this.notifyAll();
	                }
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
    final class PMET extends Thread // Process mds Event Thread
    {
	int    eventId = -1;
	String eventName;

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

	public void setEventName(final String name) {
	    if(DEBUG.M) System.out.println("Received Event Name " + name);
	    this.eventId = -1;
	    this.eventName = name;
	}
    }// end PMET class
    public final static class Provider{
	public static final String DEFAULT_HOST = "localhost";
	public static final int    DEFAULT_PORT = 8000;
	public static final String DEFAULT_USER = System.getProperty("user.name");
	public final String        host;
	private final int          port;
	public final String        user;
	private boolean            use_ssh;
	private final String       password;

	public Provider(final String provider){
	    this(provider, false, null);
	}

	public Provider(final String provider, final boolean use_ssh){
	    this(provider, use_ssh, null);
	}

	private Provider(String provider, final boolean use_ssh, final String password){
	    final boolean sshstr = provider.toLowerCase().startsWith("ssh://");
	    if(sshstr) provider = provider.substring(6);
	    if(provider == null || provider.length() == 0){
	        this.user = Provider.DEFAULT_USER;
	        this.host = Provider.DEFAULT_HOST;
	        this.port = 0;
	    }else{
	        final int at = provider.indexOf("@");
	        final int cn = provider.indexOf(":");
	        this.user = at < 0 ? Provider.DEFAULT_USER : provider.substring(0, at);
	        this.host = cn < 0 ? provider.substring(at + 1) : provider.substring(at + 1, cn);
	        this.port = cn < 0 ? 0 : Short.parseShort(provider.substring(cn + 1));
	    }
	    this.use_ssh = sshstr || use_ssh;
	    this.password = password;
	}

	public Provider(final String host, final int port){
	    this(host, port, Provider.DEFAULT_USER, false, null);
	}

	public Provider(final String host, final int port, final String user, final boolean use_ssh){
	    this(host, port, user, use_ssh, null);
	}

	private Provider(final String host, final int port, final String user, final boolean use_ssh, final String password){
	    this.user = user;
	    this.host = host == null ? Provider.DEFAULT_HOST : host;
	    this.port = port;
	    this.use_ssh = use_ssh;
	    this.password = password;
	}

	public Provider(final String host, final int port, final String user, final String password){
	    this(host, port, user, password != null, password);
	}

	public Provider(final String provider, final String password){
	    this(provider, true, password);
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

	public final String getPassword() {
	    return this.password;
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
	    final StringBuilder sb = new StringBuilder(this.user.length() + this.host.length() + 16);
	    if(this.use_ssh) sb.append("ssh://");
	    sb.append(this.user).append('@').append(this.host);
	    if(this.port != 0) sb.append(':').append(this.port);
	    return sb.toString();
	}

	public final boolean useSSH() {
	    return this.use_ssh;
	}

	public final void useSSH(final boolean usessh) {
	    this.use_ssh = usessh;
	}
    }
    static class SshLogger implements Logger{
	public SshLogger(){}

	@Override
	public boolean isEnabled(final int level) {
	    return level >= Logger.INFO;
	}

	@Override
	public void log(final int level, final String message) {
	    final String type;
	    switch(level){
	        case Logger.INFO:
	            type = "INFO";
	            break;
	        case Logger.ERROR:
	            type = "ERROR";
	            break;
	        case Logger.FATAL:
	            type = "FATAL";
	            break;
	        case Logger.WARN:
	            type = "WARN";
	            break;
	        case Logger.DEBUG:
	            type = "DEBUG";
	            break;
	        default:
	            type = "DEFAULT";
	    }
	    System.out.println(String.format("%s: %s", type, message));
	}
    }
    private static final class SSHSocket implements Closeable{
	private static final class SshUserInfo implements UserInfo, UIKeyboardInteractive{
	    static HashMap<String, String[]>    keyboard_ans         = new HashMap<String, String[]>();
	    static HashMap<String, SshUserInfo> keyboard_this        = new HashMap<String, SshUserInfo>();
	    private final JTextField            passphraseField      = new JPasswordField(20);
	    private final JTextField            passwordField        = new JPasswordField(20);
	    public boolean                      tried_pw             = false;
	    private final AncestorListener      RequestFocusListener = new AncestorListener(){
	                                                                 @Override
	                                                                 public void ancestorAdded(final AncestorEvent e) {
	                                                                     final JComponent component = ((JComponent)e.getSource());
	                                                                     component.grabFocus();
	                                                                     final Window win = (Window)component.getTopLevelAncestor();
	                                                                     win.setAlwaysOnTop(true);
	                                                                 }

	                                                                 @Override
	                                                                 public void ancestorMoved(final AncestorEvent arg0) {/**/}

	                                                                 @Override
	                                                                 public void ancestorRemoved(final AncestorEvent arg0) {/**/}
	                                                             };
	    {
	        this.passphraseField.addAncestorListener(this.RequestFocusListener);
	        this.passwordField.addAncestorListener(this.RequestFocusListener);
	    }

	    @Override
	    public final String getPassphrase() {
	        return this.passphraseField.getText();
	    }

	    @Override
	    public final String getPassword() {
	        return this.passwordField.getText();
	    }

	    @Override
	    public String[] promptKeyboardInteractive(final String destination, final String name, final String instruction, final String[] prompt, final boolean[] echo) {
	        final SshUserInfo old = SshUserInfo.keyboard_this.putIfAbsent(destination, this);
	        if(old != null && !this.equals(old)) return SshUserInfo.keyboard_ans.get(destination).clone();
	        final Object[] ob = new Object[prompt.length * 2 + 2];
	        ob[0] = name;
	        ob[1] = instruction;
	        for(int i = 0; i < prompt.length; i++){
	            ob[i * 2 + 2] = prompt[i];
	            ob[i * 2 + 3] = echo[i] ? new JTextField(20) : new JPasswordField(20);
	        }
	        if(prompt.length > 0) ((JTextField)ob[3]).addAncestorListener(this.RequestFocusListener);
	        final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, destination, JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
	        if(result != JOptionPane.OK_OPTION) return null;
	        final String[] ans = new String[prompt.length];
	        for(int i = 0; i < prompt.length; i++)
	            ans[i] = ((JTextField)ob[i * 2 + 3]).getText();
	        SshUserInfo.keyboard_ans.put(destination, ans.clone());
	        SshUserInfo.keyboard_this.put(destination, this);
	        return ans;
	    }

	    @Override
	    public final boolean promptPassphrase(final String message) {
	        if(!this.passphraseField.getText().isEmpty() && !this.tried_pw){
	            this.tried_pw = true;
	            return true;
	        }
	        final Object[] ob = {message, this.passphraseField};
	        final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, message, JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
	        if(result == JOptionPane.OK_OPTION) return true;
	        this.passphraseField.setText(null);
	        return false;
	    }

	    @Override
	    public final boolean promptPassword(final String message) {
	        if(!this.passwordField.getText().isEmpty() && !this.tried_pw){
	            this.tried_pw = true;
	            return true;
	        }
	        final Object[] ob = {message, this.passwordField};
	        final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, message, JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
	        if(result == JOptionPane.OK_OPTION) return true;
	        this.passwordField.setText(null);
	        return false;
	    }

	    @Override
	    public final boolean promptYesNo(final String str) {
	        final Object[] options = {"yes", "no"};
	        final int foo = JOptionPane.showOptionDialog(JOptionPane.getRootFrame(), str, "Warning", JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[0]);
	        return foo == 0;
	    }

	    @Override
	    public final void showMessage(final String message) {
	        JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), message);
	    }
	}
	private static final Object jsch;
	static{
	    JSch _jsch;
	    try{
	        _jsch = new JSch();
	        final File dotssh = new File(System.getProperty("user.home"), ".ssh");
	        final File known_hosts = new File(dotssh, "known_hosts");
	        final File id_rsa = new File(dotssh, "id_rsa");
	        final File config = new File(dotssh, "config");
	        if(!dotssh.exists()) dotssh.mkdirs();
	        if(known_hosts.exists()) try{
	            _jsch.setKnownHosts(known_hosts.getAbsolutePath());
	        }catch(final JSchException e){
	            e.printStackTrace();
	        }
	        else try{
	            known_hosts.createNewFile();
	        }catch(final IOException e){
	            e.printStackTrace();
	        }
	        if(id_rsa.exists()) try{
	            _jsch.addIdentity(id_rsa.getAbsolutePath());
	        }catch(final JSchException e){
	            e.printStackTrace();
	        }
	        if(config.exists()) try{
	            _jsch.setConfigRepository(OpenSSHConfig.parseFile(config.getAbsolutePath()));
	        }catch(final IOException e){
	            e.printStackTrace();
	        }
	    }catch(final Exception e){
	        System.err.println("Error loading JSch, no ssh support!");
	        _jsch = null;
	    }
	    jsch = _jsch;
	}
	private final Session     session;
	private final ChannelExec channel;

	private SSHSocket(final Session session, final ChannelExec channel){
	    this.session = session;
	    this.channel = channel;
	}

	@Override
	synchronized public void close() throws IOException {
	    try{
	        try{
	            this.channel.disconnect();
	        }finally{
	            this.session.disconnect();
	        }
	    }catch(final Exception e){
	        if(e instanceof IOException) throw(IOException)e;
	        throw new IOException(e.toString());
	    }
	}
    }
    private static final Set<MdsIp> open_connections = Collections.synchronizedSet(new HashSet<MdsIp>());
    public static final int         LOGIN_OK         = 1, LOGIN_ERROR = 2, LOGIN_CANCEL = 3;
    private static final String     NOT_CONNECTED    = "Not Connected.";
    private static final byte       MAX_MSGS         = 8;
    private static final Pattern    dollar           = Pattern.compile("\\$");

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

    private static final SSHSocket newSSHSocket(final SSHSocket.SshUserInfo userinfo, final Provider provider) throws IOException {
	if(SSHSocket.jsch == null) throw new IOException("JSch not found! SSH connection not available.");
	try{
	    final Session session_out = ((JSch)SSHSocket.jsch).getSession(provider.user, provider.host, provider.getPort());
	    session_out.setUserInfo(userinfo);
	    session_out.setPassword(provider.getPassword());
	    userinfo.tried_pw = false;
	    if(debug.DEBUG.ON){
	        final SshLogger logger = new SshLogger();
	        JSch.setLogger(logger);
	    }
	    session_out.connect(10000);// timeout in ms
	    final ChannelExec channelExec = (ChannelExec)session_out.openChannel("exec");
	    channelExec.setCommand("/bin/sh -l -c mdsip-server-ssh");
	    channelExec.connect();
	    return new SSHSocket(session_out, channelExec);
	}catch(final Exception e){
	    if(e instanceof IOException) throw(IOException)e;
	    throw new ConnectException(e.toString());
	}
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
	            con.useSSH(provider.useSSH());
	            con.connect();
	            return con;
	        }
	    final MdsIp con = new MdsIp(provider);
	    if(con.connect()) MdsIp.open_connections.add(con);
	    else con.close();
	    return con;
	}
    }

    public static MdsIp sharedConnection(final String provider) {
	return MdsIp.sharedConnection(new Provider(provider));
    }

    public static MdsIp sharedConnection(final String provider, final boolean usessh) {
	return MdsIp.sharedConnection(new Provider(provider, usessh));
    }
    private boolean               connected       = false;
    private InputStream           dis             = null;
    private OutputStream          dos             = null;
    private final Provider        provider;
    private MRT                   receiveThread   = null;
    private Closeable             connection      = null;
    private boolean               use_compression = false;
    private final Object          mutex           = new Object();
    private SSHSocket.SshUserInfo userinfo;
    private boolean               isLowLatency    = false;
    private byte                  msg_id          = 0;
    public String                 last_error;

    public MdsIp(final Provider provider){
	this(provider, null);
    }

    /** main constructor of the Connection class **/
    public MdsIp(final Provider provider, final MdsListener cl){
	this.addMdsListener(cl);
	this.provider = provider;
    }

    public MdsIp(final String provider){
	this(new Provider(provider));
    }

    public MdsIp(final String provider, final MdsListener cl){
	this(new Provider(provider), cl);
    }

    @Override
    @SuppressWarnings({"rawtypes", "unchecked"})
    protected final <T extends Descriptor> T _getDescriptor(final CTX ctx, final Request<T> req) throws MdsException {
	if(ctx == null){
	    final boolean serialize = ((req.props & Request.PROP_ATOMIC_RESULT) == 0);
	    final Message msg = this.getMessage(req, serialize);
	    if(serialize){
	        if(msg.dtype == DTYPE.T) throw new MdsException(msg.toString());
	        return Mds.bufferToClass(msg.body, req.cls);
	    }
	    final ByteBuffer b = ByteBuffer.allocate(8 + msg.body.capacity());
	    b.putShort(msg.length);
	    b.put(msg.dtype.toByte());
	    return (T)new Int32(msg.asIntArray()[0]);
	}
	final StringBuilder sb = new StringBuilder().append("List(*,(");
	sb.append(req.expr).append(";),__$sw(`_$c_=__$sw($)))");
	final Vector<Descriptor<?>> vec = new Vector<Descriptor<?>>();
	vec.addAll(Arrays.asList(req.args));
	vec.add(ctx.getDbid());
	final Request<List> nreq = new Request<List>(List.class, sb.toString(), vec.toArray(req.args));
	if(DEBUG.N) System.err.println(">>> " + nreq);
	long tictoc;
	if(DEBUG.N) tictoc = System.currentTimeMillis();
	final Message msg = this.getMessage(nreq, true);
	if(DEBUG.N) System.err.println(String.format(">>> %.3f <<<", Float.valueOf(((float)(System.currentTimeMillis() - tictoc)) / 1000)));
	if(msg.dtype == DTYPE.T){
	    if(DEBUG.N) System.err.println("<<< Exc: " + msg.toString());
	    final Message ans = this.getMessage(new Request<Pointer>(Pointer.class, "__$sw(_$c_)"), false);
	    ctx.getDbid().setAddress(ans.body);
	    throw new MdsException(msg.toString());
	}
	final Descriptor<?> dsc = Mds.bufferToClass(msg.body, nreq.cls);
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
    public final boolean connect() {
	if(this.connected) return true;
	try{
	    this.connectToServer();
	}catch(final IOException e){
	    this.last_error = "connect: " + e.getMessage();
	    return false;
	}
	return this.connected;
    }

    /** re-/connects to the servers mdsip service **/
    public final boolean connect(final boolean use_compression_in) {
	this.use_compression = use_compression_in;
	return this.connect();
    }

    @SuppressWarnings("resource")
    synchronized private final void connectToServer() throws IOException {
	if(this.connected) return;
	/* connect to server */
	this.defined_funs.clear();
	if(this.provider.useSSH()){
	    if(this.userinfo == null) this.userinfo = new SSHSocket.SshUserInfo();
	    SSHSocket sshcon;
	    this.connection = sshcon = MdsIp.newSSHSocket(this.userinfo, this.provider);
	    this.dis = sshcon.channel.getInputStream();
	    this.dos = sshcon.channel.getOutputStream();
	}else{
	    Socket sock;
	    this.connection = sock = new Socket(this.provider.host, this.provider.getPort());
	    this.dis = sock.getInputStream();
	    this.dos = sock.getOutputStream();
	    sock.setTcpNoDelay(true);
	    sock.setSoTimeout(3000);
	}
	if(DEBUG.D) System.out.println(this.connection.toString());
	/* connect to mdsip */
	final Message message = new Message(this.provider.user, this.getMsgId());
	message.useCompression(this.use_compression);
	long tictoc = -System.nanoTime();
	message.send(this.dos);
	final Message msg = Message.receive(this.dis, null);
	tictoc += System.nanoTime();
	if(DEBUG.N) System.out.println(tictoc);
	this.isLowLatency = tictoc < 50000000;// if response is faster than 50ms
	if(this.connection instanceof Socket) ((Socket)this.connection).setSoTimeout(0);
	if(msg.header.get(4) == 0){
	    this.close();
	    return;
	}
	this.receiveThread = new MRT();
	this.connected = true;
	this.receiveThread.start();
	this.setup();
	MdsIp.this.dispatchMdsEvent(new MdsEvent(this, MdsEvent.HAVE_CONTEXT, "Connected to " + this.provider.toString()));
    }

    private final void disconnectFromServer() {
	try{
	    this.connected = false;
	    this.defined_funs.clear();
	    if(this.dos != null) this.dos.close();
	}catch(final IOException e){
	    System.err.println("The closing of data output stream failed:\n" + e.getMessage());
	}
	try{
	    if(this.dis != null) this.dis.close();
	}catch(final IOException e){
	    System.err.println("The closing of data input stream failed:\n" + e.getMessage());
	}
	try{
	    if(this.connection != null) this.connection.close();
	}catch(final IOException e){
	    System.err.println("The closing of socket failed:\n" + e.getMessage());
	}
	if(this.receiveThread != null) this.receiveThread.waitExited();
	this.connected = false;
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

    private final Message getAnswer(final byte mid) throws MdsException {
	final Message message = this.receiveThread.getMessage(mid);
	if(message == null) throw new MdsException("Null response from server", 0);
	if((message.status & 1) == 0 && message.status != 0 && message.dtype == DTYPE.T){
	    final String msg = message.asString();
	    throw new MdsException((msg == null || msg.isEmpty()) ? "<empty>" : msg, message.status);
	}
	return message;
    }

    public final String getHost() {
	return this.provider.host;
    }

    public final Message getMessage(final Request<?> req, final boolean serialize) throws MdsException {
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
	            this.sendArg(idx++, DTYPE.T, totalarg, null, cmd.toString().getBytes(), mid);
	            for(final Descriptor<?> d : args)
	                d.toMessage(idx++, totalarg, mid).send(this.dos);
	        }else new Message(cmd.toString(), mid).send(this.dos);
	        this.dispatchMdsEvent(new MdsEvent(this, MdsEvent.TRANSFER, "waiting for server"));
	        msg = this.getAnswer(mid);
	        if(msg == null) throw new MdsException("Could not get IO for " + this.provider.host, 0);
	    }finally{
	        this.dispatchMdsEvent(new MdsEvent(this));
	    }
	}
	return msg;
    }

    private final byte getMsgId() {
	if(++this.msg_id >= MdsIp.MAX_MSGS) this.msg_id = 1;
	return this.msg_id;
    }

    private final String getName(final String classname) {
	return new StringBuilder(128).append(classname).append('(').append(this.provider.toString()).append(')').toString();
    }

    public final int getPort() {
	return this.provider.getPort();
    }

    public final Provider getProvider() {
	return this.provider;
    }

    public final String getUser() {
	return this.provider.user;
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

    public void lostConnection() {
	this.disconnectFromServer();
	MdsIp.this.dispatchMdsEvent(new MdsEvent(MdsIp.this, MdsEvent.LOST_CONTEXT, "Lost connection from " + MdsIp.this.provider.host));
    }

    synchronized public final void mdsRemoveEvent(final UpdateEventListener l, final String event) {
	int eventid;
	if((eventid = this.removeEvent(l, event)) == -1) return;
	final byte mid = this.getMsgId();
	try{
	    this.sendArg((byte)0, DTYPE.T, (byte)2, null, Message.EVENTCANREQUEST.getBytes(), mid);
	    this.sendArg((byte)1, DTYPE.T, (byte)2, null, new byte[]{(byte)eventid}, mid);
	}catch(final IOException e){
	    System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
	}
    }

    @Override
    protected synchronized final void mdsSetEvent(final String event, final int eventid) {
	final byte mid = this.getMsgId();
	try{
	    this.sendArg((byte)0, DTYPE.T, (byte)3, null, Message.EVENTASTREQUEST.getBytes(), mid);
	    this.sendArg((byte)1, DTYPE.T, (byte)3, null, event.getBytes(), mid);
	    this.sendArg((byte)2, DTYPE.BU, (byte)3, null, new byte[]{(byte)(eventid)}, mid);
	}catch(final IOException e){
	    System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
	}
    }

    /*synchronized private void notifyTried() {
	this.notifyAll();
    }*/
    public final void removeFromShare() {
	if(MdsIp.open_connections.contains(this)) MdsIp.open_connections.remove(this);
    }

    private final void sendArg(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final byte body[], final byte msgid) throws MdsException {
	final Message msg = new Message(descr_idx, bu, nargs, dims, body, msgid);
	try{
	    msg.send(this.dos);
	}catch(final IOException e){
	    this.lostConnection();
	    throw new MdsException("MdsIp.sendArg", e);
	}
    }

    private final void setup() throws MdsException {
	this.defineFunctions("TreeShr->TreeUsePrivateCtx(val(1))", // always use private context
	        "public fun __$sw(in _in){return(TreeShr->TreeSwitchDbid:P(val(_in)));}", // compatible with almost ever server
	        "public fun __$so(optional in _in){_out=*;MdsShr->MdsSerializeDscOut(xd(_in),xd(_out));return(_out);}", // 'optional' to support $Missing
	        "public fun __$si(in _in){_out=*;MdsShr->MdsSerializeDscIn(ref(_in),xd(_out));return(_out);}");
	this.getAPI().treeUsePrivateCtx(true);
    }

    @Override
    public final String toString() {
	final String provider_str = this.provider.toString();
	return new StringBuilder(provider_str.length() + 12).append("MdsIp(").append(provider_str).append(")").toString();
    }

    public final void useSSH(final boolean usessh) {
	this.provider.useSSH(usessh);
    }
}