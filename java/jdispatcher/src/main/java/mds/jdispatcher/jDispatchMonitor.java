package mds.jdispatcher;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import javax.swing.*;

import mds.connection.*;

public class jDispatchMonitor extends JFrame implements MdsServerListener, ConnectionListener
{
	class BuildCellRenderer extends JLabel implements ListCellRenderer<Object>
	{
		/**
		 *
		 */
		private static final long serialVersionUID = 1L;

		public BuildCellRenderer()
		{
			setOpaque(true);
		}

		@Override
		public Component getListCellRendererComponent(final JList<?> list, final Object value, final int index,
				final boolean isSelected, final boolean cellHasFocus)
		{
			// Setting Font
			// for a JList item in a renderer does not work.
			setFont(done_font);
			final MdsMonitorEvent me = (MdsMonitorEvent) value;
			setText(me.getMonitorString());
			if (me.on == 0)
				setForeground(Color.lightGray);
			else
				setForeground(Color.black);
			if (isSelected)
			{
				setBackground(list.getSelectionBackground());
			}
			else
			{
				setBackground(list.getBackground());
			}
			return this;
		}
	}

	// End BuildCellRenderer class
	public class ErrorMgr extends MdsIp
	{
		// The constructor
		public ErrorMgr(final int port)
		{
			super(port);
		}

		// The message handler: the expression and arguments are passed as a vector of
		// MdsMessage
		@Override
		public MdsMessage handleMessage(final MdsMessage[] messages)
		{
			int nids[] = null;
			String message = null;
			// method MdsMessage.ToString returns the argument as a string
			// in this as messages[0] contains the expression to be evaluated
			// the other elements of messages contain additional arguments
			// see the definition of MdsMessage (or ask me)
			// if you need to handle other types of arguments
			try
			{
				nids = messages[1].ToIntArray();
				message = messages[2].ToString();
			}
			catch (final Exception exc)
			{
				System.out.println("Error receiving error message : " + exc);
				return new MdsMessage((byte) 1);
			}
			if (nids == null || message == null)
				return new MdsMessage((byte) 1);
			Hashtable<Integer, MdsMonitorEvent> actions = null;
			if (phase_failed.containsKey(new Integer(curr_phase)))
				actions = phase_failed.get(new Integer(curr_phase));
			else
			{
				actions = new Hashtable<>();
				phase_failed.put(new Integer(curr_phase), actions);
			}
			if (actions == null)
				return new MdsMessage((byte) 1);
			if (!actions.containsKey(new Integer(nids[0])))
			{
				final MdsMonitorEvent me = new MdsMonitorEvent(this, curr_phase, nids[0], message);
				actions.put(new Integer(nids[0]), me);
			}
			// In this example the mdsip returns a string value
			return new MdsMessage((byte) 1);
			// if a integer number has to be returned instead,
			// you can type return new MdsMessage((Byte)<number>)
		}
	}// End ErrorMgr class

	class ExecutingCellRenderer extends JLabel implements ListCellRenderer<Object>
	{
		/**
		 *
		 */
		private static final long serialVersionUID = 1L;

		public ExecutingCellRenderer()
		{
			setOpaque(true);
		}

		@Override
		public Component getListCellRendererComponent(final JList<?> list, final Object value, final int index,
				final boolean isSelected, final boolean cellHasFocus)
		{
			// Setting Font
			// for a JList item in a renderer does not work.
			if (value == null)
				return this;
			setFont(done_font);
			final MdsMonitorEvent me = (MdsMonitorEvent) value;
			setText(me.getMonitorString());
			setBackground(isSelected ? list.getSelectionBackground() : list.getBackground());
			switch (me.mode)
			{
			case MdsMonitorEvent.MonitorDispatched:
				setForeground(Color.lightGray);
				break;
			case MdsMonitorEvent.MonitorDoing:
				setForeground(Color.blue);
				break;
			case MdsMonitorEvent.MonitorDone:
				if ((me.ret_status & 1) == 1)
				{
					setForeground(Color.green);
				}
				else
				{
					setForeground(Color.red);
				}
				break;
			}
			return this;
		}
	}// End ExecutingCellRenderer class

	class FailedCellRenderer extends JLabel implements ListCellRenderer<Object>
	{
		/**
		 *
		 */
		private static final long serialVersionUID = 1L;

		public FailedCellRenderer()
		{
			setOpaque(true);
		}

		@Override
		public Component getListCellRendererComponent(final JList<?> list, final Object value, final int index,
				final boolean isSelected, final boolean cellHasFocus)
		{
			// Setting Font
			// for a JList item in a renderer does not work.
			setFont(done_font);
			final MdsMonitorEvent me = (MdsMonitorEvent) value;
			setText(me.getMonitorString());
			if (isSelected)
			{
				setBackground(list.getSelectionBackground());
			}
			else
			{
				setBackground(list.getBackground());
			}
			return this;
		}
	}

	// End FailedCellRenderer class
	public class ShotsPerformance
	{
		PrintStream log;
		Vector<String> labelText = new Vector<>();
		Hashtable<Long, Vector<String>> shotsInfo = new Hashtable<>();
		Vector<String> phaseDuration;

		public ShotsPerformance()
		{
			labelText.add("SHOT");
		}

		public void add(final long shot, final String phase, final String duration)
		{
			if (phase == null)
				return;
			final Long key = new Long(shot);
			if (!labelText.contains(phase))
				labelText.add(phase);
			if (!shotsInfo.containsKey(key))
			{
				phaseDuration = new Vector<>();
				shotsInfo.put(key, phaseDuration);
			}
			else
			{
				phaseDuration = shotsInfo.get(key);
			}
			phaseDuration.add(duration);
		}

		public void print()
		{
			for (int i = 0; i < labelText.size(); i++)
				System.out.print("\t" + labelText.elementAt(i));
			System.out.println();
			final Enumeration<Long> shots = shotsInfo.keys();
			final Enumeration<Vector<String>> e = shotsInfo.elements();
			while (e.hasMoreElements())
			{
				final String shotStr = "\t" + shots.nextElement();
				final Vector<?> phDur = e.nextElement();
				for (int i = 0; i < phDur.size(); i++)
				{
					if ((i % (labelText.size() - 1)) == 0)
					{
						System.out.println(shotStr);
					}
					System.out.print("\t" + phDur.elementAt(i));
				}
			}
		}

		public void printHeader(final PrintStream out)
		{
			for (int i = 0; i < labelText.size(); i++)
				out.print("\t" + labelText.elementAt(i));
			out.println();
		}

		public void printValues(final PrintStream out)
		{
			final Vector<?> phDur = shotsInfo.get(new Long(shot));
			for (int i = 0; i < phDur.size(); i++)
			{
				if ((i % (labelText.size() - 1)) == 0)
				{
					out.println("\t" + shot);
				}
				out.print("\t" + phDur.elementAt(i));
			}
		}

		public void saveTimePhaseExecution()
		{
			final File file = new File("PhaseExecutionTime.log");
			try
			{
				if (log == null)
				{
					if (file.exists())
					{
						log = new PrintStream(new FileOutputStream(file, true));
						log.println();
						return;
					}
					else
					{
						if (labelText.size() > 1)
						{
							log = new PrintStream(new FileOutputStream(file, true));
							printHeader(log);
						}
						else
							return;
					}
				}
				printValues(log);
			}
			catch (final Exception exc)
			{
				logMonitor("PERFORMANCE", "ERROR", exc);
				exc.printStackTrace();
			}
		}
	}

	class ShowMessage implements Runnable
	{
		String msg;

		public ShowMessage(final String msg)
		{
			this.msg = msg;
		}

		@Override
		public void run()
		{
			JOptionPane.showMessageDialog(jDispatchMonitor.this, msg, "alert", JOptionPane.ERROR_MESSAGE);
		}
	}// End ShowMessage class

	class ToolTipJList extends JList<MdsMonitorEvent>
	{
		private static final long serialVersionUID = 1L;

		public ToolTipJList(final ListModel<MdsMonitorEvent> lm)
		{
			super(lm);
			addMouseListener(new MouseAdapter()
			{
				@Override
				public void mouseClicked(final MouseEvent e)
				{
					if (e.getClickCount() == 2)
					{
						final int index = locationToIndex(e.getPoint());
						final ListModel<?> dlm = getModel();
						final MdsMonitorEvent item = (MdsMonitorEvent) dlm.getElementAt(index);
						ensureIndexIsVisible(index);
						if ((item.ret_status & 1) == 0)
						{
							JOptionPane.showMessageDialog(jDispatchMonitor.this, item.error_message, "alert",
									JOptionPane.ERROR_MESSAGE);
						}
						else
						{
							if (item.error_message != null && item.error_message.length() != 0)
								JOptionPane.showMessageDialog(jDispatchMonitor.this, item.error_message, "alert",
										JOptionPane.WARNING_MESSAGE);
						}
					}
				}
			});
		}

		@Override
		public String getToolTipText(final MouseEvent event)
		{
			String out = null;
			final int st = this.getFirstVisibleIndex();
			int end = this.getLastVisibleIndex();
			if (end == -1)
				end = ((DefaultListModel<?>) this.getModel()).size() - 1;
			if (st == -1 || end == -1)
				return null;
			Rectangle r;
			for (int i = st; i <= end; i++)
			{
				r = this.getCellBounds(i, i);
				if (r.contains(event.getPoint()))
				{
					final MdsMonitorEvent me = getModel().getElementAt(i);
					out = me.error_message;
					break;
				}
			}
			return out;
		}
	} // End ToolTipJList class

	class UpdateHandler implements Runnable
	{
		JList<MdsMonitorEvent> list;
		int idx;

		UpdateHandler(final JList<MdsMonitorEvent> list, final int idx)
		{
			this.list = list;
			this.idx = idx;
		}

		@Override
		public void run()
		{
			curr_list = list;
			item_idx = idx;
			if (auto_scroll && show_phase == curr_phase)
			{
				if (item_idx == -1)
					item_idx = curr_list.getModel().getSize() - 1;
				curr_list.ensureIndexIsVisible(item_idx);
			}
			list.repaint();
		}
	}

	class UpdateList implements Runnable
	{
		MdsMonitorEvent me;

		UpdateList(final MdsMonitorEvent me)
		{
			this.me = me;
		}

		@Override
		public void run()
		{
			int status = 1;
			switch (me.mode)
			{
			case MdsMonitorEvent.MonitorStartPhase:
				startPhase(me);
				break;
			case MdsMonitorEvent.MonitorEndPhase:
				endPhase(me);
				break;
			case MdsMonitorEvent.MonitorServerConnected:
				serversInfoPanel.updateServerState(me.server_address, true);
				break;
			case MdsMonitorEvent.MonitorServerDisconnected:
				serversInfoPanel.updateServerState(me.server_address, false);
				final ServerInfo si = serversInfo.get(me.server_address);
				String msg;
				if (si != null)
				{
					if (si.killed)
						msg = null;
					else
						msg = "Disconnect from server class " + si.getClassName() + " address :" + me.server_address;
				}
				else
					msg = "Disconnect from server class address :" + me.server_address;
				if (msg != null)
					JOptionPane.showMessageDialog(jDispatchMonitor.this, msg, "Alert", JOptionPane.ERROR_MESSAGE);
				break;
			case MdsMonitorEvent.MonitorBuildBegin:
				resetAll(me);
			case MdsMonitorEvent.MonitorBuild:
			case MdsMonitorEvent.MonitorBuildEnd:
				build_list.addElement(me);
				break;
			case MdsMonitorEvent.MonitorCheckin:
				break;
			case MdsMonitorEvent.MonitorDone:
				status = me.ret_status;
				Hashtable<Integer, MdsMonitorEvent> actions = null;
				if (phase_failed.containsKey(new Integer(me.phase)))
				{
					actions = phase_failed.get(new Integer(me.phase));
					if (actions.containsKey(new Integer(me.nid)))
					{
						final MdsMonitorEvent me_failed = actions.get(new Integer(me.nid));
						if (me.error_message == null || me.error_message.indexOf("SS-W-NOMSG") != -1)
							me.error_message = me_failed.error_message;
						else
							me.error_message += " " + me_failed.error_message;
						me_failed.error_message = new String(me.error_message);
					}
				}
			case MdsMonitorEvent.MonitorDispatched:
			case MdsMonitorEvent.MonitorDoing:
				synchronized (executing_list)
				{
					SwingUtilities.invokeLater(new Runnable()
					{
						@Override
						public void run()
						{
							serversInfoPanel.updateServersInfoAction(me);
						}
					});
					final int idx = getIndex(me, executing_list.size());
					if (idx == ACTION_NOT_FOUND)
					{
						JOptionPane.showMessageDialog(jDispatchMonitor.this, "Invalid message received", "Alert",
								JOptionPane.ERROR_MESSAGE);
						return;
					}
					// NOTE: executing_list may be empty in case an error message
					// has been received before a dispatched message
					if (idx == -1 || executing_list.size() == 0)
						executing_list.addElement(me);
					else
						executing_list.set(idx, me);
					if ((status & 1) == 0)
					{
						failed_list.addElement(me);
						showUpdateAction(failedList, -1);
					}
					showUpdateAction(executingList, idx);
				}
				break;
			}
			counter(me);
		}
	}// End UpdateList class

	private static final long serialVersionUID = 1L;
	public static final int ACTION_NOT_FOUND = -2;
	// private final static Font disp_font = new Font("Platino Linotype",
	// Font.ITALIC, 12);
	// private final static Font doing_font = new Font("Platino Linotype",
	// Font.ITALIC | Font.BOLD, 12);
	private final static Font done_font = new Font("Platino Linotype", Font.PLAIN, 12);

	private static final void logMonitor(String event, String phase, Object... more)
	{
		synchronized (System.out)
		{
			System.out.print(String.format("Monitor: PHASE %-7s %-12s ", event, phase));
			for (final Object e : more)
				System.out.print(e);
			System.out.println("");
			System.out.flush();
		}
	}

	public static void main(final String[] args)
	{
		final jDispatchMonitor dm;
		String experiment = "test";
		String monitor_server = "localhost:8800";
		if (args != null && args.length > 3)
		{
			System.out.println("jDispatchMonitor [monitor_server] [-e experiment ] \n");
			System.exit(0);
		}
		if (args.length > 0)
		{
			int i;
			for (i = 0; i < args.length && !args[i].equals("-e"); i++);
			if (i < args.length)
			{
				experiment = args[i + 1];
				if (args.length >= 3)
					monitor_server = ((i == 0) ? args[i + 2] : args[0]);
			}
			else
			{
				monitor_server = args[0];
				if (args.length > 1)
					experiment = args[1];
			}
		}
		System.out.println("jDispatchMonitor " + monitor_server + " " + experiment);
		dm = new jDispatchMonitor(monitor_server, experiment);
		dm.pack();
		dm.setSize(600, 700);
		dm.setDefaultCloseOperation(EXIT_ON_CLOSE);
		dm.setVisible(true);
	}

	ServersInfoPanel serversInfoPanel = null;
	Hashtable<String, ServerInfo> serversInfo = new Hashtable<>();
	ShotsPerformance shotsPerformance = new ShotsPerformance();
	private String experiment;
	// private final static Font done_failed_font = new Font("Platino Linotype",
	// Font.ITALIC, 12);
	private MdsServer mds_server = null;
	private MdsConnection dispatcher = null;
	private JDesktopPane desktop = null;
	private final JLabel total_actions_l, dispatched_l, doing_l, done_l, failed_l, exp_l, shot_l, phase_l;
	private int disp_count = 0, doing_count = 0, done_count = 0, failed_count = 0, total_count = 0;
	private final ButtonGroup phase_group = new ButtonGroup();
	private final JMenu phase_m = new JMenu("Phase");
	private final JPopupMenu do_command_menu = new JPopupMenu();
	private JMenuItem do_redispatch;
	private JMenuItem do_abort;
	private JList<MdsMonitorEvent> curr_list;
	private int item_idx;
	private boolean auto_scroll = true;
	private int curr_phase = -1;
	private int show_phase = -1;
	private String tree;
	private long shot;
	private String monitor_server;
	private JInternalFrame build, executing, failed;
	private JList<MdsMonitorEvent> buildList, executingList, failedList;
	private final DefaultListModel<MdsMonitorEvent> build_list = new DefaultListModel<>();
	private final DefaultListModel<MdsMonitorEvent> executing_list = new DefaultListModel<>();
	private final DefaultListModel<MdsMonitorEvent> failed_list = new DefaultListModel<>();
	private int num_window = 0;
	private final Hashtable<Integer, Hashtable<Integer, MdsMonitorEvent>> phase_hash = new Hashtable<>();
	private final Hashtable<Integer, Hashtable<Integer, MdsMonitorEvent>> phase_name = new Hashtable<>();
	private final Hashtable<Integer, Hashtable<Integer, MdsMonitorEvent>> phase_failed = new Hashtable<>();
	private ErrorMgr error_mgr;
	private int info_port;
	private Process dispatcher_proc;
	long startPhaseTime = 0;

	public jDispatchMonitor()
	{
		this(null, null);
	}

	public jDispatchMonitor(final String monitor_server, final String experiment)
	{
		this.experiment = experiment;
		addWindowListener(new java.awt.event.WindowAdapter()
		{
			@Override
			public void windowClosing(final java.awt.event.WindowEvent e)
			{
				shotsPerformance.saveTimePhaseExecution();
			}
		});
		this.addKeyListener(new java.awt.event.KeyAdapter()
		{
			@Override
			public void keyTyped(final java.awt.event.KeyEvent e)
			{
				if (e.isAltDown() && e.isShiftDown() && e.getKeyChar() == 'P')
					shotsPerformance.print();
			}
		});
		final JMenuBar mb = new JMenuBar();
		this.setJMenuBar(mb);
		setWindowTitle();
		this.monitor_server = monitor_server;
		final Properties properties = MdsHelper.initialization(experiment);
		if (properties == null)
		{
			JOptionPane.showMessageDialog(this, "Error reading properties file. Exiting.", "Alert",
					JOptionPane.ERROR_MESSAGE);
			System.exit(1);
		}
		int error_port = -1;
		try
		{
			error_port = Integer.parseInt(properties.getProperty("jDispatcher.error_port"));
		}
		catch (final Exception exc)
		{
			// System.out.println("Cannot read error port");
		}
		if (error_port > 0)
		{
			error_mgr = new ErrorMgr(error_port);
			error_mgr.start();
		}
		info_port = 0;
		try
		{
			info_port = Integer.parseInt(properties.getProperty("jDispatcher.info_port"));
		}
		catch (final Exception exc)
		{
			System.out.println("Cannot read info port");
		}
		for (final ServerInfo srvInfo : MdsHelper.getServers())
			serversInfo.put(srvInfo.getAddress(), srvInfo);
		serversInfoPanel = new ServersInfoPanel();
		serversInfoPanel.setServersInfo(this.serversInfo);
		try
		{
			if (monitor_server != null && monitor_server.length() != 0)
				openConnection(monitor_server);
		}
		catch (final IOException e)
		{
			System.out.println(e.getMessage());
		}
		final JMenu file = new JMenu("File");
		final JMenuItem open = new JMenuItem("Open Connection ...");
		open.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				openConnection();
			}
		});
		file.add(open);
		final JMenuItem exit = new JMenuItem("Exit");
		exit.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				exitApplication();
			}
		});
		file.add(exit);
		final JMenu view = new JMenu("View");
		final JCheckBoxMenuItem build_cb = new JCheckBoxMenuItem("Build", true);
		build_cb.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				num_window += build.isShowing() ? -1 : 1;
				build.setVisible(!build.isShowing());
			}
		});
		view.add(build_cb);
		final JCheckBoxMenuItem executing_cb = new JCheckBoxMenuItem("Executing", true);
		executing_cb.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				num_window += executing.isShowing() ? -1 : 1;
				executing.setVisible(!executing.isShowing());
			}
		});
		view.add(executing_cb);
		final JCheckBoxMenuItem failed_cb = new JCheckBoxMenuItem("Failed", true);
		failed_cb.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				num_window += failed.isShowing() ? -1 : 1;
				failed.setVisible(!failed.isShowing());
			}
		});
		view.add(failed_cb);
		view.add(new JSeparator());
		final JCheckBoxMenuItem auto_scroll_cb = new JCheckBoxMenuItem("Auto Scroll", true);
		auto_scroll_cb.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				auto_scroll = !auto_scroll;
			}
		});
		view.add(auto_scroll_cb);
		final JMenu info_m = new JMenu("Info");
		/*
		 * JMenuItem showServer_cb = new JMenuItem("Show Server");
		 * showServer_cb.addActionListener(new ActionListener() { public void
		 * actionPerformed(ActionEvent e) { showServers(); } } );
		 * info_m.add(showServer_cb);
		 */
		final JMenuItem statistics_cb = new JMenuItem("Statistics");
		statistics_cb.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				shotsPerformance.print();
			}
		});
		info_m.add(statistics_cb);
		final JMenuItem dispatch = new JMenu("Dispatch");
		for (final MdsHelper.DispatchCmd dispcmd : MdsHelper.getDispatch())
		{
			final JMenuItem item = new JMenuItem(dispcmd.name);
			item.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(final ActionEvent e)
				{
					doCommand(dispcmd.cmd);
				}
			});
			dispatch.add(item);
		}
		mb.add(file);
		mb.add(view);
		mb.add(phase_m);
		// mb.add(info_m);
		mb.add(dispatch);
		final JPanel p0 = new JPanel();
		p0.setBorder(BorderFactory.createRaisedBevelBorder());
		final Box box = new Box(BoxLayout.Y_AXIS);
		final JPanel p = new JPanel();
		p.add(new JLabel("Active actions"));
		p.add((total_actions_l = new JLabel("    0")));
		p.add(new JLabel("Dispatched"));
		p.add((dispatched_l = new JLabel("    0")));
		p.add(new JLabel("Doing"));
		p.add((doing_l = new JLabel("    0")));
		p.add(new JLabel("Done"));
		p.add((done_l = new JLabel("    0")));
		p.add(new JLabel("Failed"));
		p.add((failed_l = new JLabel("    0")));
		JPanel p1 = new JPanel();
		p1.add((exp_l = new JLabel("Experiment:          ")));
		p1.add((shot_l = new JLabel("Shot:        ")));
		p1.add((phase_l = new JLabel("Phase:        ")));
		box.add(p1);
		box.add(p);
		p0.add(box);
		this.getContentPane().add(p0, BorderLayout.NORTH);
		desktop = new JDesktopPane()
		{
			/**
			 *
			 */
			private static final long serialVersionUID = 1L;

			private void positionWindow()
			{
				if (num_window == 0)
					return;
				final Dimension dim = desktop.getSize();
				dim.height = dim.height / num_window;
				int y = 0;
				if (build.isVisible())
				{
					build.setBounds(0, y, dim.width, dim.height);
					y += dim.height;
				}
				if (executing.isVisible())
				{
					executing.setBounds(0, y, dim.width, dim.height);
					y += dim.height;
				}
				if (failed.isVisible())
				{
					failed.setBounds(0, y, dim.width, dim.height);
				}
			}

			@Override
			public void setBounds(final int x, final int y, final int width, final int height)
			{
				super.setBounds(x, y, width, height);
				positionWindow();
			}
		};
		buildList = new JList<>(build_list);
		buildList.setCellRenderer(new BuildCellRenderer());
		final JScrollPane sp_build = new JScrollPane();
		sp_build.getViewport().setView(buildList);
		executingList = new ToolTipJList(executing_list);
		executingList.setToolTipText("");
		executingList.setCellRenderer(new ExecutingCellRenderer());
		final JScrollPane sp_executing = new JScrollPane();
		sp_executing.getViewport().setView(executingList);
		do_redispatch = new JMenuItem("Redispatch");
		do_redispatch.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				try
				{
					for (final MdsMonitorEvent event : executingList.getSelectedValuesList())
						redispatch(event);
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(jDispatchMonitor.this, "Error dispatching action(s) : " + exc,
							"Alert", JOptionPane.ERROR_MESSAGE);
				}
			}
		});
		do_command_menu.add(do_redispatch);
		do_abort = new JMenuItem("Abort");
		do_abort.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				try
				{
					for (final MdsMonitorEvent event : executingList.getSelectedValuesList())
						abortAction(event);
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(jDispatchMonitor.this, "Error Aborting Action", "Alert",
							JOptionPane.ERROR_MESSAGE);
				}
			}
		});
		do_command_menu.add(do_abort);
		executingList.addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(final MouseEvent e)
			{
				if (MdsHelper.getDispatcher() != null && (e.getModifiers() & Event.META_MASK) != 0)
				{
					final MdsMonitorEvent event = executingList.getSelectedValue();
					if (event == null)
					{
						if (executingList.getModel().getSize() != 0)
							JOptionPane.showMessageDialog(jDispatchMonitor.this,
									"Please select the action(s) to re-dispatch or abort", "Warning",
									JOptionPane.INFORMATION_MESSAGE);
						return;
					}
					if (event.mode == MdsMonitorEvent.MonitorDoing && executingList.getSelectedIndices().length == 1)
					{
						do_redispatch.setEnabled(false);
						do_abort.setEnabled(true);
					}
					else
					{
						do_redispatch.setEnabled(true);
						do_abort.setEnabled(false);
					}
					final int x = e.getX();
					final int y = e.getY();
					do_command_menu.show(executingList, x, y);
				}
			}
		});
		failedList = new ToolTipJList(failed_list);
		failedList.setToolTipText("");
		failedList.setCellRenderer(new FailedCellRenderer());
		final JScrollPane sp_failed = new JScrollPane();
		sp_failed.getViewport().setView(failedList);
		build = createInternalFrame("Build Table", sp_build);
		executing = createInternalFrame("Executing", sp_executing);
		failed = createInternalFrame("Failed", sp_failed);
		final JTabbedPane dispatchMonitorPane = new JTabbedPane();
		// this.getContentPane().add(desktop, BorderLayout.CENTER);
		this.getContentPane().add(dispatchMonitorPane, BorderLayout.CENTER);
		dispatchMonitorPane.addTab("Actions Monitor", desktop);
		/*
		 * serversInfoPanel.addComponentListener(new ComponentAdapter() { public void
		 * componentShown(ComponentEvent e) { showServers(); } });
		 */
		dispatchMonitorPane.addTab("Servers Info", serversInfoPanel);
		p1 = new JPanel();
		p1.setBackground(Color.white);
		p1.setBorder(BorderFactory.createLoweredBevelBorder());
		JLabel l1 = new JLabel("Dispatched           ");
		l1.setForeground(Color.lightGray);
		p1.add(l1);
		l1 = new JLabel("Doing               ");
		l1.setForeground(Color.blue);
		p1.add(l1);
		l1 = new JLabel("Done                ");
		l1.setForeground(Color.green);
		p1.add(l1);
		l1 = new JLabel("Failed              ");
		l1.setForeground(Color.red);
		p1.add(l1);
		this.getContentPane().add(p1, BorderLayout.SOUTH);
	}

	private void abortAction(final MdsMonitorEvent me) throws IOException
	{
		if (me.mode == MdsMonitorEvent.MonitorDoing)
			doCommand("ABORT", me);
	}

	private boolean addPhaseItem(final int phase, final String phase_st)
	{
		for (final Enumeration<?> e = phase_group.getElements(); e.hasMoreElements();)
		{
			final JCheckBoxMenuItem c = (JCheckBoxMenuItem) e.nextElement();
			if (c.getText().equals("Phase_" + phase))
				return false;
		}
		final JCheckBoxMenuItem phase_cb = new JCheckBoxMenuItem(phase_st);
		phase_cb.setActionCommand("" + phase);
		phase_cb.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				final String cmd = ((JCheckBoxMenuItem) e.getSource()).getActionCommand();
				show_phase = Integer.parseInt(cmd);
				showPhase(cmd);
			}
		});
		phase_group.add(phase_cb);
		phase_m.add(phase_cb);
		return true;
	}

	private void collectServersInfo()
	{
		try
		{
			final String address = new StringTokenizer(this.monitor_server).nextToken(":");
			if (!serversInfoPanel.checkInfoServer(address, info_port))
			{
				serversInfoPanel.loadServerState(address, info_port);
			}
			else
				serversInfoPanel.updateServersState();
		}
		catch (final Exception exc)
		{
			final ShowMessage alert = new ShowMessage(exc.getMessage());
			SwingUtilities.invokeLater(alert);
		}
	}

	private void counter(final MdsMonitorEvent me)
	{
		switch (me.mode)
		{
		case MdsMonitorEvent.MonitorBuildBegin:
			total_count++;
			break;
		case MdsMonitorEvent.MonitorBuild:
		case MdsMonitorEvent.MonitorBuildEnd:
			total_count++;
			break;
		case MdsMonitorEvent.MonitorDispatched:
			disp_count++;
			break;
		case MdsMonitorEvent.MonitorDoing:
			doing_count++;
			break;
		case MdsMonitorEvent.MonitorDone:
			doing_count--;
			done_count++;
			if ((me.ret_status & 1) == 0)
				failed_count++;
			break;
		}
		dispatched_l.setText(" " + disp_count);
		doing_l.setText(" " + doing_count);
		done_l.setText(" " + done_count);
		total_actions_l.setText(" " + total_count);
		failed_l.setText(" " + failed_count);
	}

	/**
	 * Create an internal frame
	 */
	public JInternalFrame createInternalFrame(final String title, final Container panel)
	{
		final JInternalFrame jif = new JInternalFrame();
		jif.setTitle(title);
		// set properties
		jif.setClosable(false);
		jif.setMaximizable(true);
		jif.setIconifiable(false);
		jif.setResizable(true);
		jif.setContentPane(panel);
		desktop.add(jif, 1);
		jif.setVisible(true);
		num_window++;
		return jif;
	}

	private final void doCommand(final String command)
	{
		new Thread(() ->
		{
			try
			{
				getDispatcher().MdsValueStraight('@' + command);
			}
			catch (final Exception e)
			{}
		}).start();
	}

	private void doCommand(final String command, final MdsMonitorEvent me) throws IOException
	{
		doCommand(String.format("%s %d %s", command, me.nid, MdsHelper.toPhaseName(me.phase)));
	}

	private void endPhase(final MdsMonitorEvent me)
	{
		final String phaseDuration = MdsMonitorEvent.msecToString(System.currentTimeMillis() - startPhaseTime);
		final String phase_st = MdsHelper.toPhaseName(me.phase);
		logMonitor("END", phase_st, "duration ", phaseDuration);
		phase_m.setEnabled(true);
		this.shotsPerformance.add(shot, phase_st, phaseDuration);
	}

	public void exitApplication()
	{
		shotsPerformance.saveTimePhaseExecution();
		System.exit(0);
	}

	private final MdsConnection getDispatcher() throws Exception
	{
		if (dispatcher == null)
		{
			if (monitor_server == null)
				throw new Exception("No monitor server.");
			final StringTokenizer st = new StringTokenizer(monitor_server, ":");
			dispatcher = new MdsConnection(st.nextToken() + ":" + MdsHelper.getDispatcherPort());
			dispatcher.ConnectToMds(false);
		}
		return dispatcher;
	}

	private int getIndex(final MdsMonitorEvent me, int idx)
	{
		Hashtable<Integer, MdsMonitorEvent> actions = null;
		int ph = -1;
		if (show_phase != curr_phase)
			ph = me.phase = show_phase;
		else
			ph = me.phase;
		if (phase_hash.containsKey(new Integer(ph)))
			actions = phase_hash.get(new Integer(ph));
		else
		{
			final String phase_st = MdsHelper.toPhaseName(me.phase);
			// final boolean new_phase =
			addPhaseItem(me.phase, phase_st);
			/*
			 * executing_list.removeAllElements(); phase_l.setText("Phase: "+phase_st);
			 * curr_phase = show_phase = me.phase;
			 */
			actions = new Hashtable<>();
			phase_hash.put(new Integer(me.phase), actions);
			idx = 0;
		}
		if (actions == null)
			return ACTION_NOT_FOUND;
		if (actions.containsKey(new Integer(me.nid)))
		{
			final MdsMonitorEvent e = actions.get(new Integer(me.nid));
			// System.out.println("Replace action nid "+ e.nid + " " + e.error_message);
			// Check if the current MdsMonitorActionEvenr is added
			// by ErrorMgr thread
			if (e.tree == null)
			{
				e.tree = me.tree;
				e.shot = me.shot;
				e.name = me.name;
				e.on = me.on;
				e.mode = me.mode;
				e.server = me.server;
			}
			if (me.mode == MdsMonitorEvent.MonitorDone)
			{
				// If there is an error message in the stored MdsMonitorEvent
				// it must be appended at the error message field of the
				// current MdsMonitorEvent
				if (e.error_message != null && e.error_message.indexOf("SS-W-NOMSG") == -1)
				{
					// me.error_message += " " + e.error_message;
					e.error_message = new String(e.error_message + me.error_message);
				}
				else
				{
					e.error_message = new String(me.error_message);
				}
				final long start_date = e.date.getTime();
				final long end_date = me.date.getTime();
				e.execution_time = end_date - start_date;
				me.execution_time = e.execution_time;
			}
			e.ret_status = me.ret_status;
			e.mode = me.mode;
			e.date = me.date;
			return e.jobid;
		}
		else
		{
			me.jobid = idx;
			actions.put(new Integer(me.nid), me);
			return -1;
		}
	}

	public void openConnection()
	{
		try
		{
			if (mds_server != null)
			{
				if (mds_server.isConnected())
				{
					final Object[] options =
					{ "DISCONNECT", "CANCEL" };
					final int val = JOptionPane.showOptionDialog(jDispatchMonitor.this,
							"Dispatch monitor is already connected to " + mds_server.getProvider(), "Warning",
							JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[0]);
					if (val == 0)
					{
						mds_server.shutdown();
						mds_server = null;
						setWindowTitle();
					}
					else
						return;
				}
				else
					mds_server.shutdown();
			}
			final JComboBox<?> cb = new JComboBox<>();
			cb.setEditable(true);
			monitor_server = (String) JOptionPane.showInputDialog(jDispatchMonitor.this,
					"Dispatch monitor server ip:port address", "Connection", JOptionPane.QUESTION_MESSAGE, null, null,
					monitor_server);
			if (monitor_server == null)
				return;
			openConnection(monitor_server);
		}
		catch (final IOException exc)
		{
			if (0 == JOptionPane.showConfirmDialog(jDispatchMonitor.this,
					exc.getMessage() + "\nWe will try to spawn a local instance if that is OK?",
					"Spawn local jDispatcherIp?", JOptionPane.OK_CANCEL_OPTION))
				;
			spawnDispatcherIp();
		}
	}

	public void openConnection(final String mon_srv) throws IOException
	{
		if (mon_srv == null)
			throw (new IOException("Can't connect to null address"));
		mds_server = new MdsServer(mon_srv, true, -1);
		mds_server.addMdsServerListener(jDispatchMonitor.this);
		// mds_server.addConnectionListener(jDispatchMonitor.this);
		mds_server.monitorCheckin();
		setWindowTitle();
		dispatcher = null;
		collectServersInfo();
	}

	@Override
	public synchronized void processConnectionEvent(final ConnectionEvent e)
	{
		if (e.getSource() == mds_server)
		{
			if (e.getID() == ConnectionEvent.LOST_CONNECTION)
			{
				// do the following on the gui thread
				final ShowMessage alert = new ShowMessage(e.getInfo());
				SwingUtilities.invokeLater(alert);
				setWindowTitle();
				mds_server = null;
				/*
				 * try { if(dispatcher != null); dispatcher.shutdown(); dispatcher = null; }
				 * catch (Exception exc) {dispatcher = null;} return;
				 */
			}
		}
		if (e.getSource() == dispatcher)
		{
			if (e.getID() == ConnectionEvent.LOST_CONNECTION)
				dispatcher = null;
		}
	}

	@Override
	public synchronized void processMdsServerEvent(final MdsServerEvent e)
	{
		if (e instanceof MdsMonitorEvent)
		{
			final MdsMonitorEvent me = (MdsMonitorEvent) e;
			final UpdateList ul = new UpdateList(me);
			try
			{
				// SwingUtilities.invokeAndWait(ul);
				SwingUtilities.invokeLater(ul);
			}
			catch (final Exception exc)
			{
				System.out.println("processMdsServerEvent " + exc);
			}
		}
	}

	private void redispatch(final MdsMonitorEvent me) throws IOException
	{
		if (me.mode == MdsMonitorEvent.MonitorDone)
			doCommand("REDISPATCH", me);
	}

	private void resetAll(final MdsMonitorEvent me)
	{
		total_count = 0;
		doing_count = 0;
		done_count = 0;
		failed_count = 0;
		disp_count = 0;
		phase_m.removeAll();
		build_list.removeAllElements();
		failed_list.removeAllElements();
		executing_list.removeAllElements();
		phase_hash.clear();
		phase_name.clear();
		phase_failed.clear();
		if (me != null)
		{
			shotsPerformance.saveTimePhaseExecution();
			tree = me.tree;
			shot = me.shot;
			exp_l.setText("Experiment: " + tree);
			shot_l.setText("Shot: " + shot);
			phase_l.setText("Phase: " + MdsHelper.toPhaseName(me.phase));
		}
	}

	private void setWindowTitle()
	{
		if (mds_server != null && mds_server.isConnected())
			setTitle("jDispatchMonitor - Connected to " + mds_server.getProvider() + " receive on port "
					+ mds_server.rcv_port);
		else
			setTitle("jDispatchMonitor - Not Connected");
	}

	private void showPhase(final String phase)
	{
		Hashtable<Integer, MdsMonitorEvent> actions;
		MdsMonitorEvent me;
		// int phase_id = Integer.parseInt(phase);
		if (phase_hash.containsKey(new Integer(phase)))
		{
			show_phase = Integer.parseInt(phase);
			executing_list.removeAllElements();
			failed_list.removeAllElements();
			actions = phase_hash.get(new Integer(phase));
			executing_list.setSize(actions.size());
			int s_done = 0, s_failed = 0;
			for (final Enumeration<MdsMonitorEvent> e = actions.elements(); e.hasMoreElements();)
			{
				me = e.nextElement();
				executing_list.set(me.jobid, me);
				s_done++;
				if ((me.ret_status & 1) == 0)
				{
					failed_list.addElement(me);
					s_failed++;
				}
			}
			if (curr_phase != show_phase)
			{
				phase_l.setText("Phase : " + MdsHelper.toPhaseName(show_phase));
				executing.setTitle("Executed in " + MdsHelper.toPhaseName(show_phase) + " phase Done :" + s_done
						+ " Failed: " + s_failed);
				failed.setTitle("Failed in " + MdsHelper.toPhaseName(show_phase));
			}
			else
			{
				phase_l.setText("Phase : " + MdsHelper.toPhaseName(curr_phase));
				executing.setTitle("Executing");
				failed.setTitle("Failed");
				show_phase = curr_phase;
			}
		}
	}

	private void showUpdateAction(final JList<MdsMonitorEvent> list, final int idx)
	{
		SwingUtilities.invokeLater(new UpdateHandler(list, idx));
	}

	private final void spawnDispatcherIp()
	{
		if (dispatcher_proc == null)
		{
			final String sep = System.getProperty("file.separator");
			final String classpath = System.getProperty("java.class.path");
			final String path = System.getProperty("java.home") + sep + "bin" + sep + "java";
			final ProcessBuilder processBuilder = new ProcessBuilder(path, "-cp", classpath,
					jDispatcherIp.class.getName(), experiment);
			try
			{
				dispatcher_proc = processBuilder.start();
				Runtime.getRuntime().addShutdownHook(new Thread(() ->
				{ dispatcher_proc.destroy(); }));
				new Thread(() ->
				{
					for (;;)
						try
						{
							Thread.sleep(1000);
							openConnection(monitor_server);
							break;
						}
						catch (final Exception e)
						{
							continue;
						}
				}).start();
			}
			catch (final IOException e)
			{}
		}
	}

	private void startPhase(final MdsMonitorEvent me)
	{
		startPhaseTime = System.currentTimeMillis();
		final String phase_st = MdsHelper.toPhaseName(me.phase);
		logMonitor("START", phase_st, " ", me);
		executing_list.removeAllElements();
		phase_l.setText("Phase: " + phase_st);
		phase_m.setEnabled(false);
		curr_phase = show_phase = me.phase;
	}
}
