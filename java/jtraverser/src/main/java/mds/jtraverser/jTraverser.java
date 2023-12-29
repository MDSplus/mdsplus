package mds.jtraverser;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

public class jTraverser extends JFrame implements ActionListener
{
	private static final long serialVersionUID = 1L;
	static String exp_name, shot_name;
	static boolean editable, readonly, model;
	static Tree tree;
	static JLabel status = new JLabel("jTaverser started");

	public static String getExperimentName()
	{ return exp_name; }

	public static boolean isEditable()
	{ return editable; }

	public static void main(String args[])
	{
		if (args.length >= 3)
			FrameRepository.frame = new jTraverser(args[0], args[1], args[2]);
		else if (args.length == 2)
			FrameRepository.frame = new jTraverser(args[0], args[1], null);
		else if (args.length == 1)
			FrameRepository.frame = new jTraverser(args[0], null, null);
		else
			FrameRepository.frame = new jTraverser(null, null, null);
	}

	public static void stderr(String line, Exception exc)
	{
		jTraverser.status.setText("ERROR: " + line + " (" + exc.getMessage() + ")");
		System.err.println(line + "\n" + exc);
	}

	public static void stdout(String line)
	{
		status.setText(line);
	}

	JMenu file_m, edit_m, data_m, customize_m;
	JMenuItem open, close, quit;
	JMenuItem add_action_b, add_dispatch_b, add_numeric_b, add_signal_b, add_any_b, add_task_b, add_text_b, add_window_b,
			add_axis_b, add_device_b, add_child_b, add_subtree_b, delete_node_b, modify_tags_b, rename_node_b,
			turn_on_b, turn_off_b, display_data_b, display_nci_b, display_tags_b, modify_data_b, set_default_b,
			setup_device_b, do_action_b, outline_b, tree_b, copy_b, paste_b;
	TreeDialog display_data_d, modify_data_d, display_nci_d, display_tags_d;
	DisplayData display_data;
	DisplayNci display_nci;
	DisplayTags display_tags;
	ModifyData modify_data;

	String expName;
	int expShot;
	boolean expEdit, expReadonly;

	public jTraverser(String exp_name, String shot_name, String access)
	{
		jTraverser.exp_name = exp_name;
		jTraverser.shot_name = shot_name;
		setTitle("jTraverser - no tree open");
		Boolean edit = false;
		Boolean readonly = false;
		if (access != null)
		{
			edit = (new String(access).equals("-edit"));
			readonly = (new String(access).equals("-readonly"));
		}
		final JMenuBar menu_bar = new JMenuBar();
		setJMenuBar(menu_bar);
		JMenu curr_menu = new JMenu("File");
		file_m = curr_menu;
		menu_bar.add(curr_menu);
		open = new JMenuItem("Open");
		curr_menu.add(open);
		open.addActionListener(this);
		close = new JMenuItem("Close");
		curr_menu.add(close);
		close.addActionListener(this);
		quit = new JMenuItem("Quit");
		curr_menu.add(quit);
		quit.addActionListener(this);
		edit_m = curr_menu = new JMenu("Edit");
		menu_bar.add(curr_menu);
		JMenuItem jm = new JMenu("Add Node");
		jm.add(add_action_b = new JMenuItem("Action"));
		add_action_b.addActionListener(this);
		jm.add(add_dispatch_b = new JMenuItem("Dispatch"));
		add_dispatch_b.addActionListener(this);
		jm.add(add_numeric_b = new JMenuItem("Numeric"));
		add_numeric_b.addActionListener(this);
		jm.add(add_signal_b = new JMenuItem("Signal"));
		add_signal_b.addActionListener(this);
		jm.add(add_any_b = new JMenuItem("Any"));
		add_any_b.addActionListener(this);
		jm.add(add_task_b = new JMenuItem("Task"));
		add_task_b.addActionListener(this);
		jm.add(add_text_b = new JMenuItem("Text"));
		add_text_b.addActionListener(this);
		jm.add(add_window_b = new JMenuItem("Window"));
		add_window_b.addActionListener(this);
		jm.add(add_axis_b = new JMenuItem("Axis"));
		add_axis_b.addActionListener(this);
		curr_menu.add(jm);
		curr_menu.add(add_device_b = new JMenuItem("Add Device"));
		add_device_b.addActionListener(this);
		curr_menu.add(add_child_b = new JMenuItem("Add Child"));
		add_child_b.addActionListener(this);
		curr_menu.add(add_subtree_b = new JMenuItem("Add Subtree"));
		add_subtree_b.addActionListener(this);
		curr_menu.add(delete_node_b = new JMenuItem("Delete Node"));
		delete_node_b.addActionListener(this);
		curr_menu.add(modify_tags_b = new JMenuItem("Modify tags"));
		modify_tags_b.addActionListener(this);
		curr_menu.add(rename_node_b = new JMenuItem("Rename node"));
		rename_node_b.addActionListener(this);
		curr_menu.add(copy_b = new JMenuItem("Copy"));
		copy_b.addActionListener(this);
		curr_menu.add(paste_b = new JMenuItem("Paste"));
		paste_b.addActionListener(this);
		data_m = curr_menu = new JMenu("Data");
		menu_bar.add(curr_menu);
		curr_menu.add(turn_on_b = new JMenuItem("Turn On"));
		turn_on_b.addActionListener(this);
		curr_menu.add(turn_off_b = new JMenuItem("Turn Off"));
		turn_off_b.addActionListener(this);
		curr_menu.add(display_data_b = new JMenuItem("Display Data"));
		display_data_b.addActionListener(this);
		curr_menu.add(display_nci_b = new JMenuItem("Display Nci"));
		display_nci_b.addActionListener(this);
		curr_menu.add(display_tags_b = new JMenuItem("Display Tags"));
		display_tags_b.addActionListener(this);
		curr_menu.add(modify_data_b = new JMenuItem("Modify Data"));
		modify_data_b.addActionListener(this);
		curr_menu.add(set_default_b = new JMenuItem("Set Default"));
		set_default_b.addActionListener(this);
		curr_menu.add(setup_device_b = new JMenuItem("Setup Device"));
		setup_device_b.addActionListener(this);
		curr_menu.add(do_action_b = new JMenuItem("Do Action"));
		do_action_b.addActionListener(this);
		curr_menu = new JMenu("Customize");
		menu_bar.add(curr_menu);
		jm = new JMenu("Display Mode");
		jm.add(outline_b = new JMenuItem("Outline"));
		outline_b.addActionListener(this);
		jm.add(tree_b = new JMenuItem("Tree"));
		tree_b.addActionListener(this);
		curr_menu.add(jm);
		tree = new Tree(this);
		if (exp_name != null)
		{
		    this.expName = exp_name;
		    this.expShot = (shot_name == null) ? -1 : Integer.parseInt(shot_name);
		    this.expEdit = expEdit;
		    this.expReadonly = readonly;
		    SwingUtilities.invokeLater(new Runnable()
		    {
			public void run()
			{
				jTraverser.this.tree.open(jTraverser.this.expName.toUpperCase(), jTraverser.this.expShot, 
					jTraverser.this.expEdit, jTraverser.this.expReadonly, false);
			}
		   });
		}
		getContentPane().add(tree);
		getContentPane().add(status, BorderLayout.PAGE_END);
		addWindowListener(new WindowAdapter()
		{
			@Override
			public void windowClosing(WindowEvent e)
			{
				System.exit(0);
			}
		});
		pack();
		setVisible(true);
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		final Object source = e.getSource();
		if (source == open)
			tree.open();
		if (source == close)
			tree.close();
		if (source == quit)
			tree.quit();
		if (source == tree_b)
			tree.setAngled(true);
		if (source == outline_b)
			tree.setAngled(false);
		if (source == add_action_b)
			tree.addNode("ACTION");
		if (source == add_dispatch_b)
			tree.addNode("DISPATCH");
		if (source == add_numeric_b)
			tree.addNode("NUMERIC");
		if (source == add_signal_b)
			tree.addNode("SIGNAL");
		if (source == add_any_b)
			tree.addNode("ANY");
		if (source == add_task_b)
			tree.addNode("TASK");
		if (source == add_text_b)
			tree.addNode("TEXT");
		if (source == add_child_b)
			tree.addNode("STRUCTURE");
		if (source == add_window_b)
			tree.addNode("WINDOW");
		if (source == add_axis_b)
			tree.addNode("AXIS");
		if (source == add_subtree_b)
			tree.addSubtree();
		if (source == delete_node_b)
			tree.deleteNode();
		if (source == modify_tags_b)
			tree.modifyTags();
		if (source == rename_node_b)
			Tree.dialogs.Rename.show();
		if (source == add_device_b)
			tree.addDevice();
		if (source == copy_b)
			TreeNode.copy();
		if (source == paste_b)
			TreeNode.paste();
		// Node related
		final Node currnode = Tree.getCurrentNode();
		if (currnode == null)
			return;
		if (source == turn_on_b)
		{
			currnode.turnOn();
			tree.reportChange();
		}
		if (source == turn_off_b)
		{
			currnode.turnOff();
			tree.reportChange();
		}
		if (source == display_data_b)
		{
			if (display_data_d == null)
			{
				display_data_d = new TreeDialog(display_data = new DisplayData());
				display_data.setFrame(display_data_d);
			}
			display_data.setNode(currnode);
			display_data_d.pack();
			display_data_d.setLocation(dialogLocation());
			display_data_d.setVisible(true);
		}
		if (source == display_nci_b)
		{
			if (display_nci_d == null)
			{
				display_nci_d = new TreeDialog(display_nci = new DisplayNci());
				display_nci.setFrame(display_nci_d);
			}
			display_nci.setNode(currnode);
			display_nci_d.pack();
			display_nci_d.setLocation(dialogLocation());
			display_nci_d.setVisible(true);
		}
		if (source == display_tags_b)
		{
			if (display_tags_d == null)
			{
				display_tags_d = new TreeDialog(display_tags = new DisplayTags());
				display_tags.setFrame(display_tags_d);
			}
			display_tags.setNode(currnode);
			display_tags_d.pack();
			display_tags_d.setLocation(dialogLocation());
			display_tags_d.setVisible(true);
		}
		if (source == modify_data_b)
		{
			if (modify_data_d == null)
			{
				modify_data_d = new TreeDialog(modify_data = new ModifyData());
				modify_data.setFrame(modify_data_d);
			}
			modify_data.setNode(currnode);
			modify_data_d.pack();
			modify_data_d.setLocation(dialogLocation());
			modify_data_d.setVisible(true);
		}
		if (source == set_default_b)
		{
			try
			{
				currnode.setDefault();
			}
			catch (final Exception exc)
			{
				jTraverser.stderr("Error setting default", exc);
			}
			tree.reportChange();
		}
		if (source == setup_device_b)
			currnode.setupDevice();
	}

	@Override
	public Component add(Component component)
	{
		return getContentPane().add(component);
	}

	public Point dialogLocation()
	{
		return new Point(getLocation().x + 32, getLocation().y + 32);
	}

	void reportChange(String exp, int shot, boolean editable, boolean readonly)
	{
		jTraverser.model = shot < 0;
		jTraverser.editable = editable;
		jTraverser.readonly = readonly;
		jTraverser.exp_name = exp;
		String title;
		if (exp != null)
		{
			title = "Tree: " + exp;
			if (editable)
				title = title + " (edit) ";
			if (readonly)
				title = title + " (readonly) ";
			title = title + " Shot: " + shot;
			jTraverser.stdout(title);
		}
		else
		{
			title = "no tree open";
			edit_m.setEnabled(false);
			data_m.setEnabled(false);
		}
		setTitle("jTraverser - " + title);
		if (exp == null)
			return;
		data_m.setEnabled(true);
		if (editable)
		{
			edit_m.setEnabled(true);
			modify_data_b.setEnabled(true);
			turn_on_b.setEnabled(true);
			turn_off_b.setEnabled(true);
		}
		if (readonly)
		{
			edit_m.setEnabled(false);
			modify_data_b.setEnabled(false);
			turn_on_b.setEnabled(false);
			turn_off_b.setEnabled(false);
		}
		if (!editable && !readonly)
		{
			edit_m.setEnabled(false);
			modify_data_b.setEnabled(true);
			turn_on_b.setEnabled(true);
			turn_off_b.setEnabled(true);
		}
	}
}
