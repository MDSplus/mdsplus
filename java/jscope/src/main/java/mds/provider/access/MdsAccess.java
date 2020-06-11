package mds.provider.access;

import java.io.IOException;
import java.util.StringTokenizer;

import mds.provider.MdsDataProvider;
import mds.wave.*;

public class MdsAccess implements DataAccess
{
	public static void main(String[] args) throws IOException
	{ // TODO: could this be unittest
		final MdsAccess localMdsAccess = new MdsAccess();
		final String str = args.length > 1 ? args[1]
				: "mds://raserver.igi.cnr.it:8123/rfx~inProgress/29417/\\DICO28::COS_CO2_5";
		localMdsAccess.supports(str);
		localMdsAccess.getSignal(str);
		localMdsAccess.getX(str);
	}

	String ip_addr = null;
	String shot_str = null;
	String signal = null;
	String experiment = null;
	MdsDataProvider np = null;
	String error = null;
	String encoded_credentials = null;
	String prevUrl = null;

	@Override
	public boolean supports(String url)
	{
		final StringTokenizer st = new StringTokenizer(url, ":");
		if (st.countTokens() < 2)
			return false;
		return st.nextToken().equals("mds");
	}

	@Override
	public void setProvider(String url) throws IOException
	{
		if ((url != null) && (this.prevUrl != null) && (this.prevUrl.equals(url)))
		{
			return;
		}
		final StringTokenizer st = new StringTokenizer(url, ":");
		String urlPath = st.nextToken();
		urlPath = st.nextToken("");
		urlPath = urlPath.substring(2);
		final StringTokenizer st1 = new StringTokenizer(urlPath, "/");
		if (st1.countTokens() < 4)
		{
			return;
		}
		final String ipAddress = st1.nextToken();
		if (ipAddress == null)
			return;
		if ((this.ip_addr == null) || (!this.ip_addr.equals(ipAddress)))
		{
			this.np = new MdsDataProvider(ipAddress);
			this.ip_addr = ipAddress;
		}
		String region = null;
		this.experiment = st1.nextToken();
		final StringTokenizer localStringTokenizer3 = new StringTokenizer(this.experiment, "~");
		if (localStringTokenizer3.countTokens() == 2)
		{
			this.experiment = localStringTokenizer3.nextToken();
			region = localStringTokenizer3.nextToken();
		}
		if ((this.experiment != null) && (!this.experiment.equals("")))
		{
			this.shot_str = st1.nextToken();
			final int shot = Integer.parseInt(this.shot_str);
			if (region != null)
			{
				final int out[] = np.getIntArray("treeSetSource('" + this.experiment + "','" + region + "')");
				if (out == null)
					return;
			}
			this.np.Update(this.experiment, shot, true);
		}
		this.signal = st1.nextToken();
		this.prevUrl = url;
	}

	public void setProviderOld(String url) throws IOException
	{
		final StringTokenizer st1 = new StringTokenizer(url, ":");
		String content = st1.nextToken();
		content = st1.nextToken("");
		content = content.substring(2);
		final StringTokenizer st2 = new StringTokenizer(content, "/");
		if (st2.countTokens() < 4) // ip addr/exp/shot/signal
			return;
		final String addr = st2.nextToken();
		if (addr == null)
			return;
		if (ip_addr == null || !ip_addr.equals(addr))
		{
			np = new MdsDataProvider(addr);
			/*
			 * if(encoded_credentials == null ||( ip_addr != null && !ip_addr.equals(addr)))
			 * { encoded_credentials = new String(); np.InquireCredentials(null,
			 * encoded_credentials); }
			 */
			ip_addr = addr;
		}
		experiment = st2.nextToken();
		if (experiment != null && !experiment.equals(""))
		{
			// String shot_str = st2.nextToken();
			shot_str = st2.nextToken();
			final int shot = Integer.parseInt(shot_str);
			np.update(experiment, shot);
		}
		signal = st2.nextToken();
	}

	@Override
	public String getShot()
	{ return shot_str; }

	@Override
	public String getSignalName()
	{ return signal; }

	@Override
	public String getExperiment()
	{ return experiment; }

	@Override
	public DataProvider getDataProvider()
	{ return np; }

	@Override
	public void close()
	{
		if (np != null)
			np.close();
		np = null;
		ip_addr = null;
	}

	@Override
	public float[] getX(String url) throws IOException
	{
		setProvider(url);
		if (signal == null)
			return null;
		return np.GetFloatArray("DIM_OF(" + signal + ")");
	}

	@Override
	public float[] getY(String url) throws IOException
	{
		setProvider(url);
		if (signal == null)
			return null;
		return np.GetFloatArray(signal);
	}

	public String getExpression(String paramString) throws IOException
	{
		System.out.println("Expr URL = " + paramString);
		setProvider(paramString);
		if (this.signal == null)
			return null;
		return this.np.getStringValue(this.signal);
	}

	@Override
	public Signal getSignal(String url) throws IOException
	{
		Signal s = null;
		error = null;
		final float y[] = getY(url);
		final float x[] = getX(url);
		System.out.println("URL = " + url);
		if (x == null || y == null)
		{
			error = np.getError();
			return null;
		}
		s = new Signal(x, y);
		return s;
	}

	@Override
	public FrameData getFrameData(String url) throws IOException
	{
		setProvider(url);
		return np.getFrameData(signal, null, (float) -1E8, (float) 1E8);
	}

	@Override
	public void setPassword(String encoded_credentials)
	{ this.encoded_credentials = encoded_credentials; }

	@Override
	public String getError()
	{
		if (np == null)
			return ("Cannot create MdsDataProvider");
		if (error != null)
			return error;
		return np.getError();
	}
}
