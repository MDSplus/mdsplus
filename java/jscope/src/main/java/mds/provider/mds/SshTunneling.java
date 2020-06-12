package mds.provider.mds;

import java.awt.Window;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;

import javax.swing.*;
import javax.swing.event.AncestorEvent;
import javax.swing.event.AncestorListener;

import com.jcraft.jsch.*;
import com.jcraft.jsch.ConfigRepository.Config;

public final class SshTunneling implements AutoCloseable
{
	public static final class UserInfo implements com.jcraft.jsch.UserInfo, UIKeyboardInteractive
	{
		static HashMap<String, String[]> keyboard_ans = new HashMap<>();
		static HashMap<String, UserInfo> keyboard_this = new HashMap<>();
		private final JTextField passphraseField = new JPasswordField(20);
		private final JTextField passwordField = new JPasswordField(20);
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
			{}

			@Override
			public void ancestorRemoved(final AncestorEvent arg0)
			{}
		};
		public boolean tried_pw = false;
		{
			this.passphraseField.addAncestorListener(this.RequestFocusListener);
			this.passwordField.addAncestorListener(this.RequestFocusListener);
		}

		@Override
		public final String getPassphrase()
		{ return this.passphraseField.getText(); }

		@Override
		public final String getPassword()
		{ return this.passwordField.getText(); }

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
				ob[i * 2 + 3] = echo[i] ? new JTextField(20) : new JPasswordField(20);
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
	private static final UserInfo userinfo;
	private static final File dotssh = new File(System.getProperty("user.home"), ".ssh");

	static private JSch jsch;

	static
	{
		JSch _jsch;
		try
		{
			_jsch = new JSch();
			final File known_hosts = new File(dotssh, "known_hosts");
			final File id_rsa = new File(dotssh, "id_rsa");
			if (!dotssh.exists())
				dotssh.mkdirs();
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
			if (id_rsa.exists())
				try
				{
					_jsch.addIdentity(id_rsa.getAbsolutePath());
				}
				catch (final JSchException e)
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
			_userinfo = new UserInfo();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
		userinfo = _userinfo;
	}

	private static final ConfigRepository getConfigRepository()
	{
		final File config = new File(dotssh, "config");
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

	static private SshTunneling parse(final String serverstring, ConfigRepository cr) throws JSchException
	{
		final int at = serverstring.indexOf("@");
		final int cn = serverstring.indexOf(":");
		final String user = at < 0 ? null : serverstring.substring(0, at);
		final String host = cn < 0 ? serverstring.substring(at + 1).toLowerCase()
				: serverstring.substring(at + 1, cn).toLowerCase();
		final int port = cn < 0 ? 0 : Integer.parseInt(serverstring.substring(cn + 1));
		return new SshTunneling(user, host, port, cr);
	}
	final Session session;
	final SshTunneling proxy;
	final String proxyjump;
	final String user;
	final String hostname;
	final Config config;

	final int port;

	private SshTunneling(final String user, final String host, final int port, final ConfigRepository cr)
			throws JSchException
	{
		config = cr.getConfig(host);
		final String hostname = config.getHostname();
		this.hostname = hostname != null ? hostname : host;
		if (user != null)
			this.user = user;
		else
		{
			final String cuser = config.getUser();
			this.user = cuser != null ? cuser : System.getProperty("user.name");
		}
		if (port != 0)
			this.port = port;
		else
		{
			final int pport = config.getPort();
			this.port = pport > 0 ? pport : 22;
		}
		this.proxyjump = config.getValue("ProxyJump");
		this.proxy = this.proxyjump == null ? null : parse(this.proxyjump, cr);
		session = this.connect(10_000); // timeout in ms
	}

	public SshTunneling(final String user, final String host, final int lport, final int rport) throws JSchException
	{
		this(user, host, 22, getConfigRepository());
		session.setPortForwardingL(lport, "127.0.0.1", rport);
	}

	@Override
	public void close()
	{
		session.disconnect();
		if (proxy != null)
			this.proxy.close();
	}

	private Session connect(int timeout) throws JSchException
	{
		Session session;
		if (this.proxy != null)
		{
			session = this.proxy.connect(timeout);
			final int pport = session.setPortForwardingL(0, this.hostname, this.port);
			session = jsch.getSession(this.user, "127.0.0.1", pport);
		}
		else
		{
			session = jsch.getSession(this.user, this.hostname, 22);
		}
		final String strictHostKeyChecking = config.getValue("StrictHostKeyChecking");
		if (strictHostKeyChecking != null)
			session.setConfig("StrictHostKeyChecking", strictHostKeyChecking);
		session.setUserInfo(userinfo);
		userinfo.tried_pw = false;
		session.connect(timeout);
		return session;
	}
}
