package mds.mdsip;

import java.awt.Window;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ConnectException;
import java.nio.ByteBuffer;
import java.util.HashMap;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.event.AncestorEvent;
import javax.swing.event.AncestorListener;
import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.OpenSSHConfig;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.UIKeyboardInteractive;
import mds.mdsip.MdsIp.Connection;

public final class MdsIpJsch implements Connection{
	private static class Logger implements com.jcraft.jsch.Logger{
		public Logger(){}

		@Override
		public boolean isEnabled(final int level) {
			return level >= com.jcraft.jsch.Logger.INFO;
		}

		@Override
		public void log(final int level, final String message) {
			final String type;
			switch(level){
				case com.jcraft.jsch.Logger.INFO:
					type = "INFO";
					break;
				case com.jcraft.jsch.Logger.ERROR:
					type = "ERROR";
					break;
				case com.jcraft.jsch.Logger.FATAL:
					type = "FATAL";
					break;
				case com.jcraft.jsch.Logger.WARN:
					type = "WARN";
					break;
				case com.jcraft.jsch.Logger.DEBUG:
					type = "DEBUG";
					break;
				default:
					type = "DEFAULT";
			}
			System.out.println(String.format("%s: %s", type, message));
		}
	}
	public static final class UserInfo implements com.jcraft.jsch.UserInfo, UIKeyboardInteractive{
		static HashMap<String, String[]>	keyboard_ans			= new HashMap<String, String[]>();
		static HashMap<String, UserInfo>	keyboard_this			= new HashMap<String, UserInfo>();
		private final JTextField			passphraseField			= new JPasswordField(20);
		private final JTextField			passwordField			= new JPasswordField(20);
		private final AncestorListener		RequestFocusListener	= new AncestorListener(){
			@Override
			public void ancestorAdded(final AncestorEvent e) {
				final JComponent component = ((JComponent)e.getSource());
				component.grabFocus();
				final Window win = (Window)component.getTopLevelAncestor();
				win.setAlwaysOnTop(true);
			}

			@Override
			public void ancestorMoved(final AncestorEvent arg0) {}

			@Override
			public void ancestorRemoved(final AncestorEvent arg0) {}
		};
		public boolean						tried_pw				= false;
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
			final UserInfo old = UserInfo.keyboard_this.putIfAbsent(destination, this);
			if(old != null && !this.equals(old)) return UserInfo.keyboard_ans.get(destination).clone();
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
			UserInfo.keyboard_ans.put(destination, ans.clone());
			UserInfo.keyboard_this.put(destination, this);
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
	private static final Object		jsch;
	private static final UserInfo	userinfo;
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
		UserInfo _userinfo = null;
		try{
			_userinfo = new MdsIpJsch.UserInfo();
		}catch(final Exception e){
			e.printStackTrace();
		}
		userinfo = _userinfo;
	}
	private final Channel		channel;
	private final InputStream	dis;
	private final OutputStream	dos;
	private final byte[]		buf;
	private final Session		session;

	public MdsIpJsch(String user, String host, int port) throws IOException{
		if(MdsIpJsch.jsch == null) throw new IOException("JSch not found! SSH connection not available.");
		try{
			session = ((JSch)MdsIpJsch.jsch).getSession(user, host, port);
			session.setUserInfo(userinfo);
			userinfo.tried_pw = false;
			if(debug.DEBUG.ON) JSch.setLogger(new MdsIpJsch.Logger());
			session.connect(10000);// timeout in ms
			channel = session.openChannel("exec");
			((ChannelExec)channel).setCommand("/bin/sh -l -c mdsip-server-ssh");
			channel.connect();
		}catch(final Exception e){
			if(e instanceof IOException) throw(IOException)e;
			throw new ConnectException(e.toString());
		}
		this.dis = this.channel.getInputStream();
		this.dos = this.channel.getOutputStream();
		this.buf = new byte[32*1024];
	}

	@Override
	synchronized public void close() throws IOException {
		try{
			try {
				this.dis.close();
			}finally {
				try {
					this.dos.close();
				}finally {
					try{
						this.channel.disconnect();
					}finally{
						this.session.disconnect();
					}
				}
			}
		}catch(final Exception e){
			if(e instanceof IOException) throw(IOException)e;
			throw new IOException(e.toString());
		}
	}

	@Override
	public boolean isOpen() {
		return !this.channel.isClosed() && this.channel.isConnected() && !this.channel.isEOF();
	}

	@Override
	public int read(ByteBuffer dat) throws IOException {
		final int rem = dat.remaining();
		assert (rem > 0);
		if (!this.isOpen()) return -1;
		int avail = dis.available();
		if (avail == 0) return 0;
		final int read = dis.read(buf, 0, (rem < buf.length) ? (rem < avail ? rem : avail) : (buf.length < avail ? buf.length : avail));
		if(read > 0) dat.put(buf, 0, read);
		return read;
	}

	@Override
	public int write(ByteBuffer dat) throws IOException {
		final int rem = dat.remaining();
		assert (rem > 0);
		if (!this.isOpen()) return -1;
		if(rem < buf.length){
			dat.get(buf, 0, rem);
			dos.write(buf, 0, rem);
			dos.flush();
			return rem;
		}else{
			dat.get(buf);
			dos.write(buf);
			if (!dat.hasRemaining()) dos.flush();
			return buf.length;
		}
	}
}
