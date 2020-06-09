package mds.provider;

import java.io.IOException;

import javax.swing.JFrame;

import mds.provider.mds.MdsplusParser;
import mds.wave.DataProvider;
import mds.wave.DataServerItem;

public class TsDataProvider extends MdsDataProvider
{
	public TsDataProvider()
	{
		super();
	}

	public TsDataProvider(String provider) throws IOException
	{
		super(provider);
	}

	@Override
	public void SetArgument(String arg) throws IOException
	{
		mds.setProvider(arg);
		mds.setUser("mdsplus");
	}

	@Override
	public synchronized void Update(String exp, long s)
	{
		error = null;
		shot = (int) s;
	}

	private String parseExpression(String in)
	{
		return MdsplusParser.parseFun(in, "GetTsBase(" + shot + ", \"", "\")");
	}

	@Override
	public synchronized int[] getIntArray(String in) throws IOException
	{
		return super.getIntArray(parseExpression(in));
	}

	@Override
	public synchronized float[] GetFloatArray(String in) throws IOException
	{
		final String parsed = parseExpression(in);
		if (parsed == null)
			return null;
		error = null;
		final float[] out_array = super.GetFloatArray(parsed);
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
	public boolean SupportsCompression()
	{
		return false;
	}

	@Override
	public void SetCompression(boolean state)
	{}

	@Override
	public int InquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	@Override
	public int[] getNumDimensions(String spec)
	{
		return new int[]
		{ 1 };
	}
}
