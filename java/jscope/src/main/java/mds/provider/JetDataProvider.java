package mds.provider;

import java.io.IOException;
import java.util.StringTokenizer;

public class JetDataProvider extends MdsDataProvider
{
	public JetDataProvider()
	{
		super("mdsplus.jet.efda.org");
	}

	@Override
	public synchronized int[] getIntArray(String in) throws IOException
	{
		return super.getIntArray(parseExpression(in));
	}

	@Override
	public synchronized RealArray GetRealArray(String in) throws IOException
	{
		return super.GetRealArray(parseExpression(in));
	}

	private String parseExpression(String in)
	{
		// Syntax: ppf/<signal> or jpf/<signal>
		// Ex: ppf/magn/ipla
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
}
