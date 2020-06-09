package mds.wavedisplay;

/* $Id$ */
public class DataServerItem
{
	private String name;
	private String argument;
	private String user;
	private String class_name;
	private final String browse_class;
	private final String browse_url;
	private String tunnel_port;

	public final String getName()
	{ return name; }

	public final String getArgument()
	{ return argument; }

	public final String getUser()
	{ return user; }

	public final String getClassName()
	{ return class_name; }

	public final String getBrowseUrl()
	{ return browse_url; }

	public final String getBrowseClass()
	{ return browse_class; }

	public final String getTunnelPort()
	{ return tunnel_port; }

	private static final String trimArg(final String arg)
	{
		if (arg == null)
			return null;
		final String trimmed = arg.trim();
		return trimmed.isEmpty() ? null : trimmed;
	}

	public DataServerItem(String user)
	{
		this(null, null, user, null, null, null, null);
	}

	public DataServerItem()
	{
		this(null, null, null, null, null, null, null);
	}

	public DataServerItem(String name, String argument, String user, String class_name, String browse_class,
			String browse_url, String tunnel_port)
	{
		this.name = trimArg(name);
		this.argument = trimArg(argument);
		this.user = trimArg(user);
		this.class_name = trimArg(class_name);
		this.browse_class = trimArg(browse_class);
		this.browse_url = trimArg(browse_url);
		this.tunnel_port = trimArg(tunnel_port);
	}

	@Override
	public String toString()
	{
		return name;
	}

	public boolean equals(DataServerItem dsi)
	{
		try
		{
			final boolean out = name.equals(dsi.name);
			// &&
			// argument.equals(dsi.argument) &&
			// class_name.equals(dsi.class_name) &&
			// browse_class.equals(dsi.browse_class);
			return out;
		}
		catch (final Exception exc)
		{
			return false;
		}
	}

	public boolean equals(String name)
	{
		return this.name.equals(name);
	}

	public void print()
	{
		System.out.println("name: " + this.name);
		System.out.println("argument: " + this.argument);
		System.out.println("user: " + this.user);
		System.out.println("class_name: " + this.getClassName());
		System.out.println("browse_class: " + this.name);
		System.out.println("tunnel_port: " + this.tunnel_port);
	}

	public void update(String name, String argument, String user, String class_name, String tunnel_port)
	{
		this.name = trimArg(name);
		this.argument = trimArg(argument);
		this.user = trimArg(user);
		this.class_name = trimArg(class_name);
		this.tunnel_port = trimArg(tunnel_port);
	}
}
