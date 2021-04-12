package mds.provider;

import java.io.IOException;
import java.util.StringTokenizer;

import javax.swing.JFrame;

import mds.wave.DataProvider;
import mds.wave.DataServerItem;

public class FtuDataProvider extends MdsDataProvider
{
	public FtuDataProvider()
	{
		super();
	}

	public FtuDataProvider(String provider) throws IOException
	{
		super(provider);
		setEnvironment("public _IMODE = 0;");
	}

	@Override
	public synchronized float[] GetFloatArray(String in) throws IOException
	{
		error = null;
		final float[] out_array = super.GetFloatArray(ParseExpression(in));
		if (out_array == null && error == null)
			error = "Cannot evaluate " + in + " for shot " + shot;
		if (out_array != null && out_array.length <= 1)
		{
			error = "Cannot evaluate " + in + " for shot " + shot;
			return null;
		}
		return out_array;
	}

	@Override
	public int[] getIntArray(String in) throws IOException
	{
		return super.getIntArray(ParseExpression(in));
	}

	@Override
	public int inquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	private String ParseExpression(String in)
	{
		final StringTokenizer st = new StringTokenizer(in, "\\", true);
		final StringBuffer parsed = new StringBuffer();
		int state = 0;
		try
		{
			while (st.hasMoreTokens())
			{
				final String curr_str = st.nextToken();
				// System.out.println("Token: "+curr_str);
				switch (state)
				{
				case 0:
					if (curr_str.equals("\\"))
						state = 1;
					else
						parsed.append(curr_str);
					break;
				case 1:
					if (curr_str.equals("\\"))
					{
						parsed.append("\\");
						state = 0;
					}
					else
					{
						if (curr_str.startsWith("$"))
							parsed.append("ftu(" + shot + ",\"_" + curr_str.substring(1));
						else
							parsed.append("ftu(" + shot + ",\"" + curr_str);
						state = 2;
					}
					break;
				case 2:
					if (!st.hasMoreTokens())
						parsed.append("\", _IMODE)");
					state = 3;
					break;
				case 3:
					if (!curr_str.equals("\\") || !st.hasMoreTokens())
					{
						parsed.append("\", _IMODE) " + curr_str);
						state = 0;
					}
					else
					{
						parsed.append("\\");
						state = 4;
					}
					break;
				case 4:
					if (curr_str.equals("\\"))
					{
						parsed.append("\\");
						state = 4;
					}
					else
					{
						parsed.append(curr_str);
						state = 2;
					}
					break;
				}
			}
		}
		catch (final Exception e)
		{
			System.out.println(e);
		}
		return parsed.toString();
	}
	/*
	 * private String getFirstSignal(String in_y) { if (in_y == null) return null;
	 * String curr_str; final StringTokenizer st = new StringTokenizer(in_y, "\\",
	 * true); while (st.hasMoreTokens()) { curr_str = st.nextToken(); if
	 * (curr_str.equals("\\") && st.hasMoreTokens()) return st.nextToken(); } return
	 * null; }
	 *
	 * protected String getDefaultTitle(String in_y) throws IOException { error =
	 * null; String first_sig = getFirstSignal(in_y); if (first_sig == null) return
	 * null; if (first_sig != null && first_sig.startsWith("$")) first_sig = "_" +
	 * first_sig.substring(1); final String parsed = "ftuyl(" + shot + ",\"" +
	 * first_sig + "\")"; return GetString(parsed, -1, -1, -1); }
	 *
	 * protected String getDefaultXLabel(String in_y) throws IOException { error =
	 * null; String first_sig = getFirstSignal(in_y); if (first_sig == null) return
	 * null; if (first_sig != null && first_sig.startsWith("$")) first_sig = "_" +
	 * first_sig.substring(1); final String parsed = "ftuxl(" + shot + ",\"" +
	 * first_sig + "\")"; return GetString(parsed, -1, -1, -1); }
	 */

	@Override
	public void setArgument(String arg) throws IOException
	{
		mds.setProvider(arg);
		setEnvironment("public _IMODE = 0;");
	}

	@Override
	public void SetCompression(boolean state)
	{}

	@Override
	public boolean SupportsCompression()
	{
		return false;
	}

	@Override
	public synchronized void update(String exp, long s)
	{
		error = null;
		shot = s;
	}
}
