package mds.provider;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.URL;
import java.net.URLConnection;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.swing.*;

import mds.connection.*;
import mds.provider.jet.*;
import mds.wave.*;

public class JetDataProvider implements DataProvider
{
	public static void main(String... args) throws IOException
	{
		System.out.println("\nStart readout PPF/40573/MAGN/IPLA");
		final JetDataProvider dp = new JetDataProvider("obarana", "clublatino");
		dp.setEvaluateUrl(true);
		float data[], x[];
		data = dp.GetFloatArray("PPF/40573/MAGN/BPOL", JetDataProvider.DATA);
		x = dp.GetFloatArray("PPF/40573/MAGN/BPOL", JetDataProvider.X);
		dp.GetFloatArray("X:PPF/40573/MAGN/BPOL", JetDataProvider.Y);
		for (int i = 0; i < x.length; i++)
			System.out.println(x[i] + "  " + data[i]);
		System.out.println("Num. points: " + data.length);
	}

	static final int DATA = 0, X = 1, Y = 2;
	String provider;
	String experiment;
	long shot;
	String username, passwd;
	String encoded_credentials;
	String error_string;
	URL url;
	byte[] buffer;
	Base64 translator = new Base64();
	BufferedInputStream br;
	int content_len;
	private String last_url_name;
	private float[] last_data, last_x, last_y;
	private int dimension;
	JDialog inquiry_dialog;
	JFrame owner_f;
	private int login_status;
	private boolean evaluate_url = false;
	private String url_source = "http://data.jet.uk/";
	private final Vector<ConnectionListener> connection_listener = new Vector<>();
	JTextField user_text;
	JPasswordField passwd_text;

	public JetDataProvider()
	{
		this(null, null);
	}

	JetDataProvider(String username, String passwd)
	{
		provider = "Jet Data";
		final String credentials = username + ":" + passwd;
		try
		{
			encoded_credentials = translator.encode(credentials);
		}
		catch (final Exception e)
		{}
	}

	@Override
	public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		throw (new IOException("Frames visualization on JetDataProvider not implemented"));
	}

	public void enableAsyncUpdate(boolean enable)
	{}

	@Override
	public void SetEnvironment(String s)
	{}

	@Override
	public void Dispose()
	{}

	@Override
	public String GetString(String in, int row, int col, int index)
	{
		return in;
	}

	@Override
	public double GetFloat(String in, int row, int col, int index)
	{
		return Double.parseDouble(in);
	}

	@Override
	public String ErrorString()
	{
		return error_string;
	}

	@Override
	public void AddUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void RemoveUpdateEventListener(UpdateEventListener l, String event)
	{}

	public boolean SupportsContinuous()
	{
		return false;
	}

	public boolean DataPending()
	{
		return false;
	}

	public boolean SupportsFastNetwork()
	{
		return false;
	}

	@Override
	public void SetArgument(String arg)
	{}

	@Override
	public boolean SupportsTunneling()
	{
		return false;
	}

	class SimpleWaveData implements WaveData
	{
		String in_x, in_y;

		public SimpleWaveData(String in_y)
		{
			this.in_y = in_y;
		}

		public SimpleWaveData(String in_y, String in_x)
		{
			this.in_y = in_y;
			this.in_x = in_x;
		}

		@Override
		public boolean supportsStreaming()
		{
			return false;
		}

		@Override
		public int getNumDimension() throws IOException
		{
			GetFloatArray(in_y, DATA);
			return dimension;
		}

		public float[] GetFloatData() throws IOException
		{
			return GetFloatArray(in_y, DATA);
		}

		public double[] GetXDoubleData()
		{
			return null;
		}

		public long[] GetXLongData()
		{
			return null;
		}

		public float[] GetXData() throws IOException
		{
			if (in_x != null)
				return GetFloatArray(in_x, X);
			else
				return GetFloatArray(in_y, X);
		}

		public float[] GetYData() throws IOException
		{
			return GetFloatArray(in_y, Y);
		}

		@Override
		public String GetTitle() throws IOException
		{
			return null;
		}

		@Override
		public String GetXLabel() throws IOException
		{
			return null;
		}

		@Override
		public String GetYLabel() throws IOException
		{
			return null;
		}

		@Override
		public String GetZLabel() throws IOException
		{
			return null;
		}

		@Override
		public XYData getData(long xmin, long xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public XYData getData(double xmin, double xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public XYData getData(int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public float[] getZ()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public double[] getX2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public long[] getX2DLong()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public float[] getY2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		public double[] getXLimits()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		public long[] getXLong()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public boolean isXLong()
		{ return false; }

		@Override
		public void addWaveDataListener(WaveDataListener listener)
		{}

		@Override
		public void removeWaveDataListener(WaveDataListener listener)
		{}

		@Override
		public void getDataAsync(double lowerBound, double upperBound, int numPoints)
		{}
	}

	@Override
	public WaveData GetWaveData(String in, int row, int col, int index)
	{
		return new SimpleWaveData(in);
	}

	@Override
	public WaveData GetWaveData(String in_y, String in_x, int row, int col, int index)
	{
		return new SimpleWaveData(in_y, in_x);
	}

	public WaveData GetResampledWaveData(String in, double start, double end, int n_points)
	{
		return null;
	}

	public WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
	{
		return null;
	}

	public int GetLoginStatus()
	{
		return login_status;
	}

	public void setEvaluateUrl(boolean state)
	{ evaluate_url = state; }

	public void setUrlSource(String url_source)
	{
		this.url_source = url_source;
		// System.out.println(url_source);
	}

	public boolean SupportsCompression()
	{
		return false;
	}

	public void SetCompression(boolean state)
	{}

	@Override
	public int InquireCredentials(JFrame f, DataServerItem server_item)
	{
		final String user = server_item.getUser();
		login_status = DataProvider.LOGIN_OK;
		owner_f = f;
		inquiry_dialog = new JDialog(f, "JET data server login", true);
		inquiry_dialog.getContentPane().setLayout(new BorderLayout());
		JPanel p = new JPanel();
		p.add(new JLabel("Username: "));
		user_text = new JTextField(15);
		p.add(user_text);
		if (user != null)
			user_text.setText(user);
		inquiry_dialog.getContentPane().add(p, "North");
		p = new JPanel();
		p.add(new JLabel("Password: "));
		passwd_text = new JPasswordField(15);
		passwd_text.setEchoChar('*');
		p.add(passwd_text);
		inquiry_dialog.getContentPane().add(p, "Center");
		p = new JPanel();
		final JButton ok_b = new JButton("Ok");
		ok_b.setDefaultCapable(true);
		ok_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				username = user_text.getText();
				passwd = new String(passwd_text.getPassword());
				if (!CheckPasswd(username, passwd))
				{
					JOptionPane.showMessageDialog(inquiry_dialog,
							"Login ERROR : " + ((error_string != null) ? error_string : "no further information"),
							"alert", JOptionPane.ERROR_MESSAGE);
					login_status = DataProvider.LOGIN_ERROR;
				}
				else
				{
					inquiry_dialog.setVisible(false);
					login_status = DataProvider.LOGIN_OK;
				}
			}
		});
		p.add(ok_b);
		final JButton clear_b = new JButton("Clear");
		clear_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				user_text.setText("");
				passwd_text.setText("");
			}
		});
		p.add(clear_b);
		final JButton cancel_b = new JButton("Cancel");
		cancel_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				login_status = DataProvider.LOGIN_CANCEL;
				inquiry_dialog.setVisible(false);
			}
		});
		p.add(cancel_b);
		inquiry_dialog.getContentPane().add(p, "South");
		inquiry_dialog.pack();
		if (f != null)
		{
			final Rectangle r = f.getBounds();
			inquiry_dialog.setLocation(r.x + r.width / 2 - inquiry_dialog.getBounds().width / 2,
					r.y + r.height / 2 - inquiry_dialog.getBounds().height / 2);
		}
		else
		{
			final Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
			inquiry_dialog.setLocation(screenSize.width / 2 - inquiry_dialog.getSize().width / 2,
					screenSize.height / 2 - inquiry_dialog.getSize().height / 2);
		}
		inquiry_dialog.setVisible(true);
		return login_status;
	}

	public boolean CheckPasswd(String encoded_credentials)
	{
		this.encoded_credentials = encoded_credentials;
		// System.out.println(encoded_credentials);
		try
		{
			URLConnection urlcon;
			url = new URL(url_source);
			urlcon = url.openConnection();
			urlcon.setRequestProperty("Authorization", "Basic " + encoded_credentials);
			br = new BufferedInputStream(urlcon.getInputStream());
			content_len = urlcon.getContentLength();
			if (content_len <= 0)
				return false;
			buffer = new byte[content_len];
			int num_read_bytes = 0;
			while (num_read_bytes < content_len)
				num_read_bytes += br.read(buffer, num_read_bytes, buffer.length - num_read_bytes);
			br.close();
			br = null;
		}
		catch (final Exception e)
		{
			error_string = e.getMessage();
			return false;
		}
		final String out = new String(buffer);
		if (out.indexOf("incorrect password") != -1)
		{
			error_string = "Incorrect password";
			return false;
		}
		return true;
	}

	boolean CheckPasswd(String username, String passwd)
	{
		final String credentials = username + ":" + passwd;
		encoded_credentials = translator.encode(credentials);
		return CheckPasswd(encoded_credentials);
	}

	@Override
	public void Update(String experiment, long shot)
	{
		this.experiment = experiment;
		this.shot = shot;
		error_string = null;
	}

	public float[] GetFloatArray(String in, int type) throws IOException
	{
		error_string = null;
		final boolean is_time = (type == X);
		final boolean is_y = (type == Y);
		String url_name;
		if (evaluate_url)
		{
			url_name = in;
		}
		else
		{
			if (experiment == null)
			{
				final StringTokenizer st = new StringTokenizer(in, "/", true);
				url_name = st.nextToken() + "/" + shot;
				while (st.hasMoreTokens())
					url_name = url_name + st.nextToken();
			}
			else
				url_name = experiment + "/" + shot + "/" + in;
		}
		final ConnectionEvent e = new ConnectionEvent(this, "Network");
		dispatchConnectionEvent(e);
		if (last_url_name != null && url_name.equals(last_url_name))
		{
			if (is_time)
				return last_x;
			else if (is_y)
				return last_y;
			else
				return last_data;
		}
		else
		{
			last_x = last_data = last_y = null;
			try
			{
				dimension = 1;
				last_url_name = url_name;
				URLConnection urlcon;
				url = new URL(url_source + url_name);
				urlcon = url.openConnection();
				// urlcon.setRequestProperty("Connection", "Keep-Alive");
				urlcon.setRequestProperty("Authorization", "Basic " + encoded_credentials);
				final InputStream is = urlcon.getInputStream();
				br = new BufferedInputStream(is);
				content_len = urlcon.getContentLength();
				if (content_len <= 0)
				{
					last_url_name = null;
					error_string = "Error reading URL " + url_name + " : null content length";
					throw (new IOException(error_string));
					// return null;
				}
				buffer = new byte[content_len];
				int num_read_bytes = 0;
				while (num_read_bytes < content_len)
					num_read_bytes += br.read(buffer, num_read_bytes, buffer.length - num_read_bytes);
				br.close();
				br = null;
				final JiNcSource jns = new JiNcSource("myname", new RandomAccessData(buffer));
				final JiVar jvarData = jns.getVar("SIGNAL");
				final int ndims = jvarData.getDims().length;
				final JiDim jdimTime = jvarData.getDims()[ndims - 1];
				final JiVar jvarTime = jns.getVar(jdimTime.mName);
				JiDim jdimXData = null;
				JiVar jvarXData = null;
				if (ndims >= 2)
				{
					jdimXData = jvarData.getDims()[ndims - 2];
					if (jdimXData != null)
					{
						jvarXData = jns.getVar(jdimXData.mName);
					}
				}
				JiDim[] dims = jvarTime.getDims();
				final double[] time = jvarTime.readDouble(dims);
				last_x = new float[time.length];
				for (int i = 0; i < time.length; i++)
					last_x[i] = (float) time[i];
				dims = jvarData.getDims();
				last_data = jvarData.readFloat(dims);
				if (jvarXData != null)
				{
					dimension = 2;
					dims = jvarXData.getDims();
					last_y = jvarXData.readFloat(dims);
				}
			}
			catch (final Exception ex)
			{
				error_string = "Error reading URL " + url_name + " : " + ex;
				last_url_name = null;
				throw (new IOException(error_string));
			}
			if (is_time)
				return last_x;
			else if (is_y)
				return last_y;
			else
				return last_data;
		}
	}

	@Override
	public long[] GetShots(String in, String experiment) throws IOException
	{
		error_string = null;
		long[] result;
		String curr_in = in.trim();
		if (curr_in.startsWith("[", 0))
		{
			if (curr_in.endsWith("]"))
			{
				curr_in = curr_in.substring(1, curr_in.length() - 1);
				final StringTokenizer st = new StringTokenizer(curr_in, ",", false);
				result = new long[st.countTokens()];
				int i = 0;
				try
				{
					while (st.hasMoreTokens())
						result[i++] = Long.parseLong(st.nextToken());
					return result;
				}
				catch (final Exception e)
				{}
			}
		}
		else
		{
			if (curr_in.indexOf(":") != -1)
			{
				final StringTokenizer st = new StringTokenizer(curr_in, ":");
				int start, end;
				if (st.countTokens() == 2)
				{
					try
					{
						start = Integer.parseInt(st.nextToken());
						end = Integer.parseInt(st.nextToken());
						if (end < start)
							end = start;
						result = new long[end - start + 1];
						for (int i = 0; i < end - start + 1; i++)
							result[i] = start + i;
						return result;
					}
					catch (final Exception e)
					{}
				}
			}
			else
			{
				result = new long[1];
				try
				{
					result[0] = Long.parseLong(curr_in);
					return result;
				}
				catch (final Exception e)
				{}
			}
		}
		error_string = "Error parsing shot number(s)";
		throw (new IOException(error_string));
	}

	@Override
	public void AddConnectionListener(ConnectionListener l)
	{
		if (l == null)
		{
			return;
		}
		connection_listener.addElement(l);
	}

	@Override
	public void RemoveConnectionListener(ConnectionListener l)
	{
		if (l == null)
		{
			return;
		}
		connection_listener.removeElement(l);
	}

	protected void dispatchConnectionEvent(ConnectionEvent e)
	{
		if (connection_listener != null)
			for (int i = 0; i < connection_listener.size(); i++)
				connection_listener.elementAt(i).processConnectionEvent(e);
	}

	@Override
	public boolean isBusy()
	{ return false; }
}
