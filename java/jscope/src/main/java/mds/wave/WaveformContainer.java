package mds.wave;

import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;
import java.io.*;
import java.util.Properties;
import java.util.Vector;

import javax.swing.*;

/**
 * A MultiWaveform container
 *
 * @see RowColumnLayout
 * @see RowColumnContainer
 * @see WaveformManager
 * @see MultiWaveform
 */
public class WaveformContainer extends RowColumnContainer implements WaveformManager, WaveformListener, Printable
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	private static Waveform copy_waveform = null;
	public static void disableDoubleBuffering(Component c)
	{
		final RepaintManager currentManager = RepaintManager.currentManager(c);
		currentManager.setDoubleBufferingEnabled(false);
	}
	public static void enableDoubleBuffering(Component c)
	{
		final RepaintManager currentManager = RepaintManager.currentManager(c);
		currentManager.setDoubleBufferingEnabled(true);
	}
	private Waveform sel_wave;
	int mode = Waveform.MODE_ZOOM, grid_mode = Grid.IS_DOTTED, x_grid_lines = 5, y_grid_lines = 5;
	protected boolean reversed = false;
	protected Font font = new Font("Helvetica", Font.PLAIN, 12);
	protected WavePopup wave_popup;
	private final Vector<WaveContainerListener> wave_container_listener = new Vector<>();
	protected boolean print_with_legend = false;

	protected boolean print_bw = false;

	protected String save_as_txt_directory = null;

	public WaveformContainer()
	{
		super();
		CreateWaveformContainer(true);
	}

	/**
	 * Constructs a new WaveformContainer with a number of column and component in
	 * column.
	 *
	 * @param rows an array of number of component in column
	 */
	public WaveformContainer(int rows[], boolean add_component)
	{
		super(rows, null);
		CreateWaveformContainer(add_component);
	}

	/**
	 * Add MultiWaveform to the container
	 *
	 * @param c an array of MultiWaveform to add
	 */
	public void AddComponents(Component c[])
	{
		super.add(c);
		/*
		 * for(int i = 0; i < c.length; i++) if(c[i] instanceof Waveform) {
		 * ((Waveform)c[i]).addWaveformListener(this); }
		 */
	}

	/**
	 * Adds the specified waveform container listener to receive WaveContainerEvent
	 * events from this WaveformContainer.
	 *
	 * @param l the waveform container listener
	 */
	public synchronized void addWaveContainerListener(WaveContainerListener l)
	{
		if (l == null)
		{
			return;
		}
		wave_container_listener.addElement(l);
	}

	@Override
	public void allSameScale(Waveform curr_w)
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				if (w != curr_w)
					w.SetScale(curr_w);
		}
	}

	@Override
	public void allSameXScale(Waveform curr_w)
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null && w != curr_w)
				w.SetXScale(curr_w);
		}
	}

	@Override
	public void allSameXScaleAutoY(Waveform curr_w)
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.SetXScaleAutoY(curr_w);
		}
	}

	@Override
	public void allSameYScale(Waveform curr_w)
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				if (w != curr_w)
					w.SetYScale(curr_w);
		}
	}

	synchronized public void appendUpdateWaveforms()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.appendUpdate();
		}
	}

	@Override
	public void autoscaleAll()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.Autoscale();
		}
	}

	@Override
	public void autoscaleAllImages()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null && w.IsImage())
				w.Autoscale();
		}
	}

	@Override
	public void autoscaleAllY()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.AutoscaleY();
		}
	}

	/**
	 * Return a new MultiWaveform component
	 *
	 * @return a MulitiWaveform components
	 */
	public Component CreateWaveComponent()
	{
		final Component[] c = CreateWaveComponents(1);
		return c[0];
	}

	/**
	 * Create an array of MultiWaveform
	 *
	 * @param num dimension of return array
	 * @return an array of MultiWaveform
	 */
	protected Component[] CreateWaveComponents(int num)
	{
		final Component c[] = new Component[num];
		MultiWaveform wave;
		for (int i = 0; i < c.length; i++)
		{
			wave = new MultiWaveform();
			wave.addWaveformListener(this);
			SetWaveParams(wave);
			c[i] = wave;
		}
		return c;
	}

	/**
	 * Initialize WaveformContaine
	 */
	private void CreateWaveformContainer(boolean create_component)
	{
		if (create_component)
		{
			final Component c[] = CreateWaveComponents(getComponentNumber());
			super.add(c);
		}
		addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(MouseEvent e)
			{
				final Dimension scr_dim = getToolkit().getScreenSize();
				final Waveform w = (Waveform) e.getSource();
				final int x = e.getX();
				final int y = e.getY();
				if (wave_popup != null)
				{
					final Point p = new Point();
					Component co = w;
					Dimension wp_size = wave_popup.getSize();
					if (wp_size.height == 0 || wp_size.width == 0)
					{
						wave_popup.Show(w, x, y, 0, 0);
						wp_size = wave_popup.getSize();
					}
					while (co != null)
					{
						p.x += co.getLocation().x;
						p.y += co.getLocation().y;
						co = co.getParent();
					}
					int tran_x = 0;
					int tran_y = 0;
					if (y + p.y + wp_size.height > scr_dim.height)
						tran_y = y + p.y + wp_size.height - scr_dim.height + 20;
					if (x + p.x + wp_size.width > scr_dim.width)
						tran_x = x + p.x + wp_size.width - scr_dim.width + 20;
					wave_popup.Show(w, x, y, tran_x, tran_y);
				}
			}
		});
	}

	/**
	 * Deselect waveform.
	 *
	 * @see Waveform
	 * @see MultiWaveform
	 */
	@Override
	public void deselect()
	{
		if (sel_wave != null)
			sel_wave.DeselectWave();
		sel_wave = null;
	}

	/**
	 * Processes wave container events occurring on this WaveformContainer by
	 * dispatching them to any registered WaveContainerListener objects.
	 *
	 * @param e the wave container event
	 */
	protected void dispatchWaveContainerEvent(WaveContainerEvent e)
	{
		if (wave_container_listener != null)
		{
			for (int i = 0; i < wave_container_listener.size(); i++)
			{
				wave_container_listener.elementAt(i).processWaveContainerEvent(e);
			}
		}
	}

	/**
	 * Get current waveform selected as copy source
	 *
	 * @return copy source waveform
	 */
	@Override
	public Waveform getCopySource()
	{ return copy_waveform; }

	private String getFileName(Waveform w)
	{
		String out;
		final Properties prop = new Properties();
		final String pr = w.getProperties();
		try
		{
			prop.load(new StringReader(pr));
			out = w.GetTitle();
			out = out + "_" + prop.getProperty("x_pix");
			out = out + "_" + prop.getProperty("y_pix");
			out = out + "_" + prop.getProperty("time");
			out = out.replace('.', '-') + ".txt";
		}
		catch (final Exception e)
		{
			out = null;
		}
		return out;
	}

	@Override
	public Component getMaximizeComponent()
	{ return super.getMaximizeComponent(); }

	public int GetMode()
	{
		return mode;
	}

	public Waveform GetSelectPanel()
	{
		Waveform w;
		if (sel_wave == null)
		{
			int i;
			for (i = 0; i < getGridComponentCount() && GetWavePanel(i) != null
					&& (GetWavePanel(i).GetShowSignalCount() != 0 || GetWavePanel(i).IsImage()); i++);
			if (i == getGridComponentCount())
			{
				final Component c[] = this.CreateWaveComponents(1);
				w = (Waveform) c[0];
			}
			else
			{
				w = GetWavePanel(i);
			}
		}
		else
			return sel_wave;
		return w;
	}

	@Override
	public int getWaveformCount()
	{ return this.getGridComponentCount(); }

	/**
	 * Return indexn of an added MultiWaveform
	 *
	 * @param w The MultiWaveform
	 * @return the MultiWaveform index
	 */
	public int GetWaveIndex(Waveform w)
	{
		int idx;
		for (idx = 0; idx < getGridComponentCount() && GetWavePanel(idx) != w; idx++);
		if (idx < getGridComponentCount())
			return idx;
		else
			return -1;
	}

	public Waveform GetWavePanel(int idx)
	{
		final Component c = getGridComponent(idx);
		if (c instanceof MultiWaveform || c instanceof Waveform)
			return (Waveform) c;
		else
			return null;
	}

	@Override
	public Point getWavePosition(Waveform w)
	{
		return getComponentPosition(w);
	}

	/**
	 * Get current selected waveform.
	 *
	 * @return current selected waveform or null
	 * @see Waveform
	 * @see MultiWaveform
	 */
	@Override
	public Waveform gGetSelected()
	{
		return sel_wave;
	}

	public boolean isMaximize(Waveform w)
	{
		return super.isMaximize();
	}

	public void LoadFileConfiguration()
	{}

	@Override
	public void maximizeComponent(Waveform w)
	{
		super.maximizeComponent(w);
	}

	@Override
	public void notifyChange(Waveform dest, Waveform source)
	{
		dest.Copy(source);
	}

	@Override
	public int print(Graphics g, PageFormat pf, int pageIndex) throws PrinterException
	{
		int st_x = 0, st_y = 0;
		double height = pf.getImageableHeight();
		double width = pf.getImageableWidth();
		final Graphics2D g2 = (Graphics2D) g;
		final String ver = System.getProperty("java.version");
		if (pageIndex == 0)
		{
			// fix page margin error on jdk 1.2.X
			if (ver.indexOf("1.2") != -1)
			{
				if (pf.getOrientation() == PageFormat.LANDSCAPE)
				{
					st_y = -13;
					st_x = 15;
					width -= 5;
				}
				else
				{
					// st_x = 10;
					st_y = -5;
					width -= 25;
					height -= 25;
				}
			}
			g2.translate(pf.getImageableX(), pf.getImageableY());
			PrintAll(g2, st_x, st_y, (int) height, (int) width);
			return Printable.PAGE_EXISTS;
		}
		else
			return Printable.NO_SUCH_PAGE;
	}

	public void PrintAll(Graphics g, int st_x, int st_y, int height, int width)
	{
		Waveform w;
		int i, j, k = 0;
		int pix = 1;
		if (GetWavePanel(0).grid_mode == 2)// Grid.IS_NONE mode
			pix = 0;
		int curr_height = 0;
		int curr_width = 0;
		int px = 0;
		int py = 0;
		int pos = 0;
		for (i = k = 0, px = st_x; i < rows.length; i++)
		{
			if (rows[i] == 0)
				continue;
			g.translate(px, 0);
			curr_width = (int) (width * ((RowColumnLayout) getLayout()).getPercentWidth(i) + 0.9);
			if (curr_width == 0)
			{
				k += rows[i];
				continue;
			}
			for (j = pos = 0, py = st_y; j < rows[i]; j++)
			{
				curr_height = (int) (height * ((RowColumnLayout) getLayout()).getPercentHeight(k) + 0.9);
				if (curr_height == 0)
				{
					k++;
					continue;
				}
				g.translate(0, py);
				if (j == rows[i] - 1 && pos + curr_height != height)
					curr_height = height - pos;
				g.setClip(0, 0, curr_width, curr_height);
				w = GetWavePanel(k);
				if (w != null)
				{
					int print_mode = Waveform.PRINT;
					if (print_with_legend)
						print_mode |= MultiWaveform.PRINT_LEGEND;
					if (print_bw)
						print_mode |= MultiWaveform.PRINT_BW;
					disableDoubleBuffering(w);
					w.paint(g, new Dimension(curr_width, curr_height), print_mode);
					enableDoubleBuffering(w);
				}
				py = curr_height - pix;
				pos += (curr_height - pix);
				k++;
			}
			px = curr_width - pix;
			g.translate(0, -pos - st_y + py);
		}
	}

	/**
	 * process waveform event on this container
	 *
	 * @param e the waveform event
	 */
	@Override
	public void processWaveformEvent(WaveformEvent e)
	{
		final Waveform w = (Waveform) e.getSource();
		switch (e.getID())
		{
		case WaveformEvent.BROADCAST_SCALE:
			allSameScale(w);
			return;
		case WaveformEvent.COPY_PASTE:
			if (copy_waveform != null)
				notifyChange(w, copy_waveform);
			return;
		case WaveformEvent.COPY_CUT:
			setCopySource(w);
			return;
		case WaveformEvent.PROFILE_UPDATE:
			return;
		case WaveformEvent.POINT_UPDATE:
		case WaveformEvent.MEASURE_UPDATE:
			if (w.GetMode() == Waveform.MODE_POINT)
			{
				double x = e.point_x;
				final double y = e.point_y;
				if (w.IsImage())
					x = e.delta_x;
				else if (e.is_mb2)
					allSameXScaleAutoY(w);
				// Set x to time_value allows pannels synchronization from 2D
				// signal viewed in MODE_YX
				if (!Double.isNaN(e.time_value))
					x = e.time_value;
				UpdatePoints(x, y, (Waveform) e.getSource());
			}
			/*
			 * if(!w.IsImage() && show_measure) { e = new WaveformEvent(e.getSource(),
			 * WaveformEvent.MEASURE_UPDATE, e.point_x, e.point_y, e.delta_x, e.delta_y, 0,
			 * e.signal_idx); }
			 */
			break;
		case WaveformEvent.POINT_IMAGE_UPDATE:
			break;
		}
		final WaveContainerEvent we = new WaveContainerEvent(this, e);
		dispatchWaveContainerEvent(we);
	}

	public void RemoveAllSignals()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.Erase();
		}
		System.gc();
	}

	/**
	 * Removes the specified waveform container listener so that it no longer
	 * receives WaveContainerEvent events from this WaveformContainer.
	 *
	 * @param l the waveform container listener
	 */
	public synchronized void removeContainerListener(ActionListener l)
	{
		if (l == null)
		{
			return;
		}
		wave_container_listener.removeElement(l);
	}

	@Override
	public void removePanel(Waveform w)
	{
		if (w == sel_wave)
			sel_wave = null;
		if (w.IsCopySelected())
		{
			copy_waveform = null;
			w.SetCopySelected(false);
		}
		super.removeComponent(w);
	}

	/**
	 * Remove current MultiWaveform selected
	 */
	public void RemoveSelection()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount() && copy_waveform != null; i++)
		{
			w = GetWavePanel(i);
			if (w != null)
			{
				if (w.IsCopySelected())
				{
					copy_waveform = null;
					w.SetCopySelected(false);
					break;
				}
			}
		}
	}

	public void RepaintAllWaves()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.repaint();
		}
	}

	@Override
	public void resetAllScales()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.ResetScales();
		}
	}

	public void ResetDrawPanel(int _row[])
	{
		int n_wave = 0;
		int num = 0;
		for (int i = 0; i < _row.length; i++)
		{
			n_wave = (_row[i] - rows[i]);
			if (n_wave > 0)
				num += n_wave;
		}
		Component c[] = null;
		if (num > 0)
			c = CreateWaveComponents(num);
		update(_row, c);
		if (sel_wave != null)
			sel_wave.SelectWave();
		// System.gc();
	}

	public void SaveAsText(Waveform w, boolean all)
	{
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
			final String fname = getFileName(w);
			if (fname != null)
				file_diag.setSelectedFile(new File(fname));
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
				String s1 = "", s2 = "";
				final StringBuffer space = new StringBuffer();
				try
				{
					final BufferedWriter out = new BufferedWriter(new FileWriter(txtsig_file));
					out.write("% Title: " + w.GetTitle());
					out.newLine();
					final Properties prop = new Properties();
					final String pr = w.getProperties();
					try
					{
						prop.load(new StringReader(pr));
						out.write("% Expression: " + prop.getProperty("expr"));
						out.newLine();
						out.write("% x_pixel: " + prop.getProperty("x_pix"));
						out.newLine();
						out.write("% y_pixel: " + prop.getProperty("y_pix"));
						out.newLine();
						out.write("% time: " + prop.getProperty("time"));
						out.newLine();
					}
					catch (final Exception e)
					{}
					final double xmax = w.GetWaveformMetrics().XMax();
					final double xmin = w.GetWaveformMetrics().XMin();
					s1 = "";
					s2 = "";
					final int nPoint = w.waveform_signal.getNumPoints();
					for (int j = 0; j < nPoint; j++)
					{
						final double x = w.waveform_signal.getX(j);
						if (x > xmin && x < xmax)
						{
							s1 = "" + x;
							s2 = "" + w.waveform_signal.getY(j);
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
							out.newLine();
						}
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

	/**
	 * Select a waveform
	 *
	 * @param w waveform to select
	 * @see Waveform
	 * @see MultiWaveform
	 */
	@Override
	public void select(Waveform w)
	{
		deselect();
		sel_wave = w;
		sel_wave.SelectWave();
	}

	public void SetColors(Color colors[], String colors_name[])
	{
		for (int i = 0, k = 0; i < rows.length; i++)
			for (int j = 0; j < rows[i]; j++, k++)
			{
				final Waveform w = GetWavePanel(k);
				if (w != null)
					Waveform.SetColors(colors, colors_name);
			}
	}

	/**
	 * Set copy source waveform
	 *
	 * @param w copy source waveform
	 * @see Waveform
	 * @see MultiWaveform
	 */
	@Override
	public void setCopySource(Waveform w)
	{
		/*
		 * if(w != null) w.SetCopySelected(true); else if(copy_waveform != null)
		 * copy_waveform.SetCopySelected(false);
		 */
		if (w != null)
		{
			if (w == copy_waveform)
			{
				w.SetCopySelected(false);
				copy_waveform = null;
				return;
			}
			else
				w.SetCopySelected(true);
		}
		if (copy_waveform != null)
			copy_waveform.SetCopySelected(false);
		copy_waveform = w;
	}

	public void SetFont(Font font)
	{
		Waveform.SetFont(font);
	}

	public void SetGridMode(int grid_mode)
	{
		Waveform w;
		this.grid_mode = grid_mode;
		final boolean int_label = (grid_mode == 2 ? false : true);
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.SetGridMode(grid_mode, int_label, int_label);
		}
	}

	public void SetGridStep(int x_grid_lines, int y_grid_lines)
	{
		Waveform w;
		this.x_grid_lines = x_grid_lines;
		this.y_grid_lines = y_grid_lines;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.SetGridSteps(x_grid_lines, y_grid_lines);
		}
	}

	public void setLegendMode(int legend_mode)
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null && w instanceof MultiWaveform)
				((MultiWaveform) w).setLegendMode(legend_mode);
		}
	}

	public void SetMode(int mode)
	{
		Waveform w;
		this.mode = mode;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
			{
				if (copy_waveform == w && w.mode == Waveform.MODE_COPY && mode != Waveform.MODE_COPY)
				{
					RemoveSelection();
					copy_waveform = null;
				}
				w.SetMode(mode);
			}
		}
	}

	public void SetParams(int mode, int grid_mode, int legend_mode, int x_grid_lines, int y_grid_lines,
			boolean reversed)
	{
		SetReversed(reversed);
		SetMode(mode);
		SetGridMode(grid_mode);
		SetGridStep(x_grid_lines, y_grid_lines);
		setLegendMode(legend_mode);
	}

	/**
	 * Set popup menu to this container
	 *
	 * @param wave_popup the popup menu
	 */
	public void setPopupMenu(WavePopup wave_popup)
	{
		this.wave_popup = wave_popup;
		wave_popup.setParent(this);
	}

	public void setPrintBW(boolean print_bw)
	{ this.print_bw = print_bw; }

	public void setPrintWithLegend(boolean print_with_legend)
	{ this.print_with_legend = print_with_legend; }

	public void SetReversed(boolean reversed)
	{
		Waveform w;
		this.reversed = reversed;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.SetReversed(reversed);
		}
	}

	/**
	 * Enable / disable show measurament
	 *
	 * @param state shoe measurament state
	 */
	@Override
	public void setShowMeasure(boolean state)
	{
		if (state)
		{
			Waveform w;
			for (int i = 0; i < getGridComponentCount(); i++)
			{
				w = GetWavePanel(i);
				if (w != null)
					w.show_measure = false;
			}
		}
	}

	/**
	 * Set current MultiWaveform parameters
	 *
	 * @param w the MultiWaveform to set params
	 */
	public void SetWaveParams(Waveform w)
	{
		final boolean int_label = (grid_mode == 2 ? false : true);
		w.SetMode(mode);
		w.SetReversed(reversed);
		w.SetGridMode(grid_mode, int_label, int_label);
		w.SetGridSteps(x_grid_lines, y_grid_lines);
	}

	public void stopPlaying()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.StopFrame();
		}
	}

	/* synchronized */@Override
	public void updatePoints(double x, Waveform curr_w)
	{
		UpdatePoints(x, Double.NaN, curr_w);
	}

	/**
	 * Update crosshair position
	 *
	 * @param curr_x x axis position
	 * @param w      a waveform to update cross
	 * @see Waveform
	 * @see MultiWaveform
	 */
	/* synchronized */public void UpdatePoints(double x, double y, Waveform curr_w)
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null && w != curr_w)
				w.UpdatePoint(x, y);
		}
	}

	synchronized public void updateWaveforms()
	{
		Waveform w;
		for (int i = 0; i < getGridComponentCount(); i++)
		{
			w = GetWavePanel(i);
			if (w != null)
				w.Update();
		}
	}
}
