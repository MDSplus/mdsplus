package mds.jscope;

import java.awt.*;
import java.awt.datatransfer.Clipboard;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.awt.print.*;
import java.io.*;
import java.net.URL;
import java.util.*;

import javax.print.*;
import javax.print.attribute.*;
import javax.print.attribute.standard.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.plaf.FontUIResource;
import javax.swing.plaf.basic.BasicArrowButton;

import mds.connection.*;
import mds.provider.*;
import mds.wave.*;

public class jScopeFacade extends JFrame implements ActionListener, ItemListener, WindowListener, WaveContainerListener,
		UpdateEventListener, ConnectionListener, Printable
{
	private static final long serialVersionUID = 1L;

	class FileFilter implements FilenameFilter
	{
		String fname = null;

		FileFilter(String fname)
		{
			this.fname = fname;
		}

		@Override
		public boolean accept(File dir, String name)
		{
			if (name.indexOf(fname) == 0)
				return true;
			else
				return false;
		}
	}

	class MonMemory extends Thread
	{
		@Override
		public void run()
		{
			setName("Monitor Thread");
			try
			{
				while (true)
				{
					// System.out.println
					SetWindowTitle("Free :" + (int) (Runtime.getRuntime().freeMemory() / 1024) + " " + "Total :"
							+ (int) (Runtime.getRuntime().totalMemory()) / 1024 + " " + "USED :"
							+ (int) ((Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()) / 1024.));
					sleep(2000, 0);
					// waitTime(2000);
				}
			}
			catch (final InterruptedException e)
			{}
		}

		synchronized void waitTime(long t) throws InterruptedException
		{
			wait(t);
		}
	}

	class PubVarDialog extends JDialog implements ActionListener
	{
		/**
		 *
		 */
		private static final long serialVersionUID = 1L;
		private final JButton apply, cancel, save, reset;
		jScopeFacade dw;
		private final Vector<String> expr_list = new Vector<>();
		boolean is_pv_apply = false;
		private final Vector<String> name_list = new Vector<>();

		PubVarDialog(Frame fw)
		{
			super(fw, "Public Variables", false);
			dw = (jScopeFacade) fw;
			final GridBagConstraints c = new GridBagConstraints();
			final GridBagLayout gridbag = new GridBagLayout();
			getContentPane().setLayout(gridbag);
			c.insets = new Insets(2, 2, 2, 2);
			c.fill = GridBagConstraints.NONE;
			c.anchor = GridBagConstraints.CENTER;
			c.gridwidth = 1;
			JLabel lab = new JLabel("Name");
			gridbag.setConstraints(lab, c);
			getContentPane().add(lab);
			c.gridwidth = GridBagConstraints.REMAINDER;
			lab = new JLabel("Expression");
			gridbag.setConstraints(lab, c);
			getContentPane().add(lab);
			JTextField txt;
			c.anchor = GridBagConstraints.WEST;
			c.fill = GridBagConstraints.BOTH;
			for (int i = 0; i < MAX_VARIABLE; i++)
			{
				c.gridwidth = 1;
				txt = new JTextField(10);
				gridbag.setConstraints(txt, c);
				getContentPane().add(txt);
				c.gridwidth = GridBagConstraints.REMAINDER;
				txt = new JTextField(30);
				gridbag.setConstraints(txt, c);
				getContentPane().add(txt);
			}
			final JPanel p = new JPanel();
			p.setLayout(new FlowLayout(FlowLayout.CENTER));
			apply = new JButton("Apply");
			apply.addActionListener(this);
			p.add(apply);
			save = new JButton("Save");
			save.addActionListener(this);
			p.add(save);
			reset = new JButton("Reset");
			reset.addActionListener(this);
			p.add(reset);
			cancel = new JButton("Cancel");
			cancel.addActionListener(this);
			p.add(cancel);
			c.gridwidth = GridBagConstraints.REMAINDER;
			gridbag.setConstraints(p, c);
			getContentPane().add(p);
		}

		@Override
		public void actionPerformed(ActionEvent e)
		{
			final Object ob = e.getSource();
			if (ob == apply)
			{
				dw.setPublicVariables(getPublicVar());
				dw.UpdateAllWaves();
			}
			if (ob == save)
			{
				SavePubVar();
			}
			if (ob == reset)
			{
				SetPubVar();
			}
			if (ob == cancel)
			{
				is_pv_apply = false;
				setVisible(false);
			}
		}

		public void fromFile(Properties pr, String prompt) throws IOException
		{
			String prop;
			int idx = 0;
			name_list.removeAllElements();
			expr_list.removeAllElements();
			while ((prop = pr.getProperty(prompt + idx)) != null && idx < MAX_VARIABLE)
			{
				final StringTokenizer st = new StringTokenizer(prop, "=");
				final String name = st.nextToken();
				String expr = st.nextToken("");
				expr = expr.substring(expr.indexOf('=') + 1);// remove first = character in the expression
				name_list.insertElementAt(name.trim(), idx);
				expr_list.insertElementAt(expr.trim(), idx);
				idx++;
			}
		}

		public String getCurrentPublicVar()
		{
			String txt1, txt2, str;
			final StringBuffer buf = new StringBuffer();
			final Container p = getContentPane();
			for (int i = 2; i < MAX_VARIABLE * 2; i += 2)
			{
				txt1 = ((JTextField) p.getComponent(i)).getText();
				txt2 = ((JTextField) p.getComponent(i + 1)).getText();
				if (txt1.length() != 0 && txt2.length() != 0)
				{
					if (txt1.indexOf("_") != 0)
						str = "public _" + txt1 + " = ( " + txt2 + ";)";
					else
						str = "public " + txt1 + " = (" + txt2 + ";)";
					buf.append(str);
				}
			}
			return (new String(buf));
		}

		public String getPublicVar()
		{
			if (is_pv_apply)
				return getCurrentPublicVar();
			String txt1, txt2, str;
			final StringBuffer buf = new StringBuffer();
			for (int i = 0; i < name_list.size() && i < MAX_VARIABLE; i++)
			{
				txt1 = name_list.elementAt(i);
				txt2 = expr_list.elementAt(i);
				if (txt1.length() != 0 && txt2.length() != 0)
				{
					if (txt1.indexOf("_") != 0)
						str = "public _" + txt1 + " = (" + txt2 + ");";
					else
						str = "public " + txt1 + " = (" + txt2 + ");";
					buf.append(str);
				}
			}
			return (new String(buf));
		}

		private void SavePubVar()
		{
			String txt1, txt2;
			if (name_list.size() != 0)
				name_list.removeAllElements();
			if (expr_list.size() != 0)
				expr_list.removeAllElements();
			final Container p = getContentPane();
			for (int i = 2, j = 0; j < MAX_VARIABLE; i += 2, j++)
			{
				txt1 = ((JTextField) p.getComponent(i)).getText();
				txt2 = ((JTextField) p.getComponent(i + 1)).getText();
				if (txt1.length() != 0 && txt2.length() != 0)
				{
					name_list.insertElementAt(new String(txt1), j);
					expr_list.insertElementAt(new String(txt2), j);
				}
			}
			dw.setChange(true);
		}

		private void SetPubVar()
		{
			final Container p = getContentPane();
			for (int i = 2, j = 0; j < name_list.size() && j < MAX_VARIABLE; i += 2, j++)
			{
				((JTextField) p.getComponent(i)).setText(name_list.elementAt(j));
				((JTextField) p.getComponent(i + 1)).setText(expr_list.elementAt(j));
			}
		}

		public void Show()
		{
			is_pv_apply = true;
			SetPubVar();
			pack();
			setLocationRelativeTo(dw);
			setVisible(true);
		}

		public void toFile(PrintWriter out, String prompt)
		{
			for (int i = 0; i < name_list.size() && i < MAX_VARIABLE; i++)
			{
				out.println(prompt + i + ": " + name_list.elementAt(i) + " = " + expr_list.elementAt(i));
			}
			out.println("");
		}
	}

	/**
	 * Switch the between the Windows, Motif, Mac, and the Java Look and Feel
	 */
	class ToggleUIListener implements ItemListener
	{
		@Override
		public void itemStateChanged(ItemEvent e)
		{
			final Component root = jScopeFacade.this;
			root.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
			final JRadioButtonMenuItem rb = (JRadioButtonMenuItem) e.getSource();
			try
			{
				if (rb.isSelected() && rb.getText().equals("Windows Style Look and Feel"))
				{
					// currentUI = "Windows";
					UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
					SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
				}
				else if (rb.isSelected() && rb.getText().equals("Macintosh Look and Feel"))
				{
					// currentUI = "Macintosh";
					UIManager.setLookAndFeel("com.sun.java.swing.plaf.mac.MacLookAndFeel");
					SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
				}
				else if (rb.isSelected() && rb.getText().equals("Motif Look and Feel"))
				{
					// currentUI = "Motif";
					UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");
					SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
				}
				else if (rb.isSelected() && rb.getText().equals("Java Look and Feel"))
				{
					// currentUI = "Metal";
					UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
					SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
				}
				jScopeUpdateUI();
				/*
				 * jScope.jScopeSetUI(font_dialog); jScope.jScopeSetUI(setup_default);
				 * jScope.jScopeSetUI(color_dialog); jScope.jScopeSetUI(pub_var_diag);
				 * jScope.jScopeSetUI(server_diag); jScope.jScopeSetUI(file_diag);
				 */
			}
			catch (final UnsupportedLookAndFeelException exc)
			{
				// Error - unsupported L&F
				rb.setEnabled(false);
				System.err.println("Unsupported LookAndFeel: " + rb.getText());
				// Set L&F to JLF
				try
				{
					// currentUI = "Metal";
					metalMenuItem.setSelected(true);
					UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
					SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
				}
				catch (final Exception exc2)
				{
					exc2.printStackTrace();
					System.err.println("Could not load LookAndFeel: " + exc2);
					exc2.printStackTrace();
				}
			}
			catch (final Exception exc)
			{
				rb.setEnabled(false);
				exc.printStackTrace();
				System.err.println("Could not load LookAndFeel: " + rb.getText());
				exc.printStackTrace();
			}
			root.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
		}
	}

	static boolean enableNetworkSelection = true;// If enableNetworkSelection == false and mdsDataServer null and
	static public boolean is_debug = false;
	final static String JSCOPE_CONFIGURATIONS;
	final static String JSCOPE_PALETTE;
	final static String JSCOPE_PROFILE;
	final static String JSCOPE_PROPERTIES;
	final static String JSCOPE_SERVERS;
	// LookAndFeel class names
	static String macClassName = "com.sun.java.swing.plaf.mac.MacLookAndFeel";
	public static final int MAX_NUM_SHOT = 30;
	public static final int MAX_VARIABLE = 10;
	// sshDataServer
	static String mdsDataServer = null; // If enableNetworkSelection == false, mdsDataServer contains the mdsip address.
	static String metalClassName = "javax.swing.plaf.metal.MetalLookAndFeel";
	static String motifClassName = "com.sun.java.swing.plaf.motif.MotifLookAndFeel";
	static boolean not_sup_local = false;
	static int num_scope = 0;
	static long refreshPeriod = -1;
	static DataServerItem[] server_ip_list;
	static String sshDataServer = null; // If enableNetworkSelection == false, either mdsDataServer or sshDataServer
	static Object T_message;
	static int T_messageType;
	static Component T_parentComponent;
	static String T_title;
	public static final String VERSION;
	private static jScopeFacade win;
	static String windowsClassName = "com.sun.java.swing.plaf.windows.WindowsLookAndFeel";
	private static final JWindow splashScreen = new JWindow();
	static
	{ // handle case jScope vs.jscope
		String profile = System.getProperty("user.home") + File.separator + "jScope";
		final String upper = System.getProperty("user.home") + File.separator + "jScope";
		if (!new File(profile).exists() && new File(upper).exists())
			profile = upper;
		JSCOPE_PROFILE = profile;
		JSCOPE_CONFIGURATIONS = JSCOPE_PROFILE + File.separator + "configurations";
		JSCOPE_PALETTE = JSCOPE_PROFILE + File.separator + "colors.tbl";
		JSCOPE_PROPERTIES = JSCOPE_PROFILE + File.separator + "jScope.properties";
		JSCOPE_SERVERS = JSCOPE_PROFILE + File.separator + "jScope_servers.conf";
		final String version = jScopeFacade.class.getPackage().getImplementationVersion();
		VERSION = version == null ? "unknown" : version;
	}

	static public void displayPageFormatAttributes(int idx, PageFormat myPageFormat)
	{
		System.out.println("+----------------------------------------------------------+");
		System.out.println("Index = " + idx);
		System.out.println("Width = " + myPageFormat.getWidth());
		System.out.println("Height = " + myPageFormat.getHeight());
		System.out.println("ImageableX = " + myPageFormat.getImageableX());
		System.out.println("ImageableY = " + myPageFormat.getImageableY());
		System.out.println("ImageableWidth = " + myPageFormat.getImageableWidth());
		System.out.println("ImageableHeight = " + myPageFormat.getImageableHeight());
		final int o = myPageFormat.getOrientation();
		System.out.println("Orientation = " + (o == PageFormat.PORTRAIT ? "PORTRAIT"
				: o == PageFormat.LANDSCAPE ? "LANDSCAPE"
						: o == PageFormat.REVERSE_LANDSCAPE ? "REVERSE_LANDSCAPE" : "<invalid>"));
		System.out.println("+----------------------------------------------------------+");
	}

	public static String findFileInClassPath(String file)
	{
		final StringTokenizer path = new StringTokenizer(System.getProperty("java.class.path"), File.pathSeparator);
		String p, f_name;
		File f;
		while (path.hasMoreTokens())
		{
			p = path.nextToken();
			f = new File(p);
			if (!f.isDirectory())
				continue;
			f_name = p + File.separator + file;
			f = new File(f_name);
			if (f.exists())
				return f_name;
		}
		return null;
	}

	public static long getRefreshPeriod()
	{ return refreshPeriod; }

	/**
	 * A utility function that layers on top of the LookAndFeel's
	 * isSupportedLookAndFeel() method. Returns true if the LookAndFeel is
	 * supported. Returns false if the LookAndFeel is not supported and/or if there
	 * is any kind of error checking if the LookAndFeel is supported. The L&F menu
	 * will use this method to determine whether the various L&F options should be
	 * active or inactive.
	 *
	 */
	protected static boolean isAvailableLookAndFeel(String classname)
	{
		try
		{ // Try to create a L&F given a String
			final Class<?> lnfClass = Class.forName(classname);
			final LookAndFeel newLAF = (LookAndFeel) lnfClass.newInstance();
			if (newLAF == null)
				return false;
			return newLAF.isSupportedLookAndFeel();
		}
		catch (final Exception e)
		{ // If ANYTHING weird happens, return false
			return false;
		}
	}

	/**********************
	 * jScope Main
	 ***********************/
	static boolean IsNewJVMVersion()
	{
		// String ver = System.getProperty("java.version");
		final boolean isGreater = (Float.valueOf(System.getProperty("java.specification.version")) >= 1.2);
		return (isGreater);
		// return (! (ver.indexOf("1.0") != -1 || ver.indexOf("1.1") != -1));
	}

	protected static void jScopeSetUI(Component c)
	{
		if (c != null)
			SwingUtilities.updateComponentTreeUI(c);
	}

	public static void main(String args[])
	{
		startApplication(args);
	}

	static public void ShowMessage(Component parentComponent, Object message, String title, int messageType)
	{
		T_parentComponent = parentComponent;
		T_message = message;
		T_title = title;
		T_messageType = messageType;
		// do the following on the gui thread
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				JOptionPane.showMessageDialog(T_parentComponent, T_message, T_title, T_messageType);
			}
		});
	}

	public static void startApplication(String args[])
	{
		String file = null;
		String propertiesFile = null;
		final Properties props = System.getProperties();
		final String debug = props.getProperty("debug");
		if (debug != null && debug.equals("true"))
		{
			is_debug = true;
			Waveform.is_debug = true;
		}
		if (args.length != 0)
		{
			for (int i = 0; i < args.length; i++)
			{
				if (args[i].equals("-fp"))
				{
					if (i + 1 < args.length)
						propertiesFile = args[i + 1];
					i++;
				}
				else if (args[i].equals("-s") && i < args.length - 1)
				{
					enableNetworkSelection = false;
					mdsDataServer = args[i + 1];
					i++;
				}
				else if (args[i].equals("-ssh") && i < args.length - 1)
				{
					enableNetworkSelection = false;
					sshDataServer = args[i + 1];
					i++;
				}
				else if (args[i].equals("-l"))
				{
					enableNetworkSelection = false;
					mdsDataServer = null;
				}
				else
				{
					file = new String(args[i]);
				}
			}
		}
		if (IsNewJVMVersion())
			win = new jScopeFacade(100, 100, propertiesFile);
		else
		{
			System.out.println("jScope application required JDK version 1.2 or later");
			System.out.println("Your version is " + System.getProperty("java.specification.version"));
			System.exit(1);
		}
		win.pack();
		win.setSize(750, 550);
		jScopeFacade.num_scope++;
		win.startScope(file);
	}

	/** Menu item on menu autoscale_m */
	private JMenuItem all_i, allY_i;
	private JButton apply_b;
	// PageFormat pageFormat;
	PrintRequestAttributeSet attrs;
	private JCheckBoxMenuItem brief_error_i;
	public ColorDialog color_dialog;
	public ColorMapDialog colorMapDialog;
	private String config_file;
	protected String curr_directory;
	jScopeDefaultValues def_values = new jScopeDefaultValues();
	protected JMenuItem default_i, use_i, pub_variables_i, save_as_i, use_last_i, save_i, color_i, print_all_i, open_i,
			close_i, server_list_i, font_i, save_all_as_text_i;
	int default_server_idx;
	/** Menus on menu bar */
	protected JMenu edit_m, look_and_feel_m, pointer_mode_m, customize_m, autoscale_m, network_m, help_m;
	/** Menu items on menu edit_m */
	private JMenuItem exit_i, win_i;
	private JFileChooser file_diag;
	public FontSelection font_dialog;
	int height = 500, width = 700, xpos = 50, ypos = 50;
	private jScopeBrowseUrl help_dialog;
	BasicArrowButton incShot, decShot;
	int incShotValue = 0;
	private JTextField info_text, net_text;
	boolean is_playing = false;
	Properties js_prop = null;
	protected String last_directory;
	// L&F radio buttons
	JRadioButtonMenuItem macMenuItem;
	// contains the mdsio address.
	/** Main menu bar */
	protected JMenuBar mb;
	JRadioButtonMenuItem metalMenuItem;
	private boolean modified = false;
	JRadioButtonMenuItem motifMenuItem;
	private JPanel panel, panel1;
	private JLabel point_pos;
	private final ButtonGroup pointer_mode = new ButtonGroup();
	private JMenuItem print_i, properties_i;
	PrintService printerSelection;
	PrintService[] printersServices;
	DocPrintJob prnJob = null;
	JProgressBar progress_bar;
	private final String propertiesFile;
	private PubVarDialog pub_var_diag;
	ServerDialog server_diag;
	JMenu servers_m, updates_m;
	SetupDefaults setup_default;
	SetupDataDialog setup_dialog;
	private JLabel shot_l;
	private JTextField shot_t, signal_expr;
	private JCheckBoxMenuItem update_i, update_when_icon_i;
	jScopeWaveContainer wave_panel;
	private WindowDialog win_diag;
	JRadioButtonMenuItem windowsMenuItem;
	private JRadioButton zoom, point, copy, pan;
	/** Menu item on menu pointer_mode_m */
	private JMenuItem zoom_i, point_i, copy_i, pan_i;

	public jScopeFacade(int spos_x, int spos_y, String propFile)
	{
		this.propertiesFile = propFile == null ? JSCOPE_PROPERTIES : propFile;
		if (num_scope == 0 && splashScreen.getContentPane().getComponentCount() == 0)
		{
			try
			{
				splashScreen.addMouseListener(new MouseAdapter()
				{
					@Override
					public void mouseClicked(MouseEvent e)
					{
						splashScreen.dispose();
					}
				});
				splashScreen.getContentPane().add(new SplashScreen());
				splashScreen.pack();
				final Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
				splashScreen.setLocation(screenSize.width / 2 - splashScreen.getSize().width / 2,
						screenSize.height / 2 - splashScreen.getSize().height / 2);
				splashScreen.setVisible(true);
			}
			catch (final Exception exc)
			{
				exc.printStackTrace();
			}
		}
		jScopeCreate(spos_x, spos_y);
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		final Object ob = e.getSource();
		String action_cmd = null;
		if (ob != open_i)
			wave_panel.RemoveSelection();
		if (ob instanceof AbstractButton)
			action_cmd = ((AbstractButton) ob).getModel().getActionCommand();
		if (action_cmd != null)
		{
			final StringTokenizer act = new StringTokenizer(action_cmd);
			final String action = act.nextToken();
			if (action.equals("SET_SERVER"))
			{
				final String value = action_cmd.substring(action.length() + 1);
				if (!wave_panel.GetServerLabel().equals(value))
				{
					SetDataServer(getServerItem(value));
				}
			}
		}
		if (ob == signal_expr)
		{
			final String sig = signal_expr.getText().trim();
			if (sig != null && sig.length() != 0)
			{
				SetMainShot();
				wave_panel.AddSignal(sig, false);
				setChange(true);
			}
		}
		if (ob == apply_b || ob == shot_t)
		{
			incShotValue = 0;
			if (wave_panel.isBusy())
			{
				if (ob == apply_b)
					wave_panel.AbortUpdate();
			}
			else
			{
				if (ob == shot_t)
				{
					SetMainShot();
				}
				final String sig = signal_expr.getText().trim();
				if (sig != null && sig.length() != 0)
				{
					wave_panel.AddSignal(sig, true);
					setChange(true);
				}
				UpdateAllWaves();
			}
		}
		if (ob == color_i)
		{
			final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent ae)
				{
					jScopeFacade.this.color_dialog.ShowColorDialog(jScopeFacade.this.wave_panel);
				}
			});
			t.setRepeats(false);
			t.start();
		}
		if (ob == font_i)
		{
			font_dialog.setLocationRelativeTo(this);
			final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent ae)
				{
					font_dialog.setVisible(true);
				}
			});
			t.setRepeats(false);
			t.start();
		}
		if (ob == default_i)
		{
			final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent ae)
				{
					setup_default.Show(jScopeFacade.this, jScopeFacade.this.def_values);
				}
			});
			t.setRepeats(false);
			t.start();
		}
		if (ob == win_i)
		{
			if (win_diag == null)
				win_diag = new WindowDialog(this, "Window");
			final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent ae)
				{
					final boolean returnFlag = win_diag.ShowWindowDialog();
					if (returnFlag)
					{
						wave_panel.ResetDrawPanel(win_diag.out_row);
//		        wave_panel.update();
						UpdateColors();
						UpdateFont();
						setChange(true);
					}
				}
			});
			t.setRepeats(false);
			t.start();
		}
		if (ob == use_last_i)
		{
			if (last_directory != null && last_directory.trim().length() != 0)
			{
				curr_directory = last_directory;
				config_file = curr_directory;
				setChange(false);
				LoadConfiguration();
			}
		}
		if (ob == use_i)
		{
			LoadConfigurationFrom();
		}
		if (ob == save_i)
		{
			SaveConfiguration(config_file);
		}
		if (ob == save_as_i)
		{
			SaveAs();
		}
		if (ob == save_all_as_text_i)
		{
			wave_panel.SaveAsText(null, true);
		}
		if (ob == exit_i)
		{
			if (num_scope > 1)
			{
				final Object[] options =
				{ "Close this", "Close all", "Cancel" };
				final int opt = JOptionPane.showOptionDialog(null, "Close all open scopes?", "Warning",
						JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[0]);
				switch (opt)
				{
				case JOptionPane.YES_OPTION:
					closeScope();
					break;
				case JOptionPane.NO_OPTION:
					System.exit(0);
					break;
				case JOptionPane.CANCEL_OPTION:
					break;
				}
			}
			else
			{
				closeScope();
				if (num_scope == 0)
					System.exit(0);
				// System.gc();
			}
		}
		if (ob == close_i && num_scope != 1)
		{
			closeScope();
		}
		if (ob == open_i)
		{
			num_scope++;
			final Rectangle r = getBounds();
			// jScope new_scope = new jScope(r.x+5, r.y+40);
			final jScopeFacade new_scope = buildNewScope(r.x + 5, r.y + 40);
			new_scope.wave_panel.setCopySource(wave_panel.getCopySource());
			new_scope.startScope(null);
		}
		if (ob == all_i)
		{
			wave_panel.autoscaleAll();
		}
		if (ob == allY_i)
		{
			wave_panel.autoscaleAllY();
		}
		if (ob == copy_i)
		{
			wave_panel.SetMode(Waveform.MODE_COPY);
			copy.getModel().setSelected(true);
		}
		if (ob == zoom_i)
		{
			wave_panel.SetMode(Waveform.MODE_ZOOM);
			zoom.getModel().setSelected(true);
		}
		if (ob == point_i)
		{
			wave_panel.SetMode(Waveform.MODE_POINT);
			point.getModel().setSelected(true);
		}
		if (ob == pan_i)
		{
			wave_panel.SetMode(Waveform.MODE_PAN);
			pan.getModel().setSelected(true);
		}
		if (ob == server_list_i)
		{
			final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent ae)
				{
					server_diag.Show();
					server_ip_list = server_diag.getServerIpList();
				}
			});
			t.setRepeats(false);
			t.start();
		}
		if (ob == pub_variables_i)
		{
			final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent ae)
				{
					pub_var_diag.Show();
				}
			});
			t.setRepeats(false);
			t.start();
		}
	}

	public void ArrowsIncDecShot()
	{
		int idx;
		String sh = shot_t.getText();
		if ((idx = sh.lastIndexOf("+")) > 1 || (idx = sh.lastIndexOf("-")) > 1)
		{
			sh = sh.substring(0, idx);
			sh = sh.trim();
		}
		if (incShotValue != 0)
			shot_t.setText(sh + (incShotValue > 0 ? " + " : " - ") + Math.abs(incShotValue));
		else
			shot_t.setText(sh);
	}

	public boolean briefError()
	{
		return brief_error_i.getState();
	}

	protected jScopeFacade buildNewScope(int x, int y)
	{
		return new jScopeFacade(x, y, propertiesFile);
	}

	protected jScopeWaveContainer buildWaveContainer()
	{
		final int rows[] =
		{ 1, 0, 0, 0 };
		return (new jScopeWaveContainer(rows, def_values));
	}

	public void closeScope()
	{
		if (IsChange())
		{
			switch (saveWarning())
			{
			case JOptionPane.YES_OPTION:
				if (config_file == null)
					SaveAs();
				else
					SaveConfiguration(config_file);
				break;
			case JOptionPane.NO_OPTION:
//			exitScope();
				break;
			}
		}
//	else
//	{
//        exitScope();
//	}
		exitScope();
	}

	private void connectDefault()
	{
		if (mdsDataServer != null)
			SetDataServer(new DataServerItem("mdsip connection", mdsDataServer, "JAVA_USER", //
					MdsDataProvider.class.getName(), null, null, null));
		else if (sshDataServer != null)
			SetDataServer(new DataServerItem("mdsip connection", sshDataServer, "JAVA_USER", //
					MdsDataProviderSsh.class.getName(), null, null, null));
		else
			SetDataServer(new DataServerItem("local access", null, null, //
					MdsDataProviderLocal.class.getName(), null, null, null));
	}

	private void creaHistoryFile(File f)
	{
		int idx = 0, maxIdx = 0;
		int maxHistory = 2;
		final String config_file_history = js_prop.getProperty("jScope.config_file_history_length");
		try
		{
			maxHistory = Integer.parseInt(config_file_history);
		}
		catch (final Exception exc)
		{}
		final File pf = f.getParentFile();
		final String list[] = pf.list(new FileFilter(f.getName()));
		for (final String file : list)
		{
			final StringTokenizer st = new StringTokenizer(file, ";");
			try
			{
				String s = st.nextToken();
				s = st.nextToken();
				if (s != null)
				{
					idx = Integer.parseInt(s);
					if (idx > maxIdx)
						maxIdx = idx;
				}
			}
			catch (final Exception exc)
			{}
		}
		maxIdx++;
		if (maxIdx > maxHistory)
		{
			final File fd = new File(f.getAbsolutePath() + ";" + (maxIdx - maxHistory));
			fd.delete();
		}
		final File fr = new File(f.getAbsolutePath() + ";" + maxIdx);
		f.renameTo(fr);
	}

	public boolean equalsString(String s1, String s2)
	{
		boolean res = false;
		if (s1 != null)
		{
			if (s2 == null && s1.length() == 0)
				res = true;
			else if (s1.equals(s2))
				res = true;
		}
		else
		{
			if (s2 != null && s2.length() != 0)
				res = false;
			else
				res = true;
		}
		return res;
	}

	private boolean EventUpdateEnabled()
	{
		if (update_i.getState())
		{
			SetStatusLabel("Disable event update");
			return false;
		}
		if (getExtendedState() == Frame.ICONIFIED && update_when_icon_i.getState())
		{
			this.SetStatusLabel("Event update is disabled when iconified");
			return false;
		}
		return true;
	}

	private void exitScope()
	{
		try
		{
			wave_panel.RemoveAllEvents(this);
		}
		catch (final IOException e)
		{}
		dispose();
		num_scope--;
		// System.gc();
	}

	public void FromFile(Properties pr) throws IOException
	{
		String prop = "";
		try
		{
			if ((prop = pr.getProperty("Scope.update.disable")) != null)
			{
				final boolean b = new Boolean(prop).booleanValue();
				update_i.setState(b);
			}
			if ((prop = pr.getProperty("Scope.update.disable_when_icon")) != null)
			{
				final boolean b = new Boolean(prop).booleanValue();
				update_when_icon_i.setState(b);
			}
			if ((prop = pr.getProperty("Scope.geometry")) != null)
			{
				final StringTokenizer st = new StringTokenizer(prop);
				width = Integer.parseInt(st.nextToken("x"));
				height = Integer.parseInt(st.nextToken("x+"));
				xpos = Integer.parseInt(st.nextToken("+"));
				ypos = Integer.parseInt(st.nextToken("+"));
				final Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
				if (height > screenSize.height)
					height = screenSize.height;
				if (width > screenSize.width)
					width = screenSize.width;
				if (ypos + height > screenSize.height)
					ypos = screenSize.height - height;
				if (xpos + width > screenSize.width)
					xpos = screenSize.width - width;
			}
		}
		catch (final Exception exc)
		{
			throw (new IOException(prop));
		}
	}

	protected void GetPropertiesValue()
	{
		if (js_prop == null)
			return;
		// jscope configurations file directory can be defined
		// with decrease priority order:
		// 1) by system property jscope.config_directory;
		// in this case jscope must be started with
		// -DjScope.config_directory=<directory> option.
		// 2) in jScope.properties using jScope.directory property
		// If the previous properties are not defined jscope create
		// configuration folder in <home directory>/jscope/configurations, if
		// for same abnormal reason the directory creation failed
		// <home directory> is used as configuration directory
		curr_directory = System.getProperty("jScope.config_directory");
		if (curr_directory == null || curr_directory.trim().length() == 0)
		{
			curr_directory = js_prop.getProperty("jScope.directory");
			if (curr_directory == null || curr_directory.trim().length() == 0)
			{
				curr_directory = JSCOPE_CONFIGURATIONS;
				final File jScopeUserDir = new File(curr_directory);
				if (!jScopeUserDir.exists())
				{
					jScopeUserDir.mkdirs();
				}
			}
		}
		default_server_idx = -1;
		String prop = js_prop.getProperty("jScope.default_server");
		if (prop != null)
		{
			try
			{
				default_server_idx = Integer.parseInt(prop) - 1;
			}
			catch (final NumberFormatException e)
			{}
		}
		final String f_name = js_prop.getProperty("jScope.save_selected_points");
		final String proxy_host = js_prop.getProperty("jScope.http_proxy_host");
		final String proxy_port = js_prop.getProperty("jScope.http_proxy_port");
		prop = js_prop.getProperty("jScope.vertical_offset");
		if (prop != null)
		{
			int val = 0;
			try
			{
				val = Integer.parseInt(prop);
			}
			catch (final NumberFormatException e)
			{}
			Waveform.SetVerticalOffset(val);
		}
		prop = js_prop.getProperty("jScope.horizontal_offset");
		if (prop != null)
		{
			int val = 0;
			try
			{
				val = Integer.parseInt(prop);
			}
			catch (final NumberFormatException e)
			{}
			Waveform.SetHorizontalOffset(val);
		}
		final Properties p = System.getProperties();
		if (f_name != null)
			p.put("jScope.save_selected_points", f_name);
		if (curr_directory != null)
			p.put("jScope.curr_directory", curr_directory);
		if (proxy_port != null && proxy_host != null)
		{
			p.setProperty("http.proxyHost", proxy_host);
			p.setProperty("http.proxyPort", proxy_port);
		}
		final String timeConversion = js_prop.getProperty("jScope.time_format");
		if (timeConversion != null)
		{
			if (timeConversion.toUpperCase().equals("VMS"))
				Waveform.timeMode = Waveform.VMS_TIME;
			else if (timeConversion.toUpperCase().equals("EPICS"))
				Waveform.timeMode = Waveform.EPICS_TIME;
			// Add here new time formats
		}
		final String refreshPeriodStr = js_prop.getProperty("jScope.refresh_period");
		if (refreshPeriodStr != null)
		{
			try
			{
				refreshPeriod = Integer.parseInt(refreshPeriodStr);
				// Not shorted than 0.5 s
				if (refreshPeriod < 500)
					refreshPeriod = 500;
			}
			catch (final Exception exc)
			{
				refreshPeriod = -1;
			}
		}
	}

	private DataServerItem getServerItem(String server)
	{
		for (final DataServerItem element : server_ip_list)
			if (element.equals(server))
				return element;
		return null;
	}

	private void InitDataServer()
	{
		String ip_addr = null;
		String dp_class = null;
		DataServerItem srv_item = null;
		final Properties props = System.getProperties();
		ip_addr = props.getProperty("data.address");
		dp_class = props.getProperty("data.class");
		server_diag = new ServerDialog(this, "Server list");
		if (ip_addr != null && dp_class != null) // || is_local == null || (is_local != null && is_local.equals("no")))
		{
			srv_item = new DataServerItem(ip_addr, ip_addr, null, dp_class, null, null, null);
			// Add server to the server list and if presente browse class and
			// url browse signal set it into srv_item
			server_diag.addServerIp(srv_item);
		}
		if (srv_item == null || !SetDataServer(srv_item))
		{
			srv_item = wave_panel.DataServerFromClient(srv_item);
			if (srv_item == null || !SetDataServer(srv_item))
			{
				if (server_ip_list != null && default_server_idx >= 0 && default_server_idx < server_ip_list.length)
				{
					srv_item = server_ip_list[default_server_idx];
					SetDataServer(srv_item);
				}
				else
					setDataServerLabel();
			}
		}
	}

	public void InitProperties()
	{
		try
		{
			if (jScopeFacade.is_debug)
			{
				System.out.println("jScope.properties " + System.getProperty("jScope.properties"));
				System.out.println("jScope.config_directory " + System.getProperty("jScope.config_directory"));
			}
			String f_name;
			if (((new File(f_name = propertiesFile)).exists())//
					|| (f_name = System.getProperty("jScope.properties")) != null//
					|| ((new File(f_name = JSCOPE_PROPERTIES)).exists()))
			{
				js_prop = new Properties();
				js_prop.load(new FileInputStream(f_name));
			}
			else
			{
				final File jScopeUserDir = new File(JSCOPE_PROFILE);
				if (!jScopeUserDir.exists())
					jScopeUserDir.mkdirs();
				js_prop = new Properties();
				final String res = "mds/jscope/jScope.properties";
				try (final InputStream is = jScopeFacade.class.getClassLoader().getResourceAsStream(res))
				{
					js_prop.load(is);
				}
				try (final InputStream is = jScopeFacade.class.getClassLoader().getResourceAsStream(res))
				{
					try (final OutputStream os = new FileOutputStream(JSCOPE_PROPERTIES))
					{
						final byte b[] = new byte[1024];
						for (int len = is.read(b); len > 0; len = is.read(b))
							os.write(b, 0, len);
					}
				}
			}
			if (!(new File(JSCOPE_SERVERS)).exists())
			{
				final InputStream pis = getClass().getClassLoader().getResourceAsStream("jScope_servers.conf");
				if (pis != null)
				{
					final FileOutputStream fos = new FileOutputStream(f_name);
					final byte b[] = new byte[1024];
					for (int len = pis.read(b); len > 0; len = pis.read(b))
						fos.write(b, 0, len);
					fos.close();
					pis.close();
				}
			}
		}
		catch (final FileNotFoundException e)
		{
			System.out.println(e);
		}
		catch (final IOException e)
		{
			System.out.println(e);
		}
	}

	public void InvalidateDefaults()
	{
		wave_panel.InvalidateDefaults();
	}

	public boolean IsChange()
	{
		return modified;
	}

	public boolean IsShotDefinedXX()
	{
		String s = shot_t.getText();
		if (s != null && s.trim().length() > 0)
			return true;
		s = def_values.shot_str;
		if (s != null && s.trim().length() > 0)
		{
			shot_t.setText(s);
			return true;
		}
		return false;
	}

	@Override
	public void itemStateChanged(ItemEvent e)
	{
		final Object ob = e.getSource();
		if (ob == brief_error_i)
		{
			WaveInterface.brief_error = brief_error_i.getState();
//		wave_panel.SetBriefError(brief_error_i.getState());
		}
		if (e.getStateChange() != ItemEvent.SELECTED)
			return;
		if (ob == copy)
		{
			wave_panel.SetMode(Waveform.MODE_COPY);
		}
		if (ob == zoom)
		{
			wave_panel.SetMode(Waveform.MODE_ZOOM);
		}
		if (ob == point)
		{
			wave_panel.SetMode(Waveform.MODE_POINT);
		}
		if (ob == pan)
		{
			wave_panel.SetMode(Waveform.MODE_PAN);
		}
	}

	public void jScopeCreate(int spos_x, int spos_y)
	{
		printersServices = PrintServiceLookup.lookupPrintServices(null, null);
		printerSelection = PrintServiceLookup.lookupDefaultPrintService();
		attrs = new HashPrintRequestAttributeSet();
		final PrinterResolution res = new PrinterResolution(600, 600, ResolutionSyntax.DPI);
		attrs.add(MediaSizeName.ISO_A4);
		attrs.add(OrientationRequested.LANDSCAPE);
		attrs.add(new MediaPrintableArea(5, 5, MediaSize.ISO.A4.getX(Size2DSyntax.MM) - 5,
				MediaSize.ISO.A4.getY(Size2DSyntax.MM) - 5, MediaPrintableArea.MM));
		attrs.add(res);
		if (printerSelection != null)
			prnJob = printerSelection.createPrintJob();
		/*
		 * PrintServiceAttributeSet pras = printerSelection.getAttributes();
		 *
		 * MediaSize at = (MediaSize) printerSelection.getDefaultAttributeValue(
		 * MediaSize.ISO.A4.getCategory());
		 *
		 * System.out.println("Size name " + at);
		 *
		 * { PrintService[] pservice =
		 * PrintServiceLookup.lookupPrintServices(null,null); for (int i=0;
		 * i<pservice.length; i++) { System.out.println(pservice[i]);
		 * System.out.println(" --- Job Attributes ---"); Class[] cats =
		 * pservice[i].getSupportedAttributeCategories(); for (int j=0; j < cats.length;
		 * j++) { Attribute
		 * attr=(Attribute)pservice[i].getDefaultAttributeValue(cats[j]); if (attr !=
		 * null) { // Get attribute name and values String attrName = attr.getName();
		 * String attrValue = attr.toString();
		 * System.out.println(" "+attrName+": "+attrValue); Object o =
		 * pservice[i].getSupportedAttributeValues(attr.getCategory(),null, null); if
		 * (o.getClass().isArray()) { for (int k=0; k < Array.getLength(o); k++) {
		 * Object o2 = Array.get(o, k); System.out.println(" "+o2); } } else {
		 * System.out.println(" "+o); } } } } }
		 */
		help_dialog = new jScopeBrowseUrl(this);
		try
		{
			final String path = "jdocs/jScope.html";
			final URL url = getClass().getClassLoader().getResource(path);
			help_dialog.connectToBrowser(url);
		}
		catch (final Exception e)
		{}
		setBounds(spos_x, spos_y, 750, 550);
		InitProperties();
		GetPropertiesValue();
		font_dialog = new FontSelection(this, "Waveform Font Selection");
		setup_default = new SetupDefaults(this, "Default Setup", def_values);
		color_dialog = new ColorDialog(this, "Color Configuration Dialog");
		String palette = js_prop.getProperty("jScope.color_palette_file");
		if (palette == null)
		{
			palette = JSCOPE_PALETTE;
			try
			{
				ColorMapDialog.exportPalette(new File(JSCOPE_PALETTE));
			}
			catch (final IOException e1)
			{
				e1.printStackTrace();
			}
		}
		colorMapDialog = new ColorMapDialog(this, palette);
		pub_var_diag = new PubVarDialog(this);
		getContentPane().setLayout(new BorderLayout());
		setBackground(Color.lightGray);
		addWindowListener(this);
		file_diag = new JFileChooser();
		file_diag.addChoosableFileFilter(new javax.swing.filechooser.FileFilter()
		{
			@Override
			public boolean accept(File f)
			{
				return f.isDirectory() || f.getName().toLowerCase().endsWith(".dat");
			}

			@Override
			public String getDescription()
			{ return ".dat files"; }
		});
		javax.swing.filechooser.FileFilter defaultFileFilter;
		file_diag.addChoosableFileFilter(defaultFileFilter = new javax.swing.filechooser.FileFilter()
		{
			@Override
			public boolean accept(File f)
			{
				return f.isDirectory() || f.getName().toLowerCase().endsWith(".jscp");
			}

			@Override
			public String getDescription()
			{ return ".jscp files"; }
		});
		file_diag.setFileFilter(defaultFileFilter);
		mb = new JMenuBar();
		setJMenuBar(mb);
		edit_m = new JMenu("File");
		mb.add(edit_m);
		final JMenuItem browse_signals_i = new JMenuItem("Browse signals");
		edit_m.add(browse_signals_i);
		browse_signals_i.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				wave_panel.ShowBrowseSignals();
			}
		});
		open_i = new JMenuItem("New Window");
		open_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.CTRL_MASK));
		edit_m.add(open_i);
		open_i.addActionListener(this);
		look_and_feel_m = new JMenu("Look & Feel");
		edit_m.add(look_and_feel_m);
		final JMenuItem sign = new JMenuItem("History...");
		sign.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final SignalsBoxDialog sig_box_diag = new SignalsBoxDialog(jScopeFacade.this, "Visited Signals", false);
				sig_box_diag.setVisible(true);
			}
		});
		edit_m.add(sign);
		edit_m.addSeparator();
		// Copy image to clipborad can be done only with
		// java release 1.4
//    if(AboutWindow.javaVersion.indexOf("1.4") != -1)
		{
			final JMenuItem cb_copy = new JMenuItem("Copy to Clipboard");
			cb_copy.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					final Dimension dim = wave_panel.getSize();
					final BufferedImage ri = new BufferedImage(dim.width, dim.height, BufferedImage.TYPE_INT_RGB);
					final Graphics2D g2d = (Graphics2D) ri.getGraphics();
					g2d.setBackground(Color.white);
					wave_panel.PrintAll(g2d, dim.height, dim.width);
					try
					{
						final ImageTransferable imageTransferable = new ImageTransferable(ri);
						final Clipboard cli = Toolkit.getDefaultToolkit().getSystemClipboard();
						cli.setContents(imageTransferable, imageTransferable);
					}
					catch (final Exception exc)
					{
						System.out.println("Exception " + exc);
					}
				}
			});
			edit_m.add(cb_copy);
			edit_m.addSeparator();
		}
		print_i = new JMenuItem("Print Setup ...");
		print_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P, ActionEvent.CTRL_MASK));
		print_i.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final Thread print_cnf = new Thread()
				{
					@Override
					public void run()
					{
						setName("Print Dialog Thread");
						final PrintService svc = PrintServiceLookup.lookupDefaultPrintService();
						printerSelection = ServiceUI.printDialog(null, 100, 100, printersServices, svc, null, attrs);
						if (printerSelection != null)
						{
							System.out.println(
									printerSelection.getName() + " |||| " + printerSelection.getSupportedDocFlavors()
											+ " |||| " + printerSelection.hashCode());
							prnJob = printerSelection.createPrintJob();
							PrintAllWaves(attrs);
							/*
							 * try { DocFlavor flavor = DocFlavor.SERVICE_FORMATTED.PRINTABLE; Doc doc = new
							 * SimpleDoc(jScope.this, flavor, null); prnJob.print(doc, attrs); }
							 * catch(Exception exc) { System.out.println(exc); }
							 */
						}
					}
				};
				print_cnf.start();
			}
		});
		edit_m.add(print_i);
		/*****************************************************************************************
		 * page_i = new JMenuItem("Page Setup ...");
		 * page_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_J,
		 * ActionEvent.CTRL_MASK)); page_i.addActionListener(new ActionListener() {
		 * public void actionPerformed(ActionEvent e) { Thread page_cnf = new Thread() {
		 * public void run() { setName("Page Dialog Thread"); //pageFormat =
		 * prnJob.pageDialog(pageFormat); //prnJob.validatePage(pageFormat);
		 * //displayPageFormatAttributes(pageFormat); } }; page_cnf.start(); } });
		 * edit_m.add(page_i);
		 *********************************************************************************************/
		print_all_i = new JMenuItem("Print");
		print_all_i.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final Thread print_page = new Thread()
				{
					@Override
					public void run()
					{
						setName("Print All Thread");
						PrintAllWaves(attrs);
					}
				};
				print_page.start();
			}
		});
		edit_m.add(print_all_i);
		edit_m.addSeparator();
		properties_i = new JMenuItem("Properties...");
		properties_i.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final PropertiesEditor pe = new PropertiesEditor(jScopeFacade.this, jScopeFacade.this.propertiesFile);
				pe.setVisible(true);
			}
		});
		edit_m.add(properties_i);
		edit_m.addSeparator();
		close_i = new JMenuItem("Close");
		close_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, ActionEvent.CTRL_MASK));
		edit_m.add(close_i);
		close_i.addActionListener(this);
		exit_i = new JMenuItem("Exit");
		exit_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E, ActionEvent.CTRL_MASK));
		edit_m.add(exit_i);
		exit_i.addActionListener(this);
		// Look and Feel Radio control
		final ButtonGroup group = new ButtonGroup();
		final ToggleUIListener toggleUIListener = new ToggleUIListener();
		metalMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Java Look and Feel"));
		metalMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Metal"));
		metalMenuItem.setSelected(true);
		metalMenuItem.setEnabled(isAvailableLookAndFeel(metalClassName));
		group.add(metalMenuItem);
		metalMenuItem.addItemListener(toggleUIListener);
//	metalMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ActionEvent.ALT_MASK));
		motifMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Motif Look and Feel"));
		motifMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("CDE/Motif"));
		motifMenuItem.setEnabled(isAvailableLookAndFeel(motifClassName));
		group.add(motifMenuItem);
		motifMenuItem.addItemListener(toggleUIListener);
//	motifMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, ActionEvent.ALT_MASK));
		windowsMenuItem = (JRadioButtonMenuItem) look_and_feel_m
				.add(new JRadioButtonMenuItem("Windows Style Look and Feel"));
		windowsMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Windows"));
		windowsMenuItem.setEnabled(isAvailableLookAndFeel(windowsClassName));
		group.add(windowsMenuItem);
		windowsMenuItem.addItemListener(toggleUIListener);
//	windowsMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, ActionEvent.ALT_MASK));
		macMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Macintosh Look and Feel"));
		macMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Macintosh"));
		macMenuItem.setEnabled(isAvailableLookAndFeel(macClassName));
		group.add(macMenuItem);
		macMenuItem.addItemListener(toggleUIListener);
//	macMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, ActionEvent.ALT_MASK));
		pointer_mode_m = new JMenu("Pointer mode");
		mb.add(pointer_mode_m);
		point_i = new JMenuItem("Point");
		point_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ActionEvent.CTRL_MASK));
		point_i.addActionListener(this);
		pointer_mode_m.add(point_i);
		zoom_i = new JMenuItem("Zoom");
		zoom_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, ActionEvent.CTRL_MASK));
		pointer_mode_m.add(zoom_i);
		zoom_i.addActionListener(this);
		pan_i = new JMenuItem("Pan");
		pan_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, ActionEvent.CTRL_MASK));
		pointer_mode_m.add(pan_i);
		pan_i.addActionListener(this);
		copy_i = new JMenuItem("Copy");
		copy_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, ActionEvent.CTRL_MASK));
		pointer_mode_m.add(copy_i);
		copy_i.addActionListener(this);
		pointer_mode_m.add(copy_i);
		customize_m = new JMenu("Customize");
		mb.add(customize_m);
		default_i = new JMenuItem("Global Settings ...");
		default_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
		customize_m.add(default_i);
		default_i.addActionListener(this);
		win_i = new JMenuItem("Window ...");
		win_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, ActionEvent.CTRL_MASK));
		win_i.addActionListener(this);
		customize_m.add(win_i);
		font_i = new JMenuItem("Font selection ...");
		font_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F, ActionEvent.CTRL_MASK));
		font_i.addActionListener(this);
		customize_m.add(font_i);
		color_i = new JMenuItem("Colors List ...");
		color_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
		color_i.addActionListener(this);
		customize_m.add(color_i);
		pub_variables_i = new JMenuItem("Public variables ...");
		pub_variables_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_U, ActionEvent.CTRL_MASK));
		pub_variables_i.addActionListener(this);
		customize_m.add(pub_variables_i);
		brief_error_i = new JCheckBoxMenuItem("Brief Error", true);
		brief_error_i.addItemListener(this);
		customize_m.add(brief_error_i);
		customize_m.add(new JSeparator());
		use_last_i = new JMenuItem("Use last saved settings");
		use_last_i.addActionListener(this);
		use_last_i.setEnabled(false);
		customize_m.add(use_last_i);
		use_i = new JMenuItem("Use saved settings from ...");
		use_i.addActionListener(this);
		customize_m.add(use_i);
		customize_m.add(new JSeparator());
		save_i = new JMenuItem("Save current settings");
		save_i.setEnabled(false);
		save_i.addActionListener(this);
		customize_m.add(save_i);
		save_as_i = new JMenuItem("Save current settings as ...");
		customize_m.add(save_as_i);
		save_as_i.addActionListener(this);
		customize_m.add(new JSeparator());
		save_all_as_text_i = new JMenuItem("Save all as text ...");
		save_all_as_text_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, ActionEvent.CTRL_MASK));
		customize_m.add(save_all_as_text_i);
		save_all_as_text_i.addActionListener(this);
		updates_m = new JMenu("Updates");
		mb.add(updates_m);
		update_i = new JCheckBoxMenuItem("Disable", false);
		update_when_icon_i = new JCheckBoxMenuItem("Disable when icon", true);
		updates_m.add(update_i);
		updates_m.add(update_when_icon_i);
		autoscale_m = new JMenu("Autoscale");
		mb.add(autoscale_m);
		all_i = new JMenuItem("All");
		all_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B, ActionEvent.CTRL_MASK));
		all_i.addActionListener(this);
		autoscale_m.add(all_i);
		allY_i = new JMenuItem("All Y");
		allY_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
		autoscale_m.add(allY_i);
		allY_i.addActionListener(this);
		network_m = new JMenu("Network");
		mb.add(network_m);
		servers_m = new JMenu("Servers");
		network_m.add(servers_m);
		if (!enableNetworkSelection)
			network_m.setEnabled(false);
		// servers_m.addActionListener(this);
		servers_m.addMenuListener(new MenuListener()
		{
			@Override
			public void menuCanceled(MenuEvent e)
			{}

			@Override
			public void menuDeselected(MenuEvent e)
			{}

			@Override
			public void menuSelected(MenuEvent e)
			{
				server_diag.addServerIpList(server_ip_list);
			}
		});
		server_list_i = new JMenuItem("Edit server list ...");
		network_m.add(server_list_i);
		server_list_i.addActionListener(this);
		point_pos = new JLabel("[0.000000000, 0.000000000]");
		point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
		info_text = new JTextField(" Status : ", 85);
		info_text.setBorder(BorderFactory.createLoweredBevelBorder());
//    ImageIcon icon = new ImageIcon("printer1.gif");
		final JPanel progress_pan = new JPanel(new FlowLayout(2, 0, 0));
		progress_bar = new JProgressBar(0, 100);
		progress_bar.setBorder(BorderFactory.createLoweredBevelBorder());
		progress_bar.setStringPainted(true);
		progress_pan.add(progress_bar);
		/*
		 * print_icon = new JLabel(icon) { // overrides imageUpdate to control the
		 * animated gif's animation public boolean imageUpdate(Image img, int infoflags,
		 * int x, int y, int width, int height) { System.out.println("Update image "+
		 * infoflags); if (isShowing() && (infoflags & ALLBITS) != 0 && (infoflags &
		 * FRAMEBITS) == 0) repaint(); if (isShowing() && (infoflags & FRAMEBITS) != 0
		 * && false) repaint(); return isShowing(); } };
		 * print_icon.setBorder(BorderFactory.createLoweredBevelBorder()); //
		 * print_icon.setSize(20, 30); icon.setImageObserver(print_icon);
		 * progress_pan.add(print_icon);
		 */
		help_m = new JMenu("Help");
		mb.add(help_m);
		final JMenuItem about_i = new JMenuItem("About jScope");
		help_m.add(about_i);
		about_i.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				JOptionPane.showMessageDialog(jScopeFacade.this,
						"The jScope tutorial is available at https://www.mdsplus.org.\nhttps://www.mdsplus.org/index.php?title=Documentation:Tutorial:UsingScope",
						"jScope version: " + VERSION, JOptionPane.INFORMATION_MESSAGE);
			}
		});
		setup_dialog = new SetupDataDialog(this, "Setup");
		wave_panel = buildWaveContainer();
		wave_panel.addWaveContainerListener(this);
		wave_panel.SetParams(Waveform.MODE_ZOOM, setup_default.getGridMode(), setup_default.getLegendMode(),
				setup_default.getXLines(), setup_default.getYLines(), setup_default.getReversed());
		wave_panel.setPopupMenu(new jScopeWavePopup(setup_dialog, new ProfileDialog(null, null), colorMapDialog));
		getContentPane().add("Center", wave_panel);
		panel = new JPanel();
		panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
		shot_l = new JLabel("Shot");
		shot_t = new JTextField(10);
		shot_t.addActionListener(this);
		/*
		 * shot_t.addFocusListener( new FocusAdapter() { public void
		 * focusLost(FocusEvent e) { wave_panel.SetMainShot(shot_t.getText()); } } );
		 */
		apply_b = new JButton("Apply");
		apply_b.addActionListener(this);
		point = new JRadioButton("Point");
		point.addItemListener(this);
		zoom = new JRadioButton("Zoom", true);
		zoom.addItemListener(this);
		pan = new JRadioButton("Pan");
		pan.addItemListener(this);
		copy = new JRadioButton("Copy");
		copy.addItemListener(this);
		pointer_mode.add(point);
		pointer_mode.add(zoom);
		pointer_mode.add(pan);
		pointer_mode.add(copy);
		panel.add(point);
		panel.add(zoom);
		panel.add(pan);
		panel.add(copy);
		panel.add(shot_l);
		panel.add(decShot = new BasicArrowButton(SwingConstants.WEST));
		panel.add(shot_t);
		panel.add(incShot = new BasicArrowButton(SwingConstants.EAST));
		decShot.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (shot_t.getText() != null && shot_t.getText().trim().length() != 0)
				{
					if (!jScopeFacade.this.wave_panel.isBusy())
					{
						incShotValue--;
						ArrowsIncDecShot();
						UpdateAllWaves();
					}
				}
			}
		});
		incShot.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (shot_t.getText() != null && shot_t.getText().trim().length() != 0)
				{
					if (!jScopeFacade.this.wave_panel.isBusy())
					{
						incShotValue++;
						ArrowsIncDecShot();
						UpdateAllWaves();
					}
				}
			}
		});
		panel.add(apply_b);
		panel.add(new JLabel(" Signal: "));
		final JPanel panel3 = new JPanel();
		final GridBagLayout gridbag = new GridBagLayout();
		final GridBagConstraints c = new GridBagConstraints();
		final Insets insets = new Insets(2, 2, 2, 2);
		panel3.setLayout(gridbag);
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.insets = insets;
		c.weightx = 1.0;
		c.gridwidth = 1;
		signal_expr = new JTextField(25);
		signal_expr.addActionListener(this);
		gridbag.setConstraints(signal_expr, c);
		panel3.add(signal_expr);
		final JPanel panel4 = new JPanel(new BorderLayout());
		panel4.add("West", panel);
		panel4.add("Center", panel3);
		final JPanel panel2 = new JPanel();
		panel2.setLayout(new BorderLayout());
		panel2.add(BorderLayout.WEST, progress_pan);
		panel2.add(BorderLayout.CENTER, info_text);
		panel2.add(BorderLayout.EAST, net_text = new JTextField(" Data server :", 25));
		net_text.setBorder(BorderFactory.createLoweredBevelBorder());
		info_text.setEditable(false);
		net_text.setEditable(false);
		panel1 = new JPanel();
		panel1.setLayout(new BorderLayout());
		panel1.add("North", panel4);
		panel1.add("Center", point_pos);
		panel1.add("South", panel2);
		getContentPane().add("South", panel1);
		color_dialog.SetReversed(setup_default.getReversed());
		if (jScopeFacade.is_debug)
		{
			final Thread mon_mem = new MonMemory();
			mon_mem.start();
			final JButton exec_gc = new JButton("Exec gc");
			exec_gc.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					System.gc();
				}
			});
			panel1.add("West", exec_gc);
		}
//	if(enableNetworkSelection)
		InitDataServer();
		UpdateFont();
		UpdateColors();
	}

	/*
	 * public void handleQuit() { System.exit(0); }
	 */
	protected void jScopeUpdateUI()
	{
		jScopeFacade.jScopeSetUI(this);
		jScopeFacade.jScopeSetUI(font_dialog);
		jScopeFacade.jScopeSetUI(setup_default);
		jScopeFacade.jScopeSetUI(color_dialog);
		jScopeFacade.jScopeSetUI(pub_var_diag);
		jScopeFacade.jScopeSetUI(server_diag);
		jScopeFacade.jScopeSetUI(file_diag);
	}

	public void LoadConfiguration()
	{
		if (config_file == null)
			return;
		incShotValue = 0;
		try
		{
			final jScopeProperties pr = new jScopeProperties();
			pr.load(new FileInputStream(config_file));
			LoadConfiguration(pr);
		}
		catch (final IOException e)
		{
			Reset();
			JOptionPane.showMessageDialog(this, e.getMessage(), "alert LoadConfiguration", JOptionPane.ERROR_MESSAGE);
		}
		save_i.setEnabled(true);
	}

	public void LoadConfiguration(Properties pr)
	{
		wave_panel.EraseAllWave();
		try
		{
			LoadFromFile(pr);
			setBounds(xpos, ypos, width, height);
			UpdateColors();
			UpdateFont();
			wave_panel.update();
			validate();
			DataServerItem dsi = wave_panel.GetServerItem();
			dsi = server_diag.addServerIp(dsi);
			/*
			 * remove 28/06/2005 wave_panel.SetServerItem(dsi);
			 */
			if (enableNetworkSelection) // Take Data Server fedinition from configuration file ONLY if not forced in
										// command line
			{
				if (!SetDataServer(dsi))
					SetDataServer(new DataServerItem());
			}
			UpdateAllWaves();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
			Reset();
			JOptionPane.showMessageDialog(this, e.getMessage(), "alert LoadConfiguration", JOptionPane.ERROR_MESSAGE);
		}
		// SetWindowTitle("");
		// System.gc();
	}

	private void LoadConfigurationFrom()
	{
		if (IsChange())
		{
			switch (saveWarning())
			{
			case JOptionPane.YES_OPTION:
				if (config_file == null)
				{
					SaveAs();
					return;
				}
				else
					SaveConfiguration(config_file);
				break;
			case JOptionPane.CANCEL_OPTION:
				return;
			}
		}
		setChange(false);
		if (curr_directory != null && curr_directory.trim().length() != 0)
			file_diag.setCurrentDirectory(new File(curr_directory));
		final javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent ae)
			{
				final int returnVal = file_diag.showOpenDialog(jScopeFacade.this);
				if (returnVal == JFileChooser.APPROVE_OPTION)
				{
					final File file = file_diag.getSelectedFile();
					final String d = file.getAbsolutePath();
					final String f = file.getName();
					if (f != null && f.trim().length() != 0 && d != null && d.trim().length() != 0)
					{
						curr_directory = d;
						config_file = curr_directory;
						LoadConfiguration();
					}
				}
			}
		});
		t.setRepeats(false);
		t.start();
	}

	public void LoadFromFile(Properties pr) throws IOException
	{
		try
		{
			FromFile(pr);
			font_dialog.fromFile(pr, "Scope.font");
			color_dialog.FromFile(pr, "Scope.color_");
			pub_var_diag.fromFile(pr, "Scope.public_variable_");
			wave_panel.FromFile(pr, "Scope", color_dialog.getColorMapIndex(), colorMapDialog);
			wave_panel.AddAllEvents(this);
		}
		catch (final Exception e)
		{
			throw (new IOException("Configuration file syntax error : " + e.getMessage()));
		}
	}

	@Override
	public int print(Graphics g, PageFormat pf, int pageIndex) throws PrinterException
	{
		/*
		 * int st_x = 0, st_y = 0; double height = pf.getImageableHeight(); double width
		 * = pf.getImageableWidth();
		 */
		final Graphics2D g2 = (Graphics2D) g;
//        jScope.displayPageFormatAttributes(4,pf);
		if (pageIndex == 0)
		{
			g2.translate(pf.getImageableX(), pf.getImageableY());
			final RepaintManager currentManager = RepaintManager.currentManager(this);
			currentManager.setDoubleBufferingEnabled(false);
			g2.scale(72.0 / 600, 72.0 / 600);
//        Dimension d = this.getSize();
//        this.setSize((int)(d.width*600.0/72.0), (int)(d.height*600.0/72.0));
//        this.printAll(g2);
			g2.translate(pf.getImageableWidth() / 2, pf.getImageableHeight() / 2);
			g2.translate(pf.getWidth() / 2, pf.getHeight() / 2);
			g.drawOval(0, 0, 100, 100);
			g.drawOval(0, 0, 200, 200);
			g.drawOval(0, 0, 300, 300);
			g.drawOval(0, 0, 400, 400);
			g.drawOval(0, 0, 500, 500);
			g.drawOval(0, 0, 600, 600);
			currentManager.setDoubleBufferingEnabled(true);
			return Printable.PAGE_EXISTS;
		}
		else
			return Printable.NO_SUCH_PAGE;
	}

	protected void PrintAllWaves(PrintRequestAttributeSet attrs)
	{
		try
		{
			this.SetStatusLabel("Executing print");
			wave_panel.PrintAllWaves(prnJob, attrs);
			SetStatusLabel("End print operation");
		}
		catch (final PrinterException er)
		{
			System.out.println(er);
			JOptionPane.showMessageDialog(null, "Error on print operation", "alert PrintAllWaves",
					JOptionPane.ERROR_MESSAGE);
		}
		catch (final PrintException er)
		{
			System.out.println(er);
			JOptionPane.showMessageDialog(null, "Error on print operation", "alert PrintAllWaves",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	@Override
	public void processConnectionEvent(ConnectionEvent e)
	{
		progress_bar.setString("");
		if (e.getID() == ConnectionEvent.LOST_CONNECTION)
		{
			if (!enableNetworkSelection) // Creation of DataProvider based on command line
			{
				if (JOptionPane.showConfirmDialog(this, e.info + "  Reconnect To Data Server?",
						"alert processConnectionEvent", JOptionPane.YES_NO_OPTION) == JOptionPane.NO_OPTION)
					System.exit(0);
				jScopeFacade.this.connectDefault();
			}
			else
			{
				JOptionPane.showMessageDialog(this, e.info, "alert processConnectionEvent", JOptionPane.ERROR_MESSAGE);
				SetDataServer(new DataServerItem());
			}
			return;
		}
		if (e.current_size == 0 && e.total_size == 0)
		{
			if (e.info != null)
				progress_bar.setString(e.info);
			else
				progress_bar.setString("");
			progress_bar.setValue(0);
		}
		else
		{
			final int v = (int) ((float) e.current_size / e.total_size * 100.);
			progress_bar.setValue(v);
			progress_bar.setString((e.info != null ? e.info : "") + v + "%");
		}
	}

	@Override
	public void processUpdateEvent(UpdateEvent e)
	{
		if (EventUpdateEnabled())
		{
			final String print_event = wave_panel.GetPrintEvent();
			final String event = wave_panel.GetEvent();
			if (e.getName().equals(event))
			{
				SwingUtilities.invokeLater(new Runnable()
				{
					@Override
					public void run()
					{
						UpdateAllWaves();
					}
				});
			}
			// wave_panel.StartUpdate();
			if (e.getName().equals(print_event))
				wave_panel.StartPrint(prnJob, attrs);
		}
	}

	@Override
	public void processWaveContainerEvent(WaveContainerEvent e)
	{
		String s = null;
		final int event_id = e.getID();
		switch (event_id)
		{
		case WaveContainerEvent.END_UPDATE:
		case WaveContainerEvent.KILL_UPDATE:
			apply_b.setText("Apply");
			if (event_id == WaveContainerEvent.KILL_UPDATE)
			{
				/*
				 * JOptionPane.showMessageDialog(this, e.info,
				 * "alert processWaveContainerEvent", JOptionPane.ERROR_MESSAGE);
				 */
				System.out.println(" processWaveContainerEvent " + e.info);
				SetStatusLabel(" Aborted ");
			}
			else
				SetStatusLabel(e.info);
			SetWindowTitle("");
			break;
		case WaveContainerEvent.START_UPDATE:
			SetStatusLabel(e.info);
			break;
		case WaveContainerEvent.WAVEFORM_EVENT:
			final WaveformEvent we = (WaveformEvent) e.we;
			final jScopeMultiWave w = (jScopeMultiWave) we.getSource();
			final MdsWaveInterface wi = (MdsWaveInterface) w.wi;
			final int we_id = we.getID();
			switch (we_id)
			{
			case WaveformEvent.EVENT_UPDATE:
				if (EventUpdateEnabled())
				{
					setPublicVariables(pub_var_diag.getPublicVar());
					SetMainShot();
					wave_panel.Refresh(w, we.status_info);
					w.RefreshOnEvent();
					// ??????*******
				}
				break;
			case WaveformEvent.MEASURE_UPDATE:
			case WaveformEvent.POINT_UPDATE:
			case WaveformEvent.POINT_IMAGE_UPDATE:
				s = we.toString();
				if (wi.shots != null)
				{
					point_pos.setText(
							s + " Expr : " + w.getSignalName(we.signal_idx) + " Shot = " + wi.shots[we.signal_idx]);
				}
				else
				{
					point_pos.setText(s + " Expr : " + w.getSignalName(we.signal_idx));
				}
				break;
			case WaveformEvent.STATUS_INFO:
				SetStatusLabel(we.status_info);
				break;
			case WaveformEvent.CACHE_DATA:
				if (we.status_info != null)
					progress_bar.setString(we.status_info);
				else
					progress_bar.setString("");
				progress_bar.setValue(0);
				break;
			}
			break;
		}
	}

	public void RepaintAllWaves()
	{
		final int wave_mode = wave_panel.GetMode();
		jScopeMultiWave w;
		wave_panel.SetMode(Waveform.MODE_WAIT);
		for (int i = 0, k = 0; i < 4; i++)
		{
			for (int j = 0; j < wave_panel.getComponentsInColumn(i); j++, k++)
			{
				w = (jScopeMultiWave) wave_panel.GetWavePanel(k);
				if (w.wi != null)
				{
					SetStatusLabel("Repaint signal column " + (i + 1) + " row " + (j + 1));
					setColor(w.wi);
					w.Update(w.wi);
				}
			}
		}
		wave_panel.RepaintAllWaves();
		wave_panel.SetMode(wave_mode);
	}

	private void Reset()
	{
		config_file = null;
		incShotValue = 0;
		SetWindowTitle("");
		wave_panel.Reset();
	}

	private void SaveAs()
	{
		if (curr_directory != null && curr_directory.trim().length() != 0)
			file_diag.setCurrentDirectory(new File(curr_directory));
		int returnVal = JFileChooser.CANCEL_OPTION;
		boolean done = false;
		while (!done)
		{
			returnVal = file_diag.showSaveDialog(jScopeFacade.this);
			if (returnVal == JFileChooser.APPROVE_OPTION)
			{
				final File file = file_diag.getSelectedFile();
				final String txtsig_file = file.getAbsolutePath();
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
			final File file = file_diag.getSelectedFile();
			final String d = file.getAbsolutePath();
			final String f = file.getName();
			if (f != null && f.trim().length() != 0 && d != null && d.trim().length() != 0)
			{
				curr_directory = d;
				config_file = curr_directory;
			}
			else
				config_file = null;
			if (config_file != null)
			{
				SaveConfiguration(config_file);
			}
		}
		// }
		// });
		// tim.setRepeats(false);
		// tim.start();
	}

	public void SaveConfiguration(String conf_file)
	{
		PrintWriter out;
		File ftmp, fok;
		if (conf_file == null || conf_file.length() == 0)
			return;
		final int pPos = conf_file.lastIndexOf('.');
		final int sPos = conf_file.lastIndexOf(File.separatorChar);
		if (pPos == -1 || pPos < sPos)
			conf_file = conf_file + ".jscp";
		last_directory = new String(conf_file);
		save_i.setEnabled(true);
		use_last_i.setEnabled(true);
		fok = new File(conf_file);
		ftmp = new File(conf_file + "_tmp");
		try
		{
			out = new PrintWriter(new FileWriter(ftmp));
			ToFile(out);
			out.close();
			if (fok.exists())
				creaHistoryFile(fok);
			ftmp.renameTo(fok);
		}
		catch (final Exception e)
		{
			JOptionPane.showMessageDialog(this, e, "alert", JOptionPane.ERROR_MESSAGE);
		}
		ftmp.delete();
	}

	private int saveWarning()
	{
		final Object[] options =
		{ "Save", "Don't Save", "Cancel" };
		final int val = JOptionPane.showOptionDialog(null, "Save change to the configuration file before closing ?",
				"Warning", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[0]);
		return val;
	}

	public void SetApplicationFonts(Font font)
	{
		// int fontSize=9;
		// Font userEntryFont = new Font("Dialog", Font.PLAIN, fontSize);
		// Font defaultFont = new Font("Dialog", Font.PLAIN, fontSize);
		// Font boldFont = new Font("Dialog", Font.BOLD, fontSize);
		final Font userEntryFont = font;
		final Font defaultFont = font;
		final Font boldFont = font;
		// writeToFile("c:\\temp\\outFile.txt",String.valueOf(UIManager.getDefaults()));
		// // write out defaults
		// User entry widgets
		UIManager.put("Text.font", new FontUIResource(userEntryFont));
		UIManager.put("TextField.font", new FontUIResource(userEntryFont));
		UIManager.put("TextArea.font", new FontUIResource(userEntryFont));
		UIManager.put("TextPane.font", new FontUIResource(userEntryFont));
		UIManager.put("List.font", new FontUIResource(userEntryFont));
		UIManager.put("Table.font", new FontUIResource(userEntryFont));
		UIManager.put("ComboBox.font", new FontUIResource(userEntryFont));
		// Non-user entry widgets
		UIManager.put("Button.font", new FontUIResource(defaultFont));
		UIManager.put("Label.font", new FontUIResource(defaultFont));
		UIManager.put("Menu.font", new FontUIResource(defaultFont));
		UIManager.put("MenuItem.font", new FontUIResource(defaultFont));
		UIManager.put("ToolTip.font", new FontUIResource(defaultFont));
		UIManager.put("ToggleButton.font", new FontUIResource(defaultFont));
		UIManager.put("TitledBorder.font", new FontUIResource(boldFont));
		UIManager.put("PopupMenu.font", new FontUIResource(defaultFont));
		UIManager.put("TableHeader.font", new FontUIResource(defaultFont));
		UIManager.put("PasswordField.font", new FontUIResource(defaultFont));
		UIManager.put("CheckBoxMenuItem.font", new FontUIResource(defaultFont));
		UIManager.put("CheckBox.font", new FontUIResource(defaultFont));
		UIManager.put("RadioButtonMenuItem.font", new FontUIResource(defaultFont));
		UIManager.put("RadioButton.font", new FontUIResource(defaultFont));
		// Containters
		UIManager.put("ToolBar.font", new FontUIResource(defaultFont));
		UIManager.put("MenuBar.font", new FontUIResource(defaultFont));
		UIManager.put("Panel.font", new FontUIResource(defaultFont));
		UIManager.put("ProgressBar.font", new FontUIResource(defaultFont));
		UIManager.put("TextPane.font", new FontUIResource(defaultFont));
		UIManager.put("OptionPane.font", new FontUIResource(defaultFont));
		UIManager.put("ScrollPane.font", new FontUIResource(defaultFont));
		UIManager.put("EditorPane.font", new FontUIResource(defaultFont));
		UIManager.put("ColorChooser.font", new FontUIResource(defaultFont));
		jScopeUpdateUI();
	}

	public void SetApplicationFonts(String font, int style, int size)
	{
		SetApplicationFonts(new Font(font, style, size));
	}

	public void setChange(boolean change)
	{
		if (modified == change)
			return;
		modified = change;
		this.SetWindowTitle("");
	}

	private void setColor(WaveInterface wi)
	{
//	if(wi == null || wi.colors_idx == null) return;
//	for(int i = 0; i < wi.colors_idx.length; i++)
//	    wi.colors[i] = color_dialog.GetColorAt(wi.colors_idx[i]);
	}

	public boolean SetDataServer(DataServerItem new_srv_item)
	{
		try
		{
			wave_panel.SetDataServer(new_srv_item, this);
			setDataServerLabel();
			return true;
		}
		catch (final Exception e)
		{
			// e.printStackTrace();
			JOptionPane.showMessageDialog(null, e.getMessage(), "Cannot establish connection with data server",
					JOptionPane.ERROR_MESSAGE);
			net_text.setText("Not Connected");
			if (!enableNetworkSelection)
				System.exit(0);
		}
		return false;
	}

	public void setDataServerLabel()
	{
		if (enableNetworkSelection)
		{
			if (wave_panel != null)
				net_text.setText("Data Server:" + wave_panel.GetServerLabel());
		}
		else // taken from command line
		{
			if (mdsDataServer != null)
				net_text.setText("Data Server: " + mdsDataServer);
			else if (sshDataServer != null)
				net_text.setText("SSH Data Server: " + sshDataServer);
			else
				net_text.setText("Data Server: Local");
		}
	}

	public void SetMainShot()
	{
		wave_panel.SetMainShot(shot_t.getText());
	}

	public void setPublicVariables(String public_variables)
	{
		def_values.setPublicVariables(public_variables);
		if (!def_values.getIsEvaluated())
			// def_values.setIsEvaluated(false);
			// Force update in all waveform
			wave_panel.SetModifiedState(true);
	}

	public void SetStatusLabel(String msg)
	{
		info_text.setText(" Status: " + msg);
	}

	public void SetWindowTitle(String info)
	{
		String f_name = config_file;
		if (f_name == null)
			f_name = "Untitled";
		if (wave_panel.GetTitle() != null)
			setTitle(" - " + wave_panel.GetEvaluatedTitle() + " - " + f_name + (IsChange() ? " (changed)" : "") + " "
					+ info);
		else
			setTitle("- Scope - " + f_name + (IsChange() ? " (changed)" : "") + " " + info);
	}

	public void startScope(String file)
	{
		if (file != null)
		{
			config_file = new String(file);
			LoadConfiguration();
		}
		if (!enableNetworkSelection) // Creation of DataProvider based on command line
		{
			connectDefault();
		}
		SetWindowTitle("");
		setVisible(true);
	}

	private void ToFile(PrintWriter out) throws IOException
	{
		final Rectangle r = getBounds();
		setChange(false);
		SetWindowTitle("");
		out.println("Scope.geometry: " + r.width + "x" + r.height + "+" + r.x + "+" + r.y);
		out.println("Scope.update.disable: " + update_i.getState());
		out.println("Scope.update.disable_when_icon: " + update_when_icon_i.getState());
		font_dialog.toFile(out, "Scope.font");
		pub_var_diag.toFile(out, "Scope.public_variable_");
		color_dialog.toFile(out, "Scope.color_");
		wave_panel.ToFile(out, "Scope.");
	}

	public void UpdateAllWaves()
	{
		final String s = shot_t.getText();
		final String s1 = def_values.shot_str;
		// Set main shot text field with
		// global setting shot if defined.
		if ((s == null || s.trim().length() == 0) && (s1 != null && s1.trim().length() != 0))
			shot_t.setText(s1);
		apply_b.setText("Abort");
		setPublicVariables(pub_var_diag.getPublicVar());
		SetMainShot();
		wave_panel.StartUpdate();
	}

	public void UpdateColors()
	{
		wave_panel.SetColors(color_dialog.GetColors(), color_dialog.GetColorsName());
		this.setup_dialog.SetColorList();
	}

	public void UpdateDefaultValues()
	{
		boolean is_changed = false;
		try
		{
			if ((is_changed = setup_default.IsChanged(def_values)))
			{
				this.setChange(true);
				wave_panel.RemoveAllEvents(this);
				setup_default.SaveDefaultConfiguration(def_values);
				InvalidateDefaults();
				wave_panel.AddAllEvents(this);
				UpdateAllWaves();
			}
			else
				setup_default.SaveDefaultConfiguration(def_values);
			wave_panel.SetParams(wave_panel.GetMode(), setup_default.getGridMode(), setup_default.getLegendMode(),
					setup_default.getXLines(), setup_default.getYLines(), setup_default.getReversed());
			color_dialog.SetReversed(setup_default.getReversed());
			UpdateColors();
			if (!is_changed)
				RepaintAllWaves();
		}
		catch (final IOException e)
		{
			JOptionPane.showMessageDialog(null, e.getMessage(), "alert UpdateDefaultValues", JOptionPane.ERROR_MESSAGE);
		}
	}

	public void UpdateFont()
	{
		wave_panel.SetFont(font_dialog.GetFont());
	}

	@Override
	public void windowActivated(WindowEvent e)
	{}

	@Override
	public void windowClosed(WindowEvent e)
	{}

	@Override
	public void windowClosing(WindowEvent e)
	{
		if (num_scope == 1)
			System.exit(0);
		else
			exitScope();
	}

	@Override
	public void windowDeactivated(WindowEvent e)
	{}

	@Override
	public void windowDeiconified(WindowEvent e)
	{}

	@Override
	public void windowIconified(WindowEvent e)
	{}

	@Override
	public void windowOpened(WindowEvent e)
	{
		splashScreen.dispose();
	}
}

class ServerDialog extends JDialog implements ActionListener
{
	private static final long serialVersionUID = 1L;
	static private JList<DataServerItem> server_list;
	private final JButton add_b, remove_b, exit_b, connect_b, modify_b;
	JCheckBox automatic;
	JComboBox<String> data_provider_list;
	private final Set<String> data_server_class = new HashSet<String>();
	jScopeFacade dw;
	private final DefaultListModel<DataServerItem> list_model = new DefaultListModel<>();
	JTextField server_l, server_a, server_u;
	JLabel server_label, user_label;
	JTextField tunnel_port;
	JCheckBox tunneling;

	ServerDialog(JFrame dw, String title)
	{
		super(dw, title, true);
		this.dw = (jScopeFacade) dw;
//	    setResizable(false);
		final GridBagLayout gridbag = new GridBagLayout();
		final GridBagConstraints c = new GridBagConstraints();
		final Insets insets = new Insets(4, 4, 4, 4);
		getContentPane().setLayout(gridbag);
		c.insets = insets;
		c.anchor = GridBagConstraints.WEST;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		c.gridheight = 10;
		server_list = new JList<>(list_model);
		final JScrollPane scrollServerList = new JScrollPane(server_list);
		server_list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		server_list.addListSelectionListener(new ListSelectionListener()
		{
			@Override
			public void valueChanged(ListSelectionEvent e)
			{
				final DataServerItem dsi = server_list.getSelectedValue();
				if (dsi != null)
				{
					remove_b.setEnabled(true);
					modify_b.setEnabled(true);
					server_l.setText(dsi.getName());
					server_a.setText(dsi.getArgument());
					server_u.setText(dsi.getUser());
					data_provider_list.setSelectedItem(dsi.getClassName());
					if (dsi.getTunnelPort() != null)
					{
						tunneling.setSelected(true);
						tunnel_port.setText(dsi.getTunnelPort());
						tunnel_port.setEditable(true);
					}
					else
					{
						tunnel_port.setText("");
						tunneling.setSelected(false);
						tunnel_port.setEditable(false);
					}
				}
				else
				{
					remove_b.setEnabled(false);
					modify_b.setEnabled(false);
				}
			}
		});
		// server_list.addKeyListener(this);
		gridbag.setConstraints(scrollServerList, c);
		getContentPane().add(scrollServerList);
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.NONE;
		c.gridheight = 1;
		c.gridwidth = 1;
		server_label = new JLabel("Server label ");
		gridbag.setConstraints(server_label, c);
		getContentPane().add(server_label);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		server_l = new JTextField(20);
		gridbag.setConstraints(server_l, c);
		getContentPane().add(server_l);
		c.gridwidth = 1;
		server_label = new JLabel("Server argument ");
		gridbag.setConstraints(server_label, c);
		getContentPane().add(server_label);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		server_a = new JTextField(20);
		gridbag.setConstraints(server_a, c);
		getContentPane().add(server_a);
		c.gridwidth = 1;
		tunneling = new JCheckBox("Tunneling local Port:");
		tunneling.addItemListener(new ItemListener()
		{
			@Override
			public void itemStateChanged(ItemEvent e)
			{
				if (tunneling.isSelected())
				{
					tunnel_port.setEditable(true);
				}
				else
				{
					tunnel_port.setEditable(false);
				}
			}
		});
		gridbag.setConstraints(tunneling, c);
		getContentPane().add(tunneling);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		tunnel_port = new JTextField(6);
		tunnel_port.setEditable(false);
		gridbag.setConstraints(tunnel_port, c);
		getContentPane().add(tunnel_port);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		automatic = new JCheckBox("Get user name from host");
		automatic.addItemListener(new ItemListener()
		{
			@Override
			public void itemStateChanged(ItemEvent e)
			{
				if (automatic.isSelected())
				{
					server_u.setText(System.getProperty("user.name"));
					server_u.setEditable(false);
				}
				else
				{
					server_u.setText("");
					server_u.setEditable(true);
				}
			}
		});
		gridbag.setConstraints(automatic, c);
		getContentPane().add(automatic);
		c.gridwidth = 1;
		server_label = new JLabel("User name ");
		gridbag.setConstraints(server_label, c);
		getContentPane().add(server_label);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		server_u = new JTextField(20);
		gridbag.setConstraints(server_u, c);
		getContentPane().add(server_u);
		c.anchor = GridBagConstraints.WEST;
		c.gridwidth = 1;
		final JLabel lab = new JLabel("Server Class : ");
		gridbag.setConstraints(lab, c);
		getContentPane().add(lab);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.BOTH;
		data_provider_list = new JComboBox<>();
		gridbag.setConstraints(data_provider_list, c);
		getContentPane().add(data_provider_list);
		data_provider_list.addItemListener(new ItemListener()
		{
			@Override
			public void itemStateChanged(ItemEvent e)
			{
				try
				{
					if (e.getStateChange() == ItemEvent.SELECTED)
					{
						final String srv = (String) data_provider_list.getSelectedItem();
						if (srv != null)
						{
							final Class<?> cl = Class.forName("mds.provider." + srv);
							final DataProvider dp = ((DataProvider) cl.newInstance());
							final boolean state = dp.supportsTunneling();
							tunneling.setEnabled(state);
							tunnel_port.setEnabled(state);
						}
					}
				}
				catch (final Exception exc)
				{}
			}
		});
		final JPanel p = new JPanel(new FlowLayout(FlowLayout.CENTER));
		add_b = new JButton("Add");
		add_b.addActionListener(this);
		p.add(add_b);
		modify_b = new JButton("Modify");
		modify_b.addActionListener(this);
		modify_b.setEnabled(false);
		p.add(modify_b);
		remove_b = new JButton("Remove");
		remove_b.addActionListener(this);
		remove_b.setEnabled(false);
		p.add(remove_b);
		connect_b = new JButton("Connect");
		connect_b.addActionListener(this);
		p.add(connect_b);
		exit_b = new JButton("Close");
		exit_b.addActionListener(this);
		p.add(exit_b);
		c.gridwidth = GridBagConstraints.REMAINDER;
		gridbag.setConstraints(p, c);
		getContentPane().add(p);
		// add Provider Names
		final ArrayList<String> names = new ArrayList<>(DataServerItem.knownProviders.size());
		DataServerItem.knownProviders.forEach((e) ->
		{ names.add(e.getSimpleName()); });
		names.sort(Comparator.comparing(String::toString));
		names.forEach(this::addProviderName);
		//
		if (jScopeFacade.server_ip_list == null)
			GetPropertiesValue();
		else
			addServerIpList(jScopeFacade.server_ip_list);
	}

	@Override
	public void actionPerformed(ActionEvent event)
	{
		final Object ob = event.getSource();
		if (ob == exit_b)
			setVisible(false);
		if (ob == add_b)
		{
			final String srv = server_l.getText().trim();
			if (srv != null && srv.length() != 0)
			{
				addServerIp(new DataServerItem(srv, server_a.getText().trim(), server_u.getText().trim(),
						(String) data_provider_list.getSelectedItem(), null, null, tunnel_port.getText()));
			}
		}
		if (ob == remove_b)
		{
			final int idx = server_list.getSelectedIndex();
			if (idx >= 0)
			{
				list_model.removeElementAt(idx);
				dw.servers_m.remove(idx);
			}
		}
		if (ob == connect_b)
		{
			final int idx = server_list.getSelectedIndex();
			if (idx >= 0)
				dw.SetDataServer(jScopeFacade.server_ip_list[idx]);
		}
		if (ob == modify_b)
		{
			final int idx = server_list.getSelectedIndex();
			if (idx >= 0)
			{
				final String srv = server_l.getText().trim();
				if (srv != null && srv.length() != 0)
				{
					if (!jScopeFacade.server_ip_list[idx].getName().equals(srv))
					{
						final int itemsCount = dw.servers_m.getItemCount();
						JMenuItem mi;
						for (int i = 0; i < itemsCount; i++)
						{
							mi = dw.servers_m.getItem(i);
							if (mi.getText().trim().equals(jScopeFacade.server_ip_list[idx].getName()))
							{
								mi.setText(srv);
								mi.setActionCommand("SET_SERVER " + srv);
							}
						}
					}
					jScopeFacade.server_ip_list[idx].update(srv, server_a.getText(), server_u.getText(),
							(String) data_provider_list.getSelectedItem(), tunnel_port.getText());
					server_list.repaint();
					// It is need to update the current data server if it is
					// the modified server
					// if (dw.wave_panel.GetServerItem().equals(dw.server_ip_list[idx]))
					// {
					// dw.SetDataServer(dw.server_ip_list[idx]);
					// }
				}
			}
		}
	}

	private final void addProviderName(final String name)
	{
		if (name == null || data_server_class.contains(name))
			return;
		data_server_class.add(name);
		data_provider_list.addItem(name);
	}

	public DataServerItem addServerIp(DataServerItem dsi)
	{
		JMenuItem new_ip;
		DataServerItem found_dsi = null;
		/*
		 * 23-05-2005 found = ( (found_dsi = findServer(dsi)) != null);
		 */
		found_dsi = findServer(dsi);
		if (found_dsi != null)
			return found_dsi;
		addProviderName(dsi.getClassName());
		// if (!found && dsi.class_name == null)
		if (dsi.getClassName() == null)
		{
			JOptionPane.showMessageDialog(null, "Undefined data server class for " + dsi.getName(), "alert addServerIp",
					JOptionPane.ERROR_MESSAGE);
		}
//        if (!found)
		{
			list_model.addElement(dsi);
			new_ip = new JMenuItem(dsi.getName());
			dw.servers_m.add(new_ip);
			new_ip.setActionCommand("SET_SERVER " + dsi.getName());
			new_ip.addActionListener(dw);
			jScopeFacade.server_ip_list = getServerIpList();
		}
		/*
		 * 23-05-2005 Ovverride configuration file server definitions with property
		 * server definition with the same name
		 *
		 * else {
		 *
		 * if (found_dsi != null) { dsi.user = found_dsi.user; dsi.class_name =
		 * found_dsi.class_name; dsi.browse_class = found_dsi.browse_class;
		 * dsi.browse_url = found_dsi.browse_url; dsi.tunnel_port =
		 * found_dsi.tunnel_port; } }
		 */
		return dsi;
	}

	public void addServerIpList(DataServerItem[] dsi_list)
	{
		if (dsi_list == null)
			return;
		for (final DataServerItem element : dsi_list)
			addServerIp(element);
	}

	private DataServerItem findServer(DataServerItem dsi)
	{
		DataServerItem found_dsi = null;
		final Enumeration<DataServerItem> e = list_model.elements();
		while (e.hasMoreElements())
		{
			found_dsi = e.nextElement();
			if (found_dsi.equals(dsi))
			{
				return found_dsi;
			}
		}
		return null;
	}

	private void GetPropertiesValue()
	{
		final Properties js_prop = dw.js_prop;
		if (js_prop == null)
			return;
		for (int i = 1;; i++)
		{
			final String name = js_prop.getProperty("jScope.data_server_" + i + ".name");
			if (name == null)
				break;
			final String argument = js_prop.getProperty("jScope.data_server_" + i + ".argument");
			final String user = js_prop.getProperty("jScope.data_server_" + i + ".user");
			final String class_name = js_prop.getProperty("jScope.data_server_" + i + ".class");
			final String browse_class = js_prop.getProperty("jScope.data_server_" + i + ".browse_class");
			final String browse_url = js_prop.getProperty("jScope.data_server_" + i + ".browse_url");
			final String tunnel_port = js_prop.getProperty("jScope.data_server_" + i + ".tunnel_port");
			final DataServerItem dsi = new DataServerItem(name, argument, user, class_name, browse_class, browse_url,
					tunnel_port);
			addServerIp(dsi);
		}
	}

	public DataServerItem[] getServerIpList()
	{
		final Enumeration<DataServerItem> e = list_model.elements();
		final DataServerItem out[] = new DataServerItem[list_model.size()];
		for (int i = 0; e.hasMoreElements(); i++)
			out[i] = e.nextElement();
		return out;
	}

	private void resetAll()
	{
		server_list.clearSelection();
		server_l.setText("");
		server_a.setText("");
		server_u.setText("");
		tunnel_port.setText("");
		data_provider_list.setSelectedIndex(data_provider_list.getItemCount() > 0 ? 0 : -1);
	}

	public void Show()
	{
		pack();
		resetAll();
		final DataServerItem found_dsi = findServer(dw.wave_panel.GetServerItem());
		if (found_dsi != null)
			server_list.setSelectedValue(found_dsi, true);
		setLocationRelativeTo(dw);
		setVisible(true);
	}
}

class WindowDialog extends JDialog implements ActionListener
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	boolean changed = false;
	int in_row[];
	JLabel label;
	JButton ok, apply, cancel;
	int out_row[] = new int[4];
	jScopeFacade parent;
	JSlider row_1, row_2, row_3, row_4;
	JTextField titleText, eventText, printEventText;

	WindowDialog(JFrame dw, String title)
	{
		super(dw, title, true);
		parent = (jScopeFacade) dw;
		final GridBagConstraints c = new GridBagConstraints();
		final GridBagLayout gridbag = new GridBagLayout();
		getContentPane().setLayout(gridbag);
		c.insets = new Insets(4, 4, 4, 4);
		c.fill = GridBagConstraints.BOTH;
		c.gridwidth = GridBagConstraints.REMAINDER;
		label = new JLabel("Rows in Column");
		gridbag.setConstraints(label, c);
		getContentPane().add(label);
		final JPanel p = new JPanel();
		p.setLayout(new FlowLayout(FlowLayout.LEFT));
		c.gridwidth = GridBagConstraints.BOTH;
		row_1 = new JSlider(SwingConstants.VERTICAL, 1, 16, 1);
		// row_1.setMajorTickSpacing(4);
		row_1.setMinorTickSpacing(1);
		row_1.setPaintTicks(true);
		row_1.setPaintLabels(true);
		final Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
		labelTable.put(Integer.valueOf(1), new JLabel("1"));
		labelTable.put(Integer.valueOf(4), new JLabel("4"));
		labelTable.put(Integer.valueOf(8), new JLabel("8"));
		labelTable.put(Integer.valueOf(12), new JLabel("12"));
		labelTable.put(Integer.valueOf(16), new JLabel("16"));
		row_1.setLabelTable(labelTable);
		row_1.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
		row_2 = new JSlider(SwingConstants.VERTICAL, 0, 16, 0);
		row_2.setMajorTickSpacing(4);
		row_2.setMinorTickSpacing(1);
		row_2.setPaintTicks(true);
		row_2.setPaintLabels(true);
		row_2.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
		row_3 = new JSlider(SwingConstants.VERTICAL, 0, 16, 0);
		row_3.setMajorTickSpacing(4);
		row_3.setMinorTickSpacing(1);
		row_3.setPaintTicks(true);
		row_3.setPaintLabels(true);
		row_3.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
		row_4 = new JSlider(SwingConstants.VERTICAL, 0, 16, 0);
		row_4.setMajorTickSpacing(4);
		row_4.setMinorTickSpacing(1);
		row_4.setPaintTicks(true);
		row_4.setPaintLabels(true);
		row_4.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
		c.gridwidth = GridBagConstraints.REMAINDER;
		gridbag.setConstraints(p, c);
		getContentPane().add(p);
		p.add(row_1);
		p.add(row_2);
		p.add(row_3);
		p.add(row_4);
		c.gridwidth = GridBagConstraints.BOTH;
		label = new JLabel("Title");
		gridbag.setConstraints(label, c);
		getContentPane().add(label);
		c.gridwidth = GridBagConstraints.REMAINDER;
		titleText = new JTextField(40);
		gridbag.setConstraints(titleText, c);
		getContentPane().add(titleText);
		c.gridwidth = GridBagConstraints.BOTH;
		label = new JLabel("Update event");
		gridbag.setConstraints(label, c);
		getContentPane().add(label);
		c.gridwidth = GridBagConstraints.REMAINDER;
		eventText = new JTextField(40);
		gridbag.setConstraints(eventText, c);
		getContentPane().add(eventText);
		c.gridwidth = GridBagConstraints.BOTH;
		label = new JLabel("Print event");
		gridbag.setConstraints(label, c);
		getContentPane().add(label);
		c.gridwidth = GridBagConstraints.REMAINDER;
		printEventText = new JTextField(40);
		gridbag.setConstraints(printEventText, c);
		getContentPane().add(printEventText);
		final JPanel p1 = new JPanel();
		p1.setLayout(new FlowLayout(FlowLayout.CENTER));
		ok = new JButton("Ok");
		ok.addActionListener(this);
		p1.add(ok);
		cancel = new JButton("Cancel");
		cancel.addActionListener(this);
		p1.add(cancel);
		c.gridwidth = GridBagConstraints.REMAINDER;
		gridbag.setConstraints(p1, c);
		getContentPane().add(p1);
		pack();
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		final Object ob = e.getSource();
		try
		{
			if (ob == ok || ob == apply)
			{
				parent.wave_panel.SetTitle(new String(titleText.getText()));
				String event = new String(eventText.getText().trim());
				parent.wave_panel.SetEvent(parent, event);
				event = new String(printEventText.getText().trim());
				parent.wave_panel.SetPrintEvent(parent, event);
				parent.SetWindowTitle("");
				out_row[0] = row_1.getValue();
				out_row[1] = row_2.getValue();
				out_row[2] = row_3.getValue();
				out_row[3] = row_4.getValue();
				for (int i = 0; i < 4; i++)
					if (out_row[i] != in_row[i])
					{
						changed = true;
						break;
					}
				in_row = null;
				if (ob == ok)
					setVisible(false);
			}
			if (ob == cancel)
			{
				setVisible(false);
			}
		}
		catch (final IOException ev)
		{
			JOptionPane.showMessageDialog(null, ev.getMessage(), "alert actionPerformed", JOptionPane.ERROR_MESSAGE);
		}
	}

	public boolean ShowWindowDialog()
	{
		changed = false;
		if (parent.wave_panel.GetTitle() != null)
			titleText.setText(parent.wave_panel.GetTitle());
		if (parent.wave_panel.GetEvent() != null)
			eventText.setText(parent.wave_panel.GetEvent());
		if (parent.wave_panel.GetPrintEvent() != null)
			printEventText.setText(parent.wave_panel.GetPrintEvent());
		final int in_row[] = parent.wave_panel.getComponentsInColumns();
		row_1.setValue(in_row[0]);
		row_2.setValue(in_row[1]);
		row_3.setValue(in_row[2]);
		row_4.setValue(in_row[3]);
		this.in_row = new int[in_row.length];
		for (int i = 0; i < in_row.length; i++)
			this.in_row[i] = in_row[i];
		setLocationRelativeTo(parent);
		setVisible(true);
		return changed;
	}
}
