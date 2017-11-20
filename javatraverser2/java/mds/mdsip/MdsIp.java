package mds.mdsip;

import java.awt.EventQueue;
import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.Session;
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
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Pointer;

public class MdsIp extends Mds{
    private final class MdsConnect extends Thread{
        private boolean close = false;
        private boolean tried = false;

        public MdsConnect(){
            super(MdsIp.this.getName("MdsConnect"));
            this.setDaemon(true);
        }

        synchronized public final void close() {
            this.close = true;
            this.notify();
        }

        synchronized public void retry() {
            this.tried = false;
            this.notify();
        }

        @Override
        public final void run() {
            try{
                for(;;)
                    try{
                        if(this.close) return;
                        MdsIp.this.connectToServer();
                        this.setTried(true);
                        synchronized(this){
                            if(this.close) return;
                            this.wait();
                        }
                    }catch(final IOException ce){
                        this.setTried(true);
                        try{
                            Thread.sleep(3000);
                        }catch(final InterruptedException ie){
                            System.err.println(this.getName() + ": isInterrupted1");
                        }
                    }
            }catch(final InterruptedException ie){
                System.err.println(this.getName() + ": isInterrupted2");
                this.close = true;
            }
            this.setTried(true);
        }

        synchronized private void setTried(final boolean tried) {
            this.tried = tried;
            if(tried) MdsIp.this.notifyTried();
        }

        synchronized public void update() {
            this.notifyAll();
        }
    }
    private final class MRT extends Thread // mds Receive Thread
    {
        private boolean               killed = false;
        private final Vector<Message> messages;

        public MRT(){
            super(MdsIp.this.getName("MRT"));
            this.setDaemon(true);
            this.messages = new Vector<Message>(MdsIp.MAX_MSGS);
            for(int i = 0; i < MdsIp.MAX_MSGS; i++)
                this.messages.addElement(null);
        }

        public Message getMessage(final byte mid) throws MdsAbortException {
            if(DEBUG.D) System.out.println("getMessage()");
            final int id = mid & 0xFF;
            long time;
            if(DEBUG.D) time = System.nanoTime();
            Message msg = null;
            try{
                for(;;)
                    synchronized(this){
                        this.wait(1000);
                        if(this.killed) return null;
                        synchronized(this.messages){
                            msg = this.messages.get(id);
                            this.messages.set(id, null);
                        }
                        if(msg == null) continue;
                        if(DEBUG.N) System.err.println(id + "v");
                        break;
                    }
            }catch(final InterruptedException e){
                try{
                    MdsIp.this.sock.shutdownInput();
                }catch(final IOException ioe){/**/}
                throw new MdsException.MdsAbortException();
            }
            if(DEBUG.D) System.out.println(msg.msglen + "B in " + (System.nanoTime() - time) / 1e9 + "sec" + (msg.body.capacity() == 0 ? "" : " (" + msg.asString().substring(0, (msg.body.capacity() < 64) ? msg.body.capacity() : 64) + ")"));
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
                            this.messages.set(new_message.message_id, new_message);
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
        public final int           port;
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
                this.port = Provider.DEFAULT_PORT;
            }else{
                final int at = provider.indexOf("@");
                final int cn = provider.indexOf(":");
                this.user = at < 0 ? Provider.DEFAULT_USER : provider.substring(0, at);
                this.host = cn < 0 ? provider.substring(at + 1) : provider.substring(at + 1, cn);
                this.port = cn < 0 ? Provider.DEFAULT_PORT : Short.parseShort(provider.substring(cn + 1));
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
            this.port = port == 0 ? Provider.DEFAULT_PORT : port;
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

        @Override
        public final int hashCode() {
            return this.host.toLowerCase().hashCode() + this.port;
        }

        @Override
        public final String toString() {
            final StringBuilder sb = new StringBuilder(this.user.length() + this.host.length() + (this.use_ssh ? 23 : 17));
            if(this.use_ssh) sb.append("ssh://");
            return sb.append(this.user).append('@').append(this.host).append(':').append(this.port).toString();
        }

        public final boolean useSSH() {
            return this.use_ssh;
        }

        public final void useSSH(final boolean usessh) {
            this.use_ssh = usessh;
        }
    }
    private static final class SSHSocket extends Socket{
        private static final class SshUserInfo implements UserInfo{
            private final JTextField passphraseField = new JPasswordField(20);
            private final JTextField passwordField   = new JPasswordField(20);
            public boolean           tried_pw        = false;

            @Override
            public final String getPassphrase() {
                return this.passphraseField.getText();
            }

            @Override
            public final String getPassword() {
                return this.passwordField.getText();
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
        private static final String localhost = "localhost";
        private static final Object jsch;
        static{
            JSch _jsch;
            try{
                _jsch = new JSch();
                final File dotssh = new File(System.getProperty("user.home"), ".ssh");
                final File known_hosts = new File(System.getProperty("user.home"), ".ssh/known_hosts");
                if(dotssh.exists()){
                    final File rsa = new File(System.getProperty("user.home"), ".ssh/id_rsa");
                    if(rsa.exists()) _jsch.addIdentity(rsa.getAbsolutePath());
                    if(!known_hosts.exists()) known_hosts.createNewFile();
                }else{
                    dotssh.mkdirs();
                    known_hosts.createNewFile();
                }
                _jsch.setKnownHosts(known_hosts.getAbsolutePath());
            }catch(final Exception e){
                System.err.println(e.getMessage());
                _jsch = null;
            }
            jsch = _jsch;
        }
        private final Session session;

        private SSHSocket(final Session session, final int local_port) throws IOException{
            super(SSHSocket.localhost, local_port);
            this.session = session;
        }

        @Override
        synchronized public void close() throws IOException {
            try{
                super.close();
            }finally{
                try{
                    this.session.disconnect();
                }catch(final Exception e){
                    if(e instanceof IOException) throw(IOException)e;
                    throw new IOException(e.toString());
                }
            }
        }
    }
    private static final List<MdsIp> open_connections = Collections.synchronizedList(new ArrayList<MdsIp>());
    public static final int          LOGIN_OK         = 1, LOGIN_ERROR = 2, LOGIN_CANCEL = 3;
    private static final String      NOT_CONNECTED    = "Not Connected.";
    private static final byte        MAX_MSGS         = 8;

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

    private static final StringBuilder getMessageNestCtx_new(final StringBuilder cmd, final Descriptor<?>[] args, final Pointer ctx, final Request<?> req, final boolean serialize) {
        if(ctx == null) return MdsIp.getMessageNestSerial(cmd, args, req, serialize);
        cmd.append("_ctx=TreeShr->TreeSwitchDbid:P(val($));");
        MdsIp.getMessageNestSerial(cmd, args, req, serialize);
        return cmd;
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
            final Matcher m = Pattern.compile("\\$").matcher(req.expr);
            int pos = 0;
            for(int i = 0; i < args.length && m.find(); i++){
                cmd.append(req.expr.substring(pos, m.start())).append(notatomic[i] ? Mds.serialStr : "$");
                pos = m.end();
            }
            cmd.append(req.expr.substring(pos));
        }else cmd.append(req.expr);
        return cmd;
    }

    private static final StringBuilder getMessageNestSerial(final StringBuilder cmd, final Descriptor<?>[] args, final Request<?> req, final boolean serialize) {
        if(!serialize) return MdsIp.getMessageNestExpr(cmd, args, req);
        cmd.append("_ans=*;MdsShr->MdsSerializeDscOut(xd((");
        MdsIp.getMessageNestExpr(cmd, args, req);
        cmd.append(";)),xd(_ans));_ans");
        return cmd;
    }

    private static final SSHSocket newSSHSocket(final SSHSocket.SshUserInfo userinfo, final Provider provider) throws IOException {
        if(SSHSocket.jsch == null) throw new IOException("JSch not found! SSH connection not available.");
        try{
            final Session session_out = ((JSch)SSHSocket.jsch).getSession(provider.user, provider.host);
            session_out.setUserInfo(userinfo);
            session_out.setPassword(provider.getPassword());
            userinfo.tried_pw = false;
            session_out.connect();
            final int localport = session_out.setPortForwardingL(0, SSHSocket.localhost, provider.port);
            return new SSHSocket(session_out, localport);
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
    private MdsConnect            connectThread   = null;
    private InputStream           dis             = null;
    private OutputStream          dos             = null;
    private final Provider        provider;
    private MRT                   receiveThread   = null;
    private Socket                sock            = null;
    private boolean               use_compression = true;
    private final Object          mutex           = new Object();
    private SSHSocket.SshUserInfo userinfo;
    private boolean               isLowLatency    = false;
    private byte                  msg_id          = 0;

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
    @SuppressWarnings({"rawtypes"})
    protected final <T extends Descriptor> T _getDescriptor(final CTX ctx, final Request<T> req) throws MdsException {
        final boolean serialize = (req.props & Request.PROP_ATOMIC_RESULT) == 0;
        final Message msg = this.getMessage(ctx == null ? null : ctx.getDbid(), req, serialize);
        if(msg.dtype == DTYPE.T) throw new MdsException(msg.toString());
        if(serialize) return Mds.bufferToClass(msg.body, req.cls);
        return null; // TODO: deserialize from msg Discriptor_A.readMessage()
    }

    /** disconnect from server and close **/
    @Override
    public final boolean close() {
        if(this.connectThread != null) this.connectThread.close();
        this.connectThread = null;
        this.disconnectFromServer();
        if(this.receiveThread != null) this.receiveThread.waitExited();
        this.receiveThread = null;
        this.dos = null;
        this.dis = null;
        return true;
    }

    /** re-/connects to the servers mdsip service **/
    public final boolean connect() {
        if(this.connected) return true;
        if(this.connectThread == null || !this.connectThread.isAlive()){
            this.connectThread = new MdsConnect();
            this.connectThread.start();
        }
        this.connectThread.retry();
        if(!Thread.currentThread().getName().startsWith("AWT-EventQueue-")) this.waitTried();
        return this.connected;
    }

    /** re-/connects to the servers mdsip service **/
    public final boolean connect(final boolean use_compression_in) {
        this.use_compression = use_compression_in;
        return this.connect();
    }

    synchronized private final void connectToServer() throws IOException {
        /* connect to server */
        if(this.provider.useSSH()){
            if(this.userinfo == null) this.userinfo = new SSHSocket.SshUserInfo();
            this.sock = MdsIp.newSSHSocket(this.userinfo, this.provider);
        }else this.sock = new Socket(this.provider.host, this.provider.port);
        System.out.println(this.sock.toString());
        this.sock.setTcpNoDelay(true);
        this.dis = this.sock.getInputStream();
        this.dos = this.sock.getOutputStream();
        /* connect to mdsip */
        final Message message = new Message(this.provider.user, this.getMsgId());
        message.useCompression(this.use_compression);
        this.sock.setSoTimeout(3000);
        long tick = -System.nanoTime();
        message.send(this.dos);
        final Message msg = Message.receive(this.dis, null);
        tick += System.nanoTime();
        if(DEBUG.N) System.out.println(tick);
        this.isLowLatency = tick < 50000000;// if response is faster than 50ms
        this.sock.setSoTimeout(0);
        if(msg.header.get(4) == 0){
            this.close();
            return;
        }
        this.receiveThread = new MRT();
        this.receiveThread.start();
        this.connected = true;
        MdsIp.this.dispatchMdsEvent(new MdsEvent(this, MdsEvent.HAVE_CONTEXT, "Connected to " + this.provider.toString()));
    }

    private final void disconnectFromServer() {
        try{
            try{
                if(this.dos != null) this.dos.close();
            }finally{
                if(this.dis != null) this.dis.close();
            }
            if(this.sock != null) this.sock.close();
            if(this.receiveThread != null) this.receiveThread.waitExited();
        }catch(final IOException e){
            System.err.println("The closing of sockets failed:\n" + e.getMessage());
        }
        this.connected = false;
    }

    @Override
    protected void finalize() throws Throwable {
        try{
            this.close();
        }finally{
            super.finalize();
        }
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

    public final Message getMessage(final Pointer ctx, final Request<?> req, final boolean serialize) throws MdsException {
        if(DEBUG.M) System.out.println("MdsIp.getMessage(" + ctx + ", \"" + req.expr + "\", " + req.args + ", " + serialize + ")");
        if(!this.connected) throw new MdsException(MdsIp.NOT_CONNECTED);
        final Message msg;
        byte idx = 0;
        final StringBuilder cmd = new StringBuilder(req.expr.length() + 128);
        final Descriptor<?>[] args = new Descriptor<?>[req.args.length];
        MdsIp.getMessageNestCtx_new(cmd, args, ctx, req, serialize);
        if(DEBUG.N) System.out.println("Execute(\"" + cmd + "\"," + Arrays.toString(args));
        synchronized(this.mutex){
            long tick;
            if(DEBUG.N) tick = -System.nanoTime();
            final byte mid = this.getMsgId();
            final byte midctx = ctx == null ? 0 : this.getMsgId();
            /** enter exclusive communication **/
            try{
                /** execute main request **/
                final byte totalarg = (byte)(args.length + (ctx == null ? 1 : 2));
                if(totalarg > 1){
                    this.sendArg(idx++, DTYPE.T, totalarg, null, cmd.toString().getBytes(), mid);
                    if(ctx != null) ctx.toMessage(idx++, totalarg, mid).send(this.dos);
                    for(final Descriptor<?> d : args)
                        d.toMessage(idx++, totalarg, mid).send(this.dos);
                }else new Message(cmd.toString(), mid).send(this.dos);
                msg = this.getAnswer(mid);
            }finally{
                /** save new tree context if ctx was provided **/
                if(ctx != null){
                    try{
                        new Message("TreeShr->TreeDbid:P()", midctx).send(this.dos);
                        final Message ctx_msg = this.getAnswer(midctx);
                        ctx.setAddress(ctx_msg.body);
                    }catch(final IOException e){
                        ctx.setAddress(0);
                    }
                }
            }
            if(DEBUG.N) System.out.println(tick + System.nanoTime());
        }
        if(msg == null) throw new MdsException("Could not get IO for " + this.provider.host, 0);
        return msg;
    }

    public final Message getMessage(final Request<?> req, final boolean serialize) throws MdsException {
        return this.getMessage(null, req, serialize);
    }

    private final byte getMsgId() {
        if(++this.msg_id >= MdsIp.MAX_MSGS) this.msg_id = 1;
        return this.msg_id;
    }

    private final String getName(final String classname) {
        return new StringBuilder(128).append(classname).append('(').append(this.provider.user).append('@').append(this.provider.host).append(':').append(this.provider.port).append(')').toString();
    }

    public final int getPort() {
        return this.provider.port;
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
        this.waitTried();
        if(!this.isConnected()) return MdsIp.NOT_CONNECTED;
        return null;
    }

    public void lostConnection() {
        this.connected = false;
        this.disconnectFromServer();
        // (new Thread(){ @Override public void run() {} }).start();
        MdsIp.this.dispatchMdsEvent(new MdsEvent(MdsIp.this, MdsEvent.LOST_CONTEXT, "Lost connection from " + MdsIp.this.provider.host));
        if(this.connectThread != null && this.connectThread.tried) this.connectThread.update();
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
    synchronized protected final void mdsSetEvent(final String event, final int eventid) {
        final byte mid = this.getMsgId();
        try{
            this.sendArg((byte)0, DTYPE.T, (byte)3, null, Message.EVENTASTREQUEST.getBytes(), mid);
            this.sendArg((byte)1, DTYPE.T, (byte)3, null, event.getBytes(), mid);
            this.sendArg((byte)2, DTYPE.BU, (byte)3, null, new byte[]{(byte)(eventid)}, mid);
        }catch(final IOException e){
            System.err.print("Could not get IO for " + this.provider.host + ":\n" + e.getMessage());
        }
    }

    synchronized private void notifyTried() {
        this.notifyAll();
    }

    public final void removeFromShare() {
        if(MdsIp.open_connections.contains(this)) MdsIp.open_connections.remove(this);
    }

    private final void sendArg(final byte descr_idx, final byte dtype, final byte nargs, final int dims[], final byte body[], final byte msgid) throws MdsException {
        final Message msg = new Message(descr_idx, dtype, nargs, dims, body, msgid);
        try{
            msg.send(this.dos);
        }catch(final IOException e){
            this.lostConnection();
            throw new MdsException("MdsIp.sendArg", e);
        }
    }

    @Override
    public final String toString() {
        final String provider_str = this.provider.toString();
        return new StringBuilder(provider_str.length() + 12).append("MdsIp(").append(provider_str).append(")").toString();
    }

    public final void useSSH(final boolean usessh) {
        this.provider.useSSH(usessh);
    }

    synchronized private void waitTried() {
        if(this.connectThread == null) return;
        if(!this.connectThread.tried) try{
            if(EventQueue.isDispatchThread()) Thread.sleep(5000);
            else this.wait();
        }catch(final InterruptedException e){
            Thread.currentThread().interrupt();
        }
    }
}