package mds.provider.browser;

import java.util.Properties;
import java.util.StringTokenizer;

import mds.jscope.jScopeBrowseSignals;

public class MdsBrowseSignals extends jScopeBrowseSignals
{
	private static final long serialVersionUID = 3L;
	String path;
	String shot;
	String tree;
	String server_url;

	@Override
	protected String getServerAddr()
	{ return server_url; }

	@Override
	protected String getShot()
	{ return shot; }

	@Override
	protected String getSignal(String url_name)
	{
		String sig_path = null;
		try
		{
			if (url_name != null)
			{
				String name;
				String value;
				int st_idx;
				final Properties pr = new Properties();
				if ((st_idx = url_name.indexOf("?")) != -1)
				{
					final String param = url_name.substring(st_idx + 1);
					final StringTokenizer st = new StringTokenizer(param, "&");
					name = st.nextToken("=");
					value = st.nextToken("&").substring(1);
					pr.put(name, value);
					name = st.nextToken("=").substring(1);
					value = st.nextToken("&").substring(1);
					pr.put(name, value);
					name = st.nextToken("=").substring(1);
					value = st.nextToken("&").substring(1);
					pr.put(name, value);
					tree = pr.getProperty("experiment");
					shot = pr.getProperty("shot");
					sig_path = pr.getProperty("path");
				}
			}
		}
		catch (final Exception exc)
		{
			sig_path = null;
		}
		return sig_path;
	}

	@Override
	protected String getTree()
	{ return tree; }
}
