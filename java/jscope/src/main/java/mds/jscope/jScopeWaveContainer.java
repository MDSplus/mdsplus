package mds.jscope;

import java.awt.*;
import java.awt.print.*;
import java.io.*;
import java.net.InetAddress;
import java.util.*;

import javax.print.*;
import javax.print.attribute.PrintRequestAttributeSet;
import javax.swing.*;

import mds.connection.ConnectionListener;
import mds.connection.UpdateEventListener;
import mds.provider.MdsDataProvider;
import mds.wave.*;

class jScopeWaveContainer extends WaveformContainer implements Printable
{
	class UpdW extends Thread
	{
		boolean pending = false;

		@Override
		synchronized public void run()
		{
			Date date = new Date();
			long start, end;
			WaveContainerEvent wce = null;
			setName("Update Thread");
			while (true)
			{
				try
				{
					if (!pending)
						wait();
					pending = false;
					date = new Date();
					start = date.getTime();
					wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE, "Start Update");
					jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
					try
					{
						UpdateAllWave();
						date = new Date();
						end = date.getTime();
						String msg;
						if (!abort)
							msg = "All waveforms are up to date < " + (end - start) + " ms >";
						else
							msg = " Aborted ";
						wce = new WaveContainerEvent(this, WaveContainerEvent.END_UPDATE, msg);
						jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
					}
					catch (final Throwable e)
					{
						// e.printStackTrace();
						wce = new WaveContainerEvent(this, WaveContainerEvent.KILL_UPDATE, e.getMessage());
						jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
					}
				}
				catch (final InterruptedException e)
				{}
			}
		}

		synchronized public void StartUpdate()
		{
			pending = true;
			notify();
		}
	}
	private static final long serialVersionUID = 1L;
	private static final int MAX_COLUMN = 4;
	DataProvider dp;
	private final jScopeDefaultValues def_vals;
	private String title = null;
	private DataServerItem server_item = null;
	private String event = null;
	private String print_event = null;
	private int columns;
	private UpdW updateThread;
	private boolean abort;
	private boolean add_sig = false;
	private long main_shots[] = null;
	private String main_shot_str = null;
	private String main_shot_error = null;
	jScopeMultiWave wave_all[];
	private String save_as_txt_directory = null;
	private jScopeBrowseSignals browse_sig = null;
	private String prev_add_signal = null;

	private final Object mainShotLock = new Object();

	ProgressMonitor progressMonitor;

	public jScopeWaveContainer(int rows[], DataProvider dp, jScopeDefaultValues def_vals)
	{
		super(rows, false);
		this.def_vals = def_vals;
		this.dp = dp;
		final Component c[] = CreateWaveComponents(getComponentNumber());
		AddComponents(c);
		updateThread = new UpdW();
		updateThread.start();
		setBackground(Color.white);
		save_as_txt_directory = System.getProperty("jScope.curr_directory");
	}

	public jScopeWaveContainer(int rows[], jScopeDefaultValues def_vals)
	{
		this(rows, DataServerItem.NotConnected, def_vals);
		server_item = new DataServerItem();
	}

	public void AbortUpdate()
	{
		abort = true;
	}

	public synchronized void AddAllEvents(UpdateEventListener l) throws IOException
	{
		jScopeMultiWave w;
		if (dp == null)
			return;
		if (event != null && event.length() != 0)
			dp.addUpdateEventListener(l, event);
		if (print_event != null && print_event.length() != 0)
			dp.addUpdateEventListener(l, print_event);
		for (int i = 0, k = 0; i < 4; i++)
		{
			for (int j = 0; j < getComponentsInColumn(i); j++, k++)
			{
				w = (jScopeMultiWave) getGridComponent(k);
				w.AddEvent();
			}
		}
	}

	private String AddRemoveEvent(UpdateEventListener l, String curr_event, String event) throws IOException
	{
		if (curr_event != null && curr_event.length() != 0)
		{
			if (event == null || event.length() == 0)
			{
				dp.removeUpdateEventListener(l, curr_event);
				return null;
			}
			else
			{
				if (!curr_event.equals(event))
				{
					dp.removeUpdateEventListener(l, curr_event);
					dp.addUpdateEventListener(l, event);
				}
				return event;
			}
		}
		else
		{
			if (event != null && event.length() != 0)
				dp.addUpdateEventListener(l, event);
			return event;
		}
	}

	public void AddSignal(String expr, boolean check_prev_signal)
	{
		if (expr != null && expr.length() != 0)
			if (!check_prev_signal || (check_prev_signal && (prev_add_signal == null || !prev_add_signal.equals(expr))))
			{
				prev_add_signal = expr;
				AddSignal(null, null, "", expr, false, false);
			}
	}

	public void AddSignal(String tree, String shot, String x_expr, String y_expr, boolean with_error, boolean is_image)
	{
		final String x[] = new String[1];
		final String y[] = new String[1];
		x[0] = x_expr;
		y[0] = y_expr;
		AddSignals(tree, shot, x, y, with_error, is_image);
	}

	// with_error == true => Signals is added also if an error occurs
// during its evaluations
	public void AddSignals(String tree, String shot, String x_expr[], String y_expr[], boolean with_error,
			boolean is_image)
	{
		MdsWaveInterface new_wi = null;
		final jScopeMultiWave sel_wave = (jScopeMultiWave) GetSelectPanel();
		if (sel_wave.wi == null || is_image)
		{
			sel_wave.wi = new MdsWaveInterface(sel_wave, dp, def_vals);
			sel_wave.wi.SetAsImage(is_image);
			if (!with_error)
				((MdsWaveInterface) sel_wave.wi).prev_wi = new MdsWaveInterface(sel_wave, dp, def_vals);
		}
		else
		{
			new_wi = new MdsWaveInterface((MdsWaveInterface) sel_wave.wi);
			new_wi.wave = sel_wave;
			if (!with_error)
				new_wi.prev_wi = (MdsWaveInterface) sel_wave.wi;
			sel_wave.wi = new_wi;
		}
		if (tree != null && (((MdsWaveInterface) sel_wave.wi).cexperiment == null
				|| ((MdsWaveInterface) sel_wave.wi).cexperiment.trim().length() == 0))
		{
			((MdsWaveInterface) sel_wave.wi).cexperiment = new String(tree);
			((MdsWaveInterface) sel_wave.wi).defaults &= ~(1 << MdsWaveInterface.B_exp);
		}
		if (shot != null && (((MdsWaveInterface) sel_wave.wi).cin_shot == null
				|| ((MdsWaveInterface) sel_wave.wi).cin_shot.trim().length() == 0))
		{
			((MdsWaveInterface) sel_wave.wi).cin_shot = new String(shot);
			((MdsWaveInterface) sel_wave.wi).defaults &= ~(1 << MdsWaveInterface.B_shot);
		}
		if (sel_wave.wi.AddSignals(x_expr, y_expr))
		{
			add_sig = true;
			Refresh(sel_wave, "Add signal to");
			update();
			add_sig = false;
		}
	}

	public DataServerItem buildDataServerItem(Properties pr, String prompt)
	{
		if (pr == null)
			return null;
		final String name = pr.getProperty(prompt + ".data_server_name");
		if (name == null)
			return null;
		final String argument = pr.getProperty(prompt + ".data_server_argument");
		final String user = pr.getProperty(prompt + ".data_server_user");
		final String class_name = pr.getProperty(prompt + ".data_server_class");
		final String browse_class = pr.getProperty(prompt + ".data_server_browse_class");
		final String browse_url = pr.getProperty(prompt + ".data_server_browse_url");
		final String tunnel_port = pr.getProperty(prompt + ".data_server_tunnel_port");
		return new DataServerItem(name, argument, user, class_name, browse_class, browse_url, tunnel_port);
	}

	protected jScopeMultiWave BuildjScopeMultiWave(DataProvider dp, jScopeDefaultValues def_vals)
	{
		return new jScopeMultiWave(dp, def_vals);
	}

	public void ChangeDataProvider(DataProvider dp)
	{
		jScopeMultiWave w;
		main_shot_str = null;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = (jScopeMultiWave) GetWavePanel(i);
			if (w != null)
			{
				if (w.wi != null)
				{
					w.wi.SetDataProvider(dp);
				}
				w.Erase();
				w.SetTitle(null);
			}
		}
	}

	private boolean checkIpMask(String ip, String Mask)
	{
		boolean found = false;
		final StringTokenizer tokenLocalIp = new StringTokenizer(ip, ".");
		final StringTokenizer clientMaskIp = new StringTokenizer(Mask, ".");
		if (tokenLocalIp.countTokens() != clientMaskIp.countTokens())
			return false;
		while (tokenLocalIp.hasMoreElements() && clientMaskIp.hasMoreTokens())
		{
			final String tl = tokenLocalIp.nextToken();
			final String tm = clientMaskIp.nextToken();
			if (tl.equals(tm) || tm.equals("*"))
			{
				found = true;
				continue;
			}
			found = false;
			break;
		}
		return found;
	}

@Override
	protected Component[] CreateWaveComponents(int num)
	{
		final Component c[] = new Component[num];
		jScopeMultiWave wave = null;
		for (int i = 0; i < c.length; i++)
		{
			wave = BuildjScopeMultiWave(dp, def_vals);
			wave.addWaveformListener(this);
			SetWaveParams(wave);
			c[i] = wave;
		}
		return c;
	}

	public DataServerItem DataServerFromClient(DataServerItem dataServerIn)
	{
		int c = 0;
		final boolean found = false;
		String clientMask;
		DataServerItem out = null;
		String prompt;
		try
		{
			final InetAddress localaddr = InetAddress.getLocalHost();
			final String localIpAdddress = localaddr.getHostAddress();
			if (dataServerIn != null && !dataServerIn.getClassName().equals(MdsDataProvider.class.getName()))
				return null;
			if (dataServerIn != null)
			{
				final InetAddress dsiInet = InetAddress.getByName(dataServerIn.getArgument());
				final String disIpAdddress = dsiInet.getHostAddress();
				final StringTokenizer st = new StringTokenizer(localIpAdddress, ".");
				clientMask = st.nextToken() + "." + st.nextToken() + ".*.*";
				if (!checkIpMask(disIpAdddress, clientMask))
					return dataServerIn;
			}
			if ((new File(jScopeFacade.JSCOPE_SERVERS)).exists())
			{
				final Properties srvFromClientProp = new Properties();
				final FileInputStream fis = new FileInputStream(jScopeFacade.JSCOPE_SERVERS);
				srvFromClientProp.load(fis);
				fis.close();
				while (!found)
				{
					c++;
					prompt = "jScope.server_from_client_" + c;
					clientMask = srvFromClientProp.getProperty(prompt + ".client_mask");
					if (clientMask == null)
						break;
					/*
					 * StringTokenizer tokenLocalIp = new StringTokenizer(localIpAdddress, ".");
					 * StringTokenizer clientMaskIp = new StringTokenizer(clientMask, ".");
					 *
					 * if(tokenLocalIp.countTokens() != clientMaskIp.countTokens()) continue;
					 *
					 * while(tokenLocalIp.hasMoreElements() && clientMaskIp.hasMoreTokens()) {
					 * String tl = tokenLocalIp.nextToken(); String tm = clientMaskIp.nextToken();
					 * if(tl.equals(tm) || tm.equals("*") ) { found = true; continue; } found =
					 * false; break; }
					 */
					if (checkIpMask(localIpAdddress, clientMask))
					{
						out = buildDataServerItem(srvFromClientProp, prompt);
					}
				}
			}
		}
		catch (final Exception exc)
		{
			out = null;
		}
		return out;
	}

	public void EraseAllWave()
	{
		jScopeMultiWave w;
		for (int i = 0; i < getComponentNumber(); i++)
		{
			w = (jScopeMultiWave) getGridComponent(i);
			if (w != null)
				w.Erase();
		}
	}

	public void EvaluateMainShot(String in_shots) throws IOException
	{
		long long_data[] = null;
		synchronized (mainShotLock)
		{
			main_shot_error = null;
			main_shots = null;
			main_shot_str = null;
			if (in_shots == null || in_shots.trim().length() == 0)
			{
				main_shot_error = "Main shot value Undefine";
				return;
			}
			long_data = WaveInterface.GetShotArray(in_shots, def_vals.experiment_str, dp);
			if (main_shot_error == null)
				main_shots = long_data;
			main_shot_str = in_shots.trim();
		}
	}

	public void FromFile(Properties pr, String prompt, int colorMapping[], ColorMapDialog cmd) throws IOException
	{
		String prop;
		final int read_rows[] =
		{ 0, 0, 0, 0 };
		resetMaximizeComponent();
		prop = pr.getProperty(prompt + ".columns");
		if (prop == null)
			throw (new IOException("missing columns keyword"));
		else
		{
			columns = Integer.parseInt(prop);
			pw = new float[MAX_COLUMN];
		}
		title = pr.getProperty(prompt + ".title");
		event = pr.getProperty(prompt + ".update_event");
		print_event = pr.getProperty(prompt + ".print_event");
		server_item = buildDataServerItem(pr, prompt);
		final DataServerItem server_item_conf = DataServerFromClient(server_item);
		if (server_item_conf != null)
		{
			/*
			 * Object[] options = { "YES", "NO" }; int out =
			 * JOptionPane.showOptionDialog(null, "Replace data server " + server_item.name
			 * + " defined in jScope\n" + "configuration file with "+ server_item_conf.name
			 * + " defined in jScope configuration,\n"+
			 * "optimal solution for  access data from this node.", "Warning",
			 * JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null, options,
			 * options[0]); if(out == JOptionPane.OK_OPTION)
			 */
			server_item = server_item_conf;
		}
		for (int c = 1; c <= MAX_COLUMN; c++)
		{
			prop = pr.getProperty(prompt + ".rows_in_column_" + c);
			if (prop == null)
			{
				if (c == 1)
					throw (new IOException("missing rows_in_column_1 keyword"));
				break;
			}
			final int r = Integer.parseInt(prop);
			read_rows[c - 1] = r;
		}
		if (columns > 1)
		{
			for (int c = 1; c < columns; c++)
			{
				prop = pr.getProperty(prompt + ".vpane_" + c);
				if (prop == null)
				{
					throw (new IOException("missing vpane_" + c + " keyword"));
				}
				final int w = Integer.parseInt(prop);
				pw[c - 1] = w;
			}
		}
		prop = pr.getProperty(prompt + ".reversed");
		if (prop != null)
		{
			reversed = new Boolean(prop).booleanValue();
		}
		else
			reversed = false;
		def_vals.xmax = pr.getProperty(prompt + ".global_1_1.xmax");
		def_vals.xmin = pr.getProperty(prompt + ".global_1_1.xmin");
		def_vals.xlabel = pr.getProperty(prompt + ".global_1_1.x_label");
		def_vals.ymax = pr.getProperty(prompt + ".global_1_1.ymax");
		def_vals.ymin = pr.getProperty(prompt + ".global_1_1.ymin");
		def_vals.ylabel = pr.getProperty(prompt + ".global_1_1.y_label");
		def_vals.experiment_str = pr.getProperty(prompt + ".global_1_1.experiment");
		def_vals.title_str = pr.getProperty(prompt + ".global_1_1.title");
		def_vals.upd_event_str = pr.getProperty(prompt + ".global_1_1.event");
		def_vals.def_node_str = pr.getProperty(prompt + ".global_1_1.default_node");
		prop = pr.getProperty(prompt + ".global_1_1.horizontal_offset");
		{
			if (prop != null)
			{
				int v = 0;
				try
				{
					v = Integer.parseInt(prop);
				}
				catch (final NumberFormatException exc)
				{}
				Waveform.SetHorizontalOffset(v);
			}
		}
		prop = pr.getProperty(prompt + ".global_1_1.vertical_offset");
		{
			if (prop != null)
			{
				int v = 0;
				try
				{
					v = Integer.parseInt(prop);
				}
				catch (final NumberFormatException exc)
				{}
				Waveform.SetVerticalOffset(v);
			}
		}
		prop = pr.getProperty(prompt + ".global_1_1.shot");
		if (prop != null)
		{
			if (prop.indexOf("_shots") != -1)
				def_vals.shot_str = prop.substring(prop.indexOf("[") + 1, prop.indexOf("]"));
			else
				def_vals.shot_str = prop;
			def_vals.setIsEvaluated(false);
		}
		ResetDrawPanel(read_rows);
		jScopeMultiWave w;
		for (int c = 0, k = 0; c < 4; c++)
		{
			for (int r = 0; r < read_rows[c]; r++)
			{
				w = (jScopeMultiWave) getGridComponent(k);
				((MdsWaveInterface) w.wi).FromFile(pr, "Scope.plot_" + (r + 1) + "_" + (c + 1), cmd);
				((MdsWaveInterface) w.wi).mapColorIndex(colorMapping);
				SetWaveParams(w);
				k++;
			}
		}
		// Evaluate real number of columns
		int r_columns = 0;
		for (int i = 0; i < 4; i++)
			if (read_rows[i] != 0)
				r_columns = i + 1;
		// Silent file configuration correction
		// possible define same warning information
		if (columns != r_columns)
		{
			columns = r_columns;
			pw = new float[MAX_COLUMN];
			for (int i = 0; i < columns; i++)
				pw[i] = (float) 1. / columns;
		}
		else
		{
			if (columns == 4)
				pw[3] = Math.abs((float) ((1000 - pw[2]) / 1000.));
			if (columns >= 3)
				pw[2] = Math.abs((float) (((pw[2] != 0) ? (pw[2] - pw[1]) : 1000 - pw[1]) / 1000.));
			if (columns >= 2)
				pw[1] = Math.abs((float) (((pw[1] != 0) ? (pw[1] - pw[0]) : 1000 - pw[0]) / 1000.));
			if (columns >= 1)
				pw[0] = Math.abs((float) (((pw[0] == 0) ? 1000 : pw[0]) / 1000.));
		}
		UpdateHeight();
	}

	public String GetBrowseClass()
	{
		return (server_item != null ? server_item.getBrowseClass() : "");
	}

	public String GetBrowseUrl()
	{
		return (server_item != null ? server_item.getBrowseUrl() : "");
	}

	public String GetEvaluatedTitle()
	{
		if (title == null || title.length() == 0 || dp == null)
			return "";
		try
		{
			final String t = dp.getString(title, -1, -1, -1);
			final String err = dp.getError();
			if (err == null || err.length() == 0)
				return t;
			else
				return "< evaluation error >";
		}
		catch (final Exception exc)
		{
			return "";
		}
	}

	public String GetEvent()
	{
		return event;
	}

	private JFrame GetFrameParent()
	{
		Container c = this;
		while (c != null && !(c instanceof JFrame))
			c = c.getParent();
		return (JFrame) c;
	}

	public synchronized void getjScopeMultiWave()
	{
		wave_all = new jScopeMultiWave[getGridComponentCount()];
		for (int i = 0, k = 0; i < 4; i++)
		{
			for (int j = 0; j < rows[i]; j++, k++)
				wave_all[k] = (jScopeMultiWave) getGridComponent(k);
		}
	}

	public String getMainShotError(boolean brief)
	{
		// if(brief)
		// return main_shot_error.substring(0, main_shot_error.indexOf("\n"));
		// else
		return main_shot_error;
	}

	public long[] getMainShots()
	{ return main_shots; }

	public String getMainShotStr()
	{ return main_shot_str; }

	public String GetPrintEvent()
	{
		return print_event;
	}

	//  public String GetServerLabel(){return (server_item != null ? server_item.name : "");}
	public String GetServerArgument()
	{
		return (server_item != null ? server_item.getArgument() : "");
	}

	public DataServerItem GetServerItem()
	{
		if (server_item == null)
			server_item = new DataServerItem();
		return server_item;
	}

	/*
	 * remove 28/06/2005 public void SetServerItem(DataServerItem dsi) { server_item
	 * = dsi; }
	 */
	public String GetServerLabel()
	{
		if (dp == null && server_item != null && server_item.getName() != null)
			return "Can't connect to " + server_item.getName();
		if (dp == null && server_item == null)
			return "Not connected";
		return server_item.getName();
	}

	public String GetTitle()
	{
		return title;
	}

	public void InvalidateDefaults()
	{
		jScopeMultiWave w;
		for (int i = 0, k = 0; i < rows.length; i++)
		{
			for (int j = 0; j < rows[i]; j++, k++)
			{
				w = (jScopeMultiWave) getGridComponent(k);
				if (w != null)
					((MdsWaveInterface) w.wi).default_is_update = false;
			}
		}
	}

	public boolean isBusy()
	{ return this.dp.isBusy(); }

	@Override
	public void maximizeComponent(Waveform w)
	{
		super.maximizeComponent(w);
		if (w == null)
			this.StartUpdate();
	}

	@Override
	public void notifyChange(Waveform dest, Waveform source)
	{
		final jScopeMultiWave w = ((jScopeMultiWave) source);
		final MdsWaveInterface mwi = new MdsWaveInterface(((MdsWaveInterface) w.wi));
		mwi.setDefaultsValues(def_vals);
		((jScopeMultiWave) dest).wi = mwi;
		((jScopeMultiWave) dest).wi.SetDataProvider(dp);
		((jScopeMultiWave) dest).wi.wave = dest;
		Refresh((jScopeMultiWave) dest, "Copy in");
	}

	@Override
	public int print(Graphics g, PageFormat pf, int pageIndex) throws PrinterException
	{
		final int st_x = 0, st_y = 0;
		final double height = pf.getImageableHeight();
		final double width = pf.getImageableWidth();
		final Graphics2D g2 = (Graphics2D) g;
//        jScope.displayPageFormatAttributes(4,pf);
		if (pageIndex == 0)
		{
			g2.translate(pf.getImageableX(), pf.getImageableY());
			/*
			 * double scale = (72.0/600.); height /= scale; width /= scale; g2.scale(scale,
			 * scale);
			 */
			PrintAll(g2, st_x, st_y, (int) height, (int) width);
			return Printable.PAGE_EXISTS;
		}
		else
			return Printable.NO_SUCH_PAGE;
	}

	public void PrintAll(Graphics g, int height, int width)
	{
		if (font == null)
		{
			font = g.getFont();
			font = new Font(font.getName(), font.getStyle(), 18);
			g.setFont(font);
		}
		else
		{
			font = new Font(font.getName(), font.getStyle(), 18);
			g.setFont(font);
		}
		super.PrintAll(g, 0, 0, height, width);
	}

	@Override
	public void PrintAll(Graphics g, int st_x, int st_y, int height, int width)
	{
		final String title = GetEvaluatedTitle();
		if (title != null && title.length() != 0)
		{
			FontMetrics fm;
			int s_width;
			if (font == null)
			{
				font = g.getFont();
				font = new Font(font.getName(), font.getStyle(), 18);
				g.setFont(font);
			}
			else
			{
				font = new Font(font.getName(), font.getStyle(), 18);
				g.setFont(font);
			}
			fm = g.getFontMetrics();
			s_width = fm.stringWidth(title);
			st_y += fm.getHeight() / 2 + 2;
			g.drawString(title, st_x + (width - s_width) / 2, st_y);
			st_y += 2;
			height -= st_y;
		}
		super.PrintAll(g, st_x, st_y, height, width);
	}

	public void PrintAllWaves(DocPrintJob prnJob, PrintRequestAttributeSet attrs)
			throws PrinterException, PrintException
	{
		final DocFlavor flavor = DocFlavor.SERVICE_FORMATTED.PRINTABLE;
		final Doc doc = new SimpleDoc(this, flavor, null);
		prnJob.print(doc, attrs);
	}

	@Override
	public void processWaveformEvent(WaveformEvent e)
	{
		super.processWaveformEvent(e);
		final jScopeMultiWave w = (jScopeMultiWave) e.getSource();
		switch (e.getID())
		{
		case WaveformEvent.END_UPDATE:
			final Point p = getComponentPosition(w);
			if (w.wi.isAddSignal())
			{
				String er;
				if (!w.wi.IsSignalAdded())
					prev_add_signal = null;
				if (w.wi.error != null)
					er = w.wi.error;
				else
					er = ((MdsWaveInterface) w.wi).getErrorString(); // this.brief_error);
				if (er != null)
					JOptionPane.showMessageDialog(this, er, "alert processWaveformEvent", JOptionPane.ERROR_MESSAGE);
				w.wi.setAddSignal(false);
			}
			final WaveContainerEvent wce = new WaveContainerEvent(this, WaveContainerEvent.END_UPDATE,
					"Wave column " + p.x + " row " + p.y + " is updated");
			jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
			break;
		}
	}

	public synchronized void Refresh(jScopeMultiWave w, String label)
	{
		Point p = null;
		if (add_sig)
			p = getSplitPosition();
		if (p == null)
			p = getComponentPosition(w);
		final WaveContainerEvent wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE,
				label + " wave column " + p.x + " row " + p.y);
		jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
		// If is added a signal to the waveform only signal added
		// is evaluated, in the other cases, refresh from popup menu
		// or event update, all signals in the waveform must be
		// evaluated
		/*
		 * if (!add_sig) ( (jScopeMultiWave) w).wi.setModified(true);
		 */
		w.Refresh();
		if (add_sig)
			resetSplitPosition();
	}

	public void RemoveAllEvents(UpdateEventListener l) throws IOException
	{
		jScopeMultiWave w;
		if (dp == null)
			return;
		if (event != null && event.length() != 0)
			dp.removeUpdateEventListener(l, event);
		if (print_event != null && print_event.length() != 0)
			dp.removeUpdateEventListener(l, print_event);
		for (int i = 0, k = 0; i < 4; i++)
		{
			for (int j = 0; j < getComponentsInColumn(i); j++, k++)
			{
				w = (jScopeMultiWave) getGridComponent(k);
				w.RemoveEvent();
			}
		}
	}

	private void RepaintAllWave()
	{
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				jScopeMultiWave wx;
				for (int i = 0, k = 0; i < 4; i++)
				{
					for (int j = 0; j < rows[i]; j++, k++)
					{
						wx = (jScopeMultiWave) getGridComponent(k);
						if (wx.wi != null)
							wx.Update(wx.wi);
					}
				}
			}
		});
	}

	public void Reset()
	{
		final int reset_rows[] =
		{ 1, 0, 0, 0 };
		ph = null;
		pw = null;
		SetTitle(null);
		event = null;
		print_event = null;
		ResetDrawPanel(reset_rows);
		update();
		final jScopeMultiWave w = (jScopeMultiWave) GetWavePanel(0);
		w.jScopeErase();
		def_vals.Reset();
	}

	public void SaveAsText(jScopeMultiWave w, boolean all)
	{
		final Vector<Waveform> panel = new Vector<>();
		MdsWaveInterface wi;
		jScopeMultiWave wave;
		if (!all && (w == null || w.wi == null || w.wi.signals == null || w.wi.signals.length == 0))
			return;
		String title = "Save";
		if (all)
			title = "Save all signals in text format";
		else
		{
			final Point p = this.getWavePosition(w);
			if (p != null)
				title = "Save signals on panel (" + p.x + ", " + p.y + ") in text format";
		}
		final JFileChooser file_diag = new JFileChooser();
		if (save_as_txt_directory != null && save_as_txt_directory.trim().length() != 0)
			file_diag.setCurrentDirectory(new File(save_as_txt_directory));
		file_diag.setDialogTitle(title);
		int returnVal = JFileChooser.CANCEL_OPTION;
		boolean done = false;
		String txtsig_file = null;
		while (!done)
		{
			returnVal = file_diag.showSaveDialog(this);
			if (returnVal == JFileChooser.APPROVE_OPTION)
			{
				final File file = file_diag.getSelectedFile();
				txtsig_file = file.getAbsolutePath();
				if (file.exists())
				{
					final Object[] options =
					{ "Yes", "No" };
					final int val = JOptionPane.showOptionDialog(null,
							txtsig_file + " already exists.\nDo you want to replace it?", "Save as",
							JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[1]);
					if (val == JOptionPane.YES_OPTION)
						done = true;
				}
				else
					done = true;
			}
			else
				done = true;
		}
		if (returnVal == JFileChooser.APPROVE_OPTION)
		{
			if (txtsig_file != null)
			{
				save_as_txt_directory = new String(txtsig_file);
				if (all)
				{
					for (int i = 0; i < getWaveformCount(); i++)
						panel.addElement(GetWavePanel(i));
				}
				else
					panel.addElement(w);
				String s = "", s1 = "", s2 = "";
				boolean g_more_point;
				final StringBuffer space = new StringBuffer();
				try
				{
					final BufferedWriter out = new BufferedWriter(new FileWriter(txtsig_file));
					for (int l = 0; l < 3; l++)
					{
						s = "%";
						for (int k = 0; k < panel.size(); k++)
						{
							wave = (jScopeMultiWave) panel.elementAt(k);
							wi = (MdsWaveInterface) wave.wi;
							if (wi == null || wi.signals == null)
								continue;
							for (int i = 0; i < wi.signals.length; i++)
							{
								switch (l)
								{
								case 0:
									s += "x : " + ((wi.in_x != null && wi.in_x.length > 0) ? wi.in_x[i] : "None");
									break;
								case 1:
									s += "y : " + ((wi.in_y != null && wi.in_y.length > 0) ? wi.in_y[i] : "None");
									break;
								case 2:
									s += "Shot : "
											+ ((wi.shots != null && wi.shots.length > 0) ? "" + wi.shots[i] : "None");
									break;
								}
								out.write(s, 0, (s.length() < 50) ? s.length() : 50);
								space.setLength(0);
								for (int u = 0; u < 52 - s.length(); u++)
									space.append(' ');
								out.write(space.toString());
								s = "";
							}
						}
						out.newLine();
					}
					g_more_point = true;
					int n_max_sig = 0;
					final boolean more_point[] = new boolean[panel.size()];
					for (int k = 0; k < panel.size(); k++)
					{
						more_point[k] = true;
						wave = (jScopeMultiWave) panel.elementAt(k);
						wi = (MdsWaveInterface) wave.wi;
						if (wi == null || wi.signals == null)
							continue;
						if (wi.signals.length > n_max_sig)
							n_max_sig = wi.signals.length;
					}
					final int start_idx[][] = new int[panel.size()][n_max_sig];
					while (g_more_point)
					{
						g_more_point = false;
						for (int k = 0; k < panel.size(); k++)
						{
							wave = (jScopeMultiWave) panel.elementAt(k);
							wi = (MdsWaveInterface) wave.wi;
							if (wi == null || wi.signals == null)
								continue;
							if (!more_point[k])
							{
								for (@SuppressWarnings("unused")
								final Signal signal : wi.signals)
									out.write("                                   ");
								continue;
							}
							g_more_point = true;
							int j = 0;
							final double xmax = wave.GetWaveformMetrics().XMax();
							final double xmin = wave.GetWaveformMetrics().XMin();
							// int start_idx[] = new int[wi.signals.length];
							// for(int i = 0; i < wi.signals.length; i++)
							// start_idx[i] = 0;
							more_point[k] = false;
							for (int i = 0; i < wi.signals.length; i++)
							{
								s1 = "";
								s2 = "";
								if (wi.signals[i] != null && wi.signals[i].hasX())
								{
									for (j = start_idx[k][i]; j < wi.signals[i].getNumPoints(); j++)
									{
										if (wi.signals[i].getX(j) > xmin && wi.signals[i].getX(j) < xmax)
										{
											more_point[k] = true;
											s1 = "" + wi.signals[i].getX(j);
											s2 = "" + wi.signals[i].getY(j);
											start_idx[k][i] = j + 1;
											break;
										}
									}
								}
								out.write(s1);
								space.setLength(0);
								for (int u = 0; u < 25 - s1.length(); u++)
									space.append(' ');
								space.append(' ');
								out.write(space.toString());
								out.write(" ");
								out.write(s2);
								space.setLength(0);
								for (int u = 0; u < 25 - s2.length(); u++)
									space.append(' ');
								out.write(space.toString());
							}
						}
						out.newLine();
					}
					out.close();
				}
				catch (final IOException e)
				{
					System.out.println(e);
				}
			}
		}
	}

	public void SetDataServer(DataServerItem server_item, UpdateEventListener l) throws Exception
	{
		if (server_item == null || server_item.getName() == null)
			throw (new Exception("Defined null or empty data server name"));
		final DataProvider new_dp = server_item.getProvider();
		if (browse_sig != null && browse_sig.isShowing())
			browse_sig.setVisible(false);
		try
		{
			// Current data server Disconnection
			RemoveAllEvents(l);
			if (dp != null)
			{
				dp.removeConnectionListener((ConnectionListener) l);
				dp.close();
			}
			dp = new_dp;
			final int option = dp.inquireCredentials(GetFrameParent(), server_item);
			switch (option)
			{
			case DataProvider.LOGIN_OK:
				dp.setArgument(server_item.getArgument());
				break;
			case DataProvider.LOGIN_ERROR:
			case DataProvider.LOGIN_CANCEL:
				server_item = new DataServerItem();
				dp = DataServerItem.NotConnected;
			}
			if (dp != null)
				dp.addConnectionListener((ConnectionListener) l);
			if (server_item.isNotConnected())
			{
				// Check data server connection
				if (dp.getShots("0", "xx") == null)
					throw (new Exception("Cannot connect to " + server_item.getClassName() + " data server"));
			}
			ChangeDataProvider(dp);
			AddAllEvents(l);
			// create browse panel if defined
			if (server_item != null && server_item.getBrowseClass() != null && server_item.getBrowseUrl() != null)
			{
				try
				{
					final Class<?> cl = Class.forName("mds.provider.browser." + server_item.getBrowseClass());
					browse_sig = (jScopeBrowseSignals) cl.newInstance();
					browse_sig.setWaveContainer(this);
				}
				catch (final Exception e)
				{
					browse_sig = null;
					JOptionPane.showMessageDialog(this,
							"Unable to locate the signal server " + server_item.getBrowseUrl() + " : " + e.getMessage(),
							"alert", JOptionPane.ERROR_MESSAGE);
				}
			}
			else
			{
				browse_sig = null;
			}
			this.server_item = server_item;
		}
		catch (final IOException e)
		{
			this.server_item = new DataServerItem();
			this.dp = DataServerItem.NotConnected;
			ChangeDataProvider(dp);
			throw (new Exception(e.getMessage()));
		}
	}

	public void SetEvent(UpdateEventListener l, String event) throws IOException
	{
		this.event = AddRemoveEvent(l, this.event, event);
	}

	public void SetMainShot(String shot_str)
	{
		if (shot_str != null)
		{
			try
			{
				EvaluateMainShot(shot_str.trim());
			}
			catch (final IOException exc)
			{
				main_shot_str = null;
				main_shot_error = "Main Shots evaluations error : \n" + exc.getMessage();
				JOptionPane.showMessageDialog(this, main_shot_error, "alert SetMainShot", JOptionPane.ERROR_MESSAGE);
			}
		}
	}

	public void SetModifiedState(boolean state)
	{
		jScopeMultiWave w;
		for (int i = 0; i < getComponentNumber(); i++)
		{
			w = (jScopeMultiWave) getGridComponent(i);
			if (w != null && w.wi != null)
			{
				w.wi.setModified(state);
			}
		}
	}

	public void SetPrintEvent(UpdateEventListener l, String print_event) throws IOException
	{
		this.print_event = AddRemoveEvent(l, this.print_event, print_event);
	}

	public void SetTitle(String title)
	{
		this.title = title;
	}

public void ShowBrowseSignals()
	{
		if (browse_sig != null)
		{
			if (!browse_sig.isConnected())
			{
				try
				{
					browse_sig.connectToBrowser(server_item.getBrowseUrl());
					browse_sig.setTitle("URL : " + server_item.getBrowseUrl());
				}
				catch (final Exception e)
				{
					JOptionPane.showMessageDialog(this, e.getMessage(), "alert", JOptionPane.ERROR_MESSAGE);
				}
			}
			browse_sig.setVisible(true);
		}
		else
		{
			String msg;
			if (this.GetBrowseUrl() == null)
				msg = "Signals browser not yet implemented on this data server";
			else
			{
				msg = "jScope is unable to locate the signal server page at " + this.GetBrowseUrl();
				msg = msg + "\nModify browse_url property for this data server in jScope.properties file.";
			}
			JOptionPane.showMessageDialog(null, msg, "alert", JOptionPane.ERROR_MESSAGE);
		}
	}

	public void StartPrint(DocPrintJob prnJob, PrintRequestAttributeSet attrs)
	{
		try
		{
			getjScopeMultiWave();
			UpdateAllWave();
			PrintAllWaves(prnJob, attrs);
		}
		catch (final InterruptedException e)
		{}
		catch (final PrinterException e)
		{}
		catch (final Exception e)
		{}
	}

	public void StartUpdate()
	{
		if (!updateThread.isAlive())
		{
			updateThread = new UpdW();
			updateThread.start();
		}
		getjScopeMultiWave();
		updateThread.StartUpdate();
	}

	public void startUpdatingPanel()
	{
		progressMonitor = new ProgressMonitor(this, "Running a Long Task", "", 0, 0);
	}

	public void stopUpdatingPanel()
	{
		progressMonitor.close();
	}

	public void ToFile(PrintWriter out, String prompt) throws IOException
	{
		jScopeMultiWave w;
		MdsWaveInterface wi;
		WaveInterface.WriteLine(out, prompt + "title: ", title);
		if (server_item != null)
		{
			WaveInterface.WriteLine(out, prompt + "data_server_name: ", server_item.getName());
			WaveInterface.WriteLine(out, prompt + "data_server_class: ", server_item.getClassName());
			if (server_item.getArgument() != null)
				WaveInterface.WriteLine(out, prompt + "data_server_argument: ", server_item.getArgument());
			if (server_item.getUser() != null)
				WaveInterface.WriteLine(out, prompt + "data_server_user: ", server_item.getUser());
			if (server_item.getBrowseClass() != null)
				WaveInterface.WriteLine(out, prompt + "data_server_browse_class: ", server_item.getBrowseClass());
			if (server_item.getBrowseUrl() != null)
				WaveInterface.WriteLine(out, prompt + "data_server_browse_url: ", server_item.getBrowseUrl());
			if (server_item.getTunnelPort() != null)
				WaveInterface.WriteLine(out, prompt + "data_server_tunnel_port: ", server_item.getTunnelPort());
		}
		WaveInterface.WriteLine(out, prompt + "update_event: ", event);
		WaveInterface.WriteLine(out, prompt + "print_event: ", print_event);
		WaveInterface.WriteLine(out, prompt + "reversed: ", "" + reversed);
		out.println();
		WaveInterface.WriteLine(out, prompt + "global_1_1.experiment: ", def_vals.experiment_str);
		WaveInterface.WriteLine(out, prompt + "global_1_1.event: ", def_vals.upd_event_str);
		WaveInterface.WriteLine(out, prompt + "global_1_1.default_node: ", def_vals.def_node_str);
		WaveInterface.WriteLine(out, prompt + "global_1_1.shot: ", def_vals.shot_str);
		WaveInterface.WriteLine(out, prompt + "global_1_1.title: ", def_vals.title_str);
		WaveInterface.WriteLine(out, prompt + "global_1_1.xmax: ", def_vals.xmax);
		WaveInterface.WriteLine(out, prompt + "global_1_1.xmin: ", def_vals.xmin);
		WaveInterface.WriteLine(out, prompt + "global_1_1.x_label: ", def_vals.xlabel);
		WaveInterface.WriteLine(out, prompt + "global_1_1.ymax: ", def_vals.ymax);
		WaveInterface.WriteLine(out, prompt + "global_1_1.ymin: ", def_vals.ymin);
		WaveInterface.WriteLine(out, prompt + "global_1_1.y_label: ", def_vals.ylabel);
		WaveInterface.WriteLine(out, prompt + "global_1_1.horizontal_offset: ", "" + Waveform.GetHorizontalOffset());
		WaveInterface.WriteLine(out, prompt + "global_1_1.vertical_offset: ", "" + Waveform.GetVerticalOffset());
		out.println();
		out.println("Scope.columns: " + getColumns());
		final float normHeight[] = getNormalizedHeight();
		final float normWidth[] = getNormalizedWidth();
		final Dimension dim = getSize();
		for (int i = 0, c = 1, k = 0; i < getColumns(); i++, c++)
		{
			WaveInterface.WriteLine(out, prompt + "rows_in_column_" + c + ": ", "" + getComponentsInColumn(i));
			for (int j = 0, r = 1; j < getComponentsInColumn(i); j++, r++)
			{
				w = (jScopeMultiWave) getGridComponent(k);
				wi = (MdsWaveInterface) w.wi;
				out.println("\n");
//			WaveInterface.WriteLine(out, prompt + "plot_" + r + "_" + c + ".height: "          , ""+w.getSize().height );
				WaveInterface.WriteLine(out, prompt + "plot_" + r + "_" + c + ".height: ",
						"" + (int) (dim.height * normHeight[k]));
				WaveInterface.WriteLine(out, prompt + "plot_" + r + "_" + c + ".grid_mode: ", "" + w.grid_mode);
				if (wi != null)
					wi.ToFile(out, prompt + "plot_" + r + "_" + c + ".");
				k++;
			}
		}
		out.println();
		for (int i = 1, pos = 0; i < getColumns(); i++)
		{
			// w = (jScopeMultiWave)getGridComponent(k);
			// wi = (MdsWaveInterface)w.wi;
			// pos += (int)(((float)w.getSize().width/ getSize().width) * 1000.);
			pos += (int) (normWidth[i - 1] * 1000.);
			WaveInterface.WriteLine(out, prompt + "vpane_" + i + ": ", "" + pos);
			getComponentsInColumn(i);
		}
	}

	public synchronized void UpdateAllWave() throws Exception
	{
		WaveContainerEvent wce;
		try
		{
			if (wave_all == null)
				abort = true;
			else
				abort = false;
			if (def_vals != null && !def_vals.getIsEvaluated())
			{
				dp.setEnvironment(def_vals.getPublicVariables());
				def_vals.setIsEvaluated(true);
			}
			for (int i = 0, k = 0; i < 4 && !abort; i++)
			{
				for (int j = 0; j < rows[i]; j++, k++)
				{
					if (wave_all[k].wi != null && wave_all[k].isWaveformVisible())
						((MdsWaveInterface) wave_all[k].wi).Update();
				}
			}
			// Initialize wave evaluation
			for (int i = 0, k = 0; i < 4 && !abort; i++)
			{
				for (int j = 0; j < rows[i] && !abort; j++, k++)
				{
					if (wave_all[k].wi != null && wave_all[k].wi.error == null && wave_all[k].isWaveformVisible())
					{
						wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE,
								"Start Evaluate column " + (i + 1) + " row " + (j + 1));
						try
						{
							dispatchWaveContainerEvent(wce);
							((MdsWaveInterface) wave_all[k].wi).StartEvaluate();
						}
						catch (final Exception exc)
						{
							// exc.printStackTrace();
						}
					}
				}
			}
			synchronized (mainShotLock)
			{
				if (main_shots != null)
				{
					for (int l = 0; l < main_shots.length && !abort; l++)
					{
						for (int i = 0, k = 0; i < 4 && !abort; i++)
						{
							for (int j = 0; j < rows[i] && !abort; j++, k++)
							{
								if (wave_all[k].wi != null && wave_all[k].wi.error == null
										&& wave_all[k].isWaveformVisible() && wave_all[k].wi.num_waves != 0
										&& ((MdsWaveInterface) wave_all[k].wi).UseDefaultShot())
								{
									wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE,
											"Update signal column " + (i + 1) + " row " + (j + 1) + " main shot "
													+ main_shots[l]);
									dispatchWaveContainerEvent(wce);
									((MdsWaveInterface) wave_all[k].wi).EvaluateShot(main_shots[l]);
									if (((MdsWaveInterface) wave_all[k].wi).allEvaluated())
									{
										if (wave_all[k].wi != null)
											wave_all[k].Update(wave_all[k].wi);
									}
								}
							}
						}
					}
				}
			}
			// Evaluate evaluate other shot
			for (int i = 0, k = 0; i < 4; i++)
			{
				for (int j = 0; j < rows[i]; j++, k++)
				{
					if (wave_all != null && wave_all[k] != null)
					{
						if (wave_all[k].wi != null && wave_all[k].wi.error == null && wave_all[k].isWaveformVisible()
								&& wave_all[k].wi.num_waves != 0 && !abort)
						{
							if (((MdsWaveInterface) wave_all[k].wi).allEvaluated())
								continue;
							wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE,
									"Evaluate wave column " + (i + 1) + " row " + (j + 1));
							dispatchWaveContainerEvent(wce);
							((MdsWaveInterface) wave_all[k].wi).EvaluateOthers();
						}
						if (wave_all[k].wi != null && wave_all[k].isWaveformVisible())
							wave_all[k].Update(wave_all[k].wi);
					}
				}
			}
			for (int i = 0, k = 0; i < 4; i++)
			{
				for (int j = 0; j < rows[i]; j++, k++)
				{
					if (wave_all != null && wave_all[k] != null)
					{
						((MdsWaveInterface) wave_all[k].wi).allEvaluated();
					}
				}
			}
			wave_all = null;
		}
		catch (final Exception e)
		{
//            e.printStackTrace();
			RepaintAllWave();
			// throw (e);
		}
	}

	public void UpdateHeight()
	{
		float height = 0;
		jScopeMultiWave w;
		ph = new float[getComponentNumber()];
		for (int j = 0, k = 0; j < columns; j++)
		{
			height = 0;
			for (int i = 0; i < rows[j]; i++)
			{
				w = (jScopeMultiWave) getGridComponent(k + i);
				height += w.wi.height;
			}
			for (int i = 0; i < rows[j]; i++, k++)
			{
				w = (jScopeMultiWave) getGridComponent(k);
				if (height == 0)
				{
					k -= i;
					for (i = 0; i < rows[j]; i++)
						ph[k++] = (float) 1. / rows[j];
					break;
				}
				ph[k] = w.wi.height / height;
			}
		}
		invalidate();
	}
}
