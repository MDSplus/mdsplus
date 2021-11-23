package mds.mdsip;

import java.awt.Window;
import java.io.File;
import java.io.IOException;
import java.net.ConnectException;
import java.util.HashMap;
import java.util.Vector;

import javax.swing.*;
import javax.swing.event.AncestorEvent;
import javax.swing.event.AncestorListener;

import com.jcraft.jsch.*;

import mds.mdsip.MdsIp.Connection;
import mds.mdsip.MdsIp.MdsIpIOStream;

public final class MdsIpJsch extends MdsIpIOStream
{
	private static final int FIELD_LENGTH = 32;

	private static class Logger implements com.jcraft.jsch.Logger
	{
		public Logger()
		{
		}

		@Override
		public boolean isEnabled(final int level)
		{
			return level >= com.jcraft.jsch.Logger.INFO;
		}

		@Override
		public void log(final int level, final String message)
		{
			final String type;
			switch (level)
			{
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

	static private final class SshServerInfo
	{
		static public SshServerInfo parse(final String serverstring, final ConfigRepository cr)
		{
			final int at = serverstring.indexOf("@");
			final int cn = serverstring.indexOf(":");
			final String user = at < 0 ? null : serverstring.substring(0, at);
			final String host = cn < 0 ? serverstring.substring(at + 1).toLowerCase()
					: serverstring.substring(at + 1, cn).toLowerCase();
			final int port = cn < 0 ? 0 : Integer.parseInt(serverstring.substring(cn + 1));
			return new SshServerInfo(user, host, port, cr);
		}

		final SshServerInfo proxy;
		final String proxyjump;
		final String user;
		final String host;
		final ConfigRepository config;
		final int port;

		public SshServerInfo(final String user, final String host, final int port)
		{
			this(user, host,  port, MdsIpJsch.getConfigRepository());
		}

		private SshServerInfo(final String user, final String host, final int port, final ConfigRepository cr)
		{
			this.config = cr;
			this.host = host;
			this.user = user;
			this.port = port;
			this.proxyjump = this.config.getConfig(host).getValue("ProxyJump");
			this.proxy = this.proxyjump == null ? null : SshServerInfo.parse(this.proxyjump, cr);
		}

		public Vector<Session> connect(final int timeout) throws JSchException
		{

			final Vector<Session> sessions;
			final int pport;
			final String hostname;
			final String username;
			if (proxy != null)
			{
				hostname = "127.0.0.1";
				sessions = proxy.connect(timeout);
				username = config.getConfig(hostname).getUser();
				pport = sessions.firstElement().setPortForwardingL(0, hostname, port);
			}
			else
			{
				((JSch) MdsIpJsch.jsch).setConfigRepository(config);
				sessions = new Vector<>();
				username = user;
				hostname = null;
				pport = port;
			}
			Session session = ((JSch) MdsIpJsch.jsch).getSession(username, host);
			if (hostname != null)
				session.setHost(hostname);
			if (pport > 0)
				session.setPort(pport);
			sessions.insertElementAt(session, 0);
			session.setUserInfo(MdsIpJsch.userinfo);
			MdsIpJsch.userinfo.tried_pw = false;
			session.connect(timeout);
			return sessions;
		}
	}

	public static final class UserInfo implements com.jcraft.jsch.UserInfo, UIKeyboardInteractive
	{
		static HashMap<String, String[]> keyboard_ans = new HashMap<>();
		static HashMap<String, UserInfo> keyboard_this = new HashMap<>();
		private final JTextField passphraseField = new JPasswordField(FIELD_LENGTH);
		private final JTextField passwordField = new JPasswordField(FIELD_LENGTH);
		private final AncestorListener RequestFocusListener = new AncestorListener()
		{
			@Override
			public void ancestorAdded(final AncestorEvent e)
			{
				final JComponent component = ((JComponent) e.getSource());
				component.grabFocus();
				final Window win = (Window) component.getTopLevelAncestor();
				win.setAlwaysOnTop(true);
			}

			@Override
			public void ancestorMoved(final AncestorEvent arg0)
			{
				// noting to do
			}

			@Override
			public void ancestorRemoved(final AncestorEvent arg0)
			{
				// noting to do
			}
		};
		public boolean tried_pw = false;
		{
			this.passphraseField.addAncestorListener(this.RequestFocusListener);
			this.passwordField.addAncestorListener(this.RequestFocusListener);
		}

		@Override
		public final String getPassphrase()
		{
			return this.passphraseField.getText();
		}

		@Override
		public final String getPassword()
		{
			return this.passwordField.getText();
		}

		@Override
		public String[] promptKeyboardInteractive(final String destination, final String name, final String instruction,
				final String[] prompt, final boolean[] echo)
		{
			final UserInfo old = UserInfo.keyboard_this.putIfAbsent(destination, this);
			if (old != null && !this.equals(old))
				return UserInfo.keyboard_ans.get(destination).clone();
			final Object[] ob = new Object[prompt.length * 2 + 2];
			ob[0] = name;
			ob[1] = instruction;
			for (int i = 0; i < prompt.length; i++)
			{
				ob[i * 2 + 2] = prompt[i];
				ob[i * 2 + 3] = echo[i] ? new JTextField(FIELD_LENGTH) : new JPasswordField(FIELD_LENGTH);
			}
			if (prompt.length > 0)
				((JTextField) ob[3]).addAncestorListener(this.RequestFocusListener);
			final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, destination,
					JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
			if (result != JOptionPane.OK_OPTION)
				return null;
			final String[] ans = new String[prompt.length];
			for (int i = 0; i < prompt.length; i++)
				ans[i] = ((JTextField) ob[i * 2 + 3]).getText();
			UserInfo.keyboard_ans.put(destination, ans.clone());
			UserInfo.keyboard_this.put(destination, this);
			return ans;
		}

		@Override
		public final boolean promptPassphrase(final String message)
		{
			if (!this.passphraseField.getText().isEmpty() && !this.tried_pw)
			{
				this.tried_pw = true;
				return true;
			}
			final Object[] ob =
			{ message, this.passphraseField };
			final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, message,
					JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
			if (result == JOptionPane.OK_OPTION)
				return true;
			this.passphraseField.setText(null);
			return false;
		}

		@Override
		public final boolean promptPassword(final String message)
		{
			if (!this.passwordField.getText().isEmpty() && !this.tried_pw)
			{
				this.tried_pw = true;
				return true;
			}
			final Object[] ob =
			{ message, this.passwordField };
			final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, message,
					JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
			if (result == JOptionPane.OK_OPTION)
				return true;
			this.passwordField.setText(null);
			return false;
		}

		@Override
		public final boolean promptYesNo(final String str)
		{
			final Object[] options =
			{ "yes", "no" };
			final int foo = JOptionPane.showOptionDialog(JOptionPane.getRootFrame(), str, "Warning",
					JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[0]);
			return foo == 0;
		}

		@Override
		public final void showMessage(final String message)
		{
			JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), message);
		}
	}

	private static final Object jsch;
	private static final UserInfo userinfo;
	private static final File dotssh = new File(System.getProperty("user.home"), ".ssh");
	static
	{
		JSch _jsch;
		try
		{
			_jsch = new JSch();
			final File known_hosts = new File(MdsIpJsch.dotssh, "known_hosts");
			if (!MdsIpJsch.dotssh.exists())
				MdsIpJsch.dotssh.mkdirs();
			if (known_hosts.exists())
				try
				{
					_jsch.setKnownHosts(known_hosts.getAbsolutePath());
				}
				catch (final JSchException e)
				{
					e.printStackTrace();
				}
			else
				try
				{
					known_hosts.createNewFile();
				}
				catch (final IOException e)
				{
					e.printStackTrace();
				}
		}
		catch (final Exception e)
		{
			System.err.println("Error loading JSch, no ssh support!");
			_jsch = null;
		}
		jsch = _jsch;
		UserInfo _userinfo = null;
		try
		{
			_userinfo = new MdsIpJsch.UserInfo();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
		userinfo = _userinfo;
	}

	public static Connection fromString(final String string) throws IOException
	{
		final String usersplit[] = string.split("@", 2);
		final String user = usersplit.length == 1 ? null : usersplit[1];
		final String rest = usersplit.length == 1 ? usersplit[0] : usersplit[1];
		final String portsplit[] = rest.split(":", 2);
		final int port = portsplit.length == 1 ? 0 : Integer.parseInt(portsplit[1]);
		return new MdsIpJsch(user, portsplit[0], port);
	}

	private static final ConfigRepository getConfigRepository()
	{
		final File config = new File(MdsIpJsch.dotssh, "config");
		if (config.exists())
			try
			{
				return OpenSSHConfig.parseFile(config.getAbsolutePath());
			}
			catch (final IOException e)
			{
				e.printStackTrace();
			}
		return ConfigRepository.nullConfig;
	}

	private final Channel channel;
	private final Vector<Session> sessions;

	public MdsIpJsch(final String user, final String host, final int port) throws IOException
	{
		if (MdsIpJsch.jsch == null)
			throw new IOException("JSch not found! SSH connection not available.");
		try
		{
			if (debug.DEBUG.ON)
				JSch.setLogger(new MdsIpJsch.Logger());
			final SshServerInfo serverinfo = new SshServerInfo(user, host, port);
			this.sessions = serverinfo.connect(10_000); // timeout in ms
			this.channel = this.sessions.firstElement().openChannel("exec");
			((ChannelExec) this.channel).setCommand("/bin/sh -l -c mdsip-server-ssh");
			this.channel.connect();
		}
		catch (final Exception e)
		{
			if (e instanceof IOException)
				throw (IOException) e;
			throw new ConnectException(e.toString());
		}
		this.dis = this.channel.getInputStream();
		this.dos = this.channel.getOutputStream();
	}

	@Override
	synchronized public void close() throws IOException
	{
		try
		{
			try
			{
				this.dos.close();
			}
			finally
			{
				try
				{
					this.dis.close();
				}
				finally
				{
					try
					{
						this.channel.disconnect();
					}
					finally
					{
						for (final Session session : this.sessions)
							session.disconnect();
					}
				}
			}
		}
		catch (final Exception e)
		{
			if (e instanceof IOException)
				throw (IOException) e;
			throw new IOException(e.toString());
		}
	}

	public String getUser()
	{
		return this.sessions.firstElement().getUserName();
	}

	@Override
	public boolean isOpen()
	{
		return !this.channel.isClosed() && this.channel.isConnected() && !this.channel.isEOF();
	}
}
