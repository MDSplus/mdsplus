package mds.provider;

import java.io.IOException;
import java.util.StringTokenizer;

import javax.swing.JFrame;

import mds.wave.DataProvider;
import mds.wave.DataServerItem;

public class JetMdsDataProvider extends MdsDataProvider
{
	public JetMdsDataProvider()
	{
		super("mdsplus.jet.efda.org");
	}

	@Override
	public void SetArgument(String arg)
	{}

	@Override
	public synchronized void Update(String exp, long s)
	{
		error = null;
		shot = s;
	}

	@Override
	public synchronized int[] GetIntArray(String in) throws IOException
	{
		return super.GetIntArray(ParseExpression(in));
	}

	// Syntax: ppf/<signal> or jpf/<signal>
	// Ex: ppf/magn/ipla
	protected String ParseExpression(String in)
	{
		error = null;
		final StringTokenizer st = new StringTokenizer(in, " /(){}[]*+,:;", true);
		String parsed = "", signal = "";
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
					if (curr_str.equals("/"))
					{
						if (parsed.endsWith("PPF") || parsed.endsWith("ppf") || parsed.endsWith("JPF")
								|| parsed.endsWith("jpf"))
						{
							signal = parsed.substring(parsed.length() - 3) + "/";
							parsed = parsed.substring(0, parsed.length() - 3);
							state = 1;
						}
						else
							parsed += curr_str;
					}
					else
						parsed += curr_str;
					break;
				case 1:
					signal += curr_str;
					state = 2;
					break;
				case 2:
					signal += curr_str;
					state = 3;
					break;
				case 3:
					parsed += ("(jet(\"" + signal + curr_str + "\", " + shot + ")) ");
					signal = "";
					state = 0;
					break;
				}
			}
		}
		catch (final Exception e)
		{
			System.out.println(e);
		}
		return parsed;
	}

	/*
	 * public synchronized float[] GetFloatArray(String in) throws IOException {
	 * //System.out.println("parsed: "+ parsed); float [] out_array =
	 * super.GetFloatArray(ParseExpression(in)); if(out_array == null && error ==
	 * null) error = "Cannot evaluate " + in + " for shot " + shot;
	 *
	 * //if(out_array != null && out_array.length <= 1) //{ // error =
	 * "Cannot evaluate " + in + " for shot " + shot; // return null; //}
	 *
	 * return out_array; }
	 */
	@Override
	public synchronized RealArray GetRealArray(String in) throws IOException
	{
		return super.GetRealArray(ParseExpression(in));
	}

	@Override
	public boolean SupportsCompression()
	{
		return false;
	}

	@Override
	public void SetCompression(boolean state)
	{}

	public boolean SupportsContinuous()
	{
		return false;
	}

	public boolean DataPending()
	{
		return false;
	}

	@Override
	public int InquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	@Override
	public boolean SupportsFastNetwork()
	{
		return true;
	}
}
