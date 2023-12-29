package mds.jtraverser;

//package jTraverser;
import java.awt.*;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.event.*;
import java.beans.*;
import java.util.Stack;
import java.util.Vector;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;

public class Tree extends JScrollPane implements TreeSelectionListener, MouseListener, ActionListener, KeyListener
{
	static class dialogs
	{
		static class flags
		{
			private static JDialog dialog;
			private static JCheckBox[] flag;
			private static boolean[] settable_flag;
			private static JButton close_b;
			private static JButton update_b;

			public static void close()
			{
				if (dialog != null)
					dialog.setVisible(false);
			}

			private static void construct()
			{
				dialog = new JDialog(frame);
				dialog.setFocusableWindowState(false);
				final JPanel jp = new JPanel();
				jp.setLayout(new BorderLayout());
				final JPanel jp1 = new JPanel();
				jp1.setLayout(new GridLayout(8, 4));
				flag = new JCheckBox[32];
				jp1.add(flag[13] = new JCheckBox("PathReference"));
				jp1.add(flag[14] = new JCheckBox("NidReference"));
				jp1.add(flag[5] = new JCheckBox("Segmented"));
				jp1.add(flag[8] = new JCheckBox("Compressible"));
				jp1.add(flag[1] = new JCheckBox("ParentOff"));
				jp1.add(flag[4] = new JCheckBox("Versions"));
				jp1.add(flag[16] = new JCheckBox("CompressSegments"));
				jp1.add(flag[9] = new JCheckBox("DoNotCompress"));
				jp1.add(flag[0] = new JCheckBox("Off"));
				jp1.add(flag[6] = new JCheckBox("Setup"));
				jp1.add(flag[2] = new JCheckBox("Essential"));
				jp1.add(flag[10] = new JCheckBox("CompressOnPut"));
				jp1.add(flag[11] = new JCheckBox("NoWriteModel"));
				jp1.add(flag[12] = new JCheckBox("NoWriteShot"));
				jp1.add(flag[7] = new JCheckBox("WriteOnce"));
				jp1.add(flag[15] = new JCheckBox("IncludeInPulse"));
				jp1.add(flag[3] = new JCheckBox("Cached"));
				for (byte i = 17; i < 31; i++)
					jp1.add(flag[i] = new JCheckBox("UndefinedFlag" + (i)));
				jp1.add(flag[31] = new JCheckBox("Error"));
				settable_flag = new boolean[]
				{ true, false, true, true, false, false, true, true, false, true, true, true, true, false, false, true,
						true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
						false };
				flag[0].addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						final Node currnode = Tree.getCurrentNode();
						if (currnode == null)
							return;
						if (flag[0].isSelected())
							currnode.turnOff();
						else
							currnode.turnOn();
						jTraverser.tree.reportChange();
					}
				});
				for (byte i = 1; i < 32; i++)
					if (flag[i] != null)
					{
						final byte ii = i;
						flag[i].addActionListener(new ActionListener()
						{
							@Override
							public void actionPerformed(ActionEvent e)
							{
								editFlag(ii);
								jTraverser.tree.reportChange();
							}
						});
					}
				jp.add(jp1);
				final JPanel jp3 = new JPanel();
				jp3.setLayout(new GridLayout(1, 2));
				jp3.add(close_b = new JButton("Close"));
				close_b.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						close();
					}
				});
				jp3.add(update_b = new JButton("Refresh"));
				update_b.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						show();
					}
				});
				jp.add(jp3, "South");
				dialog.getContentPane().add(jp);
				dialog.addKeyListener(new KeyAdapter()
				{
					@Override
					public void keyTyped(KeyEvent e)
					{
						if (e.getKeyCode() == KeyEvent.VK_ESCAPE)
							dialog.setVisible(false);
					}
				});
				dialog.pack();
				dialog.setResizable(false);
			}

			private static void editFlag(byte idx)
			{
				final Node currnode = Tree.getCurrentNode();
				if (currnode == null)
					return;
				if (flag[idx].isSelected())
					try
					{
						currnode.setFlag(idx);
					}
					catch (final Exception exc)
					{
						JOptionPane.showMessageDialog(frame, exc.getMessage(), "Error setting flag" + idx,
								JOptionPane.WARNING_MESSAGE);
					}
				else
					try
					{
						currnode.clearFlag(idx);
					}
					catch (final Exception exc)
					{
						JOptionPane.showMessageDialog(frame, exc.getMessage(), "Error clearing flag " + idx,
								JOptionPane.WARNING_MESSAGE);
					}
				show();
			}

			private static boolean[] readFlags() throws Exception
			{
				int iflags = 0;
				final boolean[] flags = new boolean[32];
				final Node currnode = Tree.getCurrentNode();
				if (currnode == null)
				{
					flags[31] = true;
					return flags;
				}
				iflags = currnode.getFlags();
				if (iflags < 0)
					jTraverser.stderr("MdsJava returned -1.", null);
				for (byte i = 0; i < 32; i++)
					flags[i] = (iflags & (1 << i)) != 0;
				return flags;
			}

			public static void show()
			{
				if (dialog == null)
					construct();
				boolean[] flags;
				try
				{
					flags = readFlags();
				}
				catch (final Exception exc)
				{
					jTraverser.stderr("Error getting flags", exc);
					close();
					return;
				}
				final Node currnode = Tree.getCurrentNode();
				final boolean is_ok = !(jTraverser.readonly || (currnode == null));
				for (int i = 0; i < 32; i++)
				{
					flag[i].setSelected(flags[i]);
					flag[i].setEnabled(is_ok && settable_flag[i]);
				}
				if (currnode == null)
					dialog.setTitle("Flags of <none selected>");
				else
					dialog.setTitle("Flags of " + currnode.getFullPath());
				if (!dialog.isVisible())
				{
					dialog.setLocation(Tree.frame.dialogLocation());
					dialog.setVisible(true);
				}
			}

			public static void update()
			{
				if (dialog == null)
					return;
				if (!dialog.isVisible())
					return;
				show();
			}
		}

		static class Rename
		{
			private static JDialog dialog;
			private static JTextField new_name;

			public static void close()
			{
				if (dialog != null)
					dialog.setVisible(false);
			}

			private static void construct()
			{
				dialog = new JDialog();
				final JPanel mjp = new JPanel();
				mjp.setLayout(new BorderLayout());
				JPanel jp = new JPanel();
				jp.add(new JLabel("New Name: "));
				jp.add(new_name = new JTextField(12));
				mjp.add(jp, "North");
				jp = new JPanel();
				final JButton ok_b = new JButton("Ok");
				ok_b.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						rename();
					}
				});
				jp.add(ok_b);
				final JButton cancel_b = new JButton("Cancel");
				cancel_b.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						dialog.setVisible(false);
					}
				});
				jp.add(cancel_b);
				mjp.add(jp, "South");
				dialog.getContentPane().add(mjp);
				dialog.addKeyListener(new KeyAdapter()
				{
					@Override
					public void keyTyped(KeyEvent e)
					{
						if (e.getKeyCode() == KeyEvent.VK_ENTER)
							rename();
					}
				});
				dialog.pack();
				dialog.setResizable(false);
			}

			private static void rename()
			{
				final Node currnode = Tree.getCurrentNode();
				if (currnode == null)
					return;
				final String name = new_name.getText();
				if (name == null || name.length() == 0)
					return;
				try
				{
				     String path = currnode.getFullPath();
				     int idx;
				     for(idx = path.length() - 1; idx >= 0; idx--)
				     {
					char c = path.charAt(idx);
					if(c == '.' || c == ':' || c == '\\')
					    break;
				     }
				     path = path.substring(idx+1);
				     path = path + name;
				     currnode.rename(path);
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(frame, exc.getMessage(), "Error renaming Node",
							JOptionPane.WARNING_MESSAGE);
					return;
				}
				Tree.curr_tree.treeDidChange();
				dialog.setVisible(false);
			}

			public static void show()
			{
				final Node currnode = Tree.getCurrentNode();
				if (currnode == null)
					return;
				if (dialog == null)
					construct();
				dialog.setTitle("Rename node " + currnode.getFullPath());
				dialog.setLocation(Tree.frame.dialogLocation());
				new_name.setText("");
				dialog.setVisible(true);
			}

			public static void update()
			{// don't update; close instead
				close();
			}
		}

		static void update()
		{
			flags.update();
			Rename.update();
		}
	}

	class DialogSet
	{
		Vector<TreeDialog> dialogs = new Vector<>();

		TreeDialog getDialog(Class<?> ed_class, Node node)
		{
			int idx;
			TreeDialog curr_dialog = null;
			NodeEditor curr_editor;
			for (idx = 0; idx < dialogs.size(); idx++)
			{
				curr_dialog = dialogs.elementAt(idx);
				if (!curr_dialog.inUse())
					break;
			}
			if (idx == dialogs.size())
			{
				try
				{
					curr_editor = (NodeEditor) ((PropertyEditor) ed_class.newInstance()).getCustomEditor();
					curr_dialog = new TreeDialog(curr_editor);
					curr_editor.setFrame(curr_dialog);
					dialogs.addElement(curr_dialog);
				}
				catch (final Exception exc)
				{
					jTraverser.stderr("Error creating node editor", exc);
				}
			}
			curr_dialog.setUsed(true);
			curr_dialog.getEditor().setNode(node);
			return curr_dialog;
		}
	}

	// Inner class FromTranferHandler managed drag operation
	class FromTransferHandler extends TransferHandler
	{
		/**
			 *
			 */
		private static final long serialVersionUID = 1L;

		@Override
		public Transferable createTransferable(JComponent comp)
		{
			if (curr_tree == null)
				return null;
			try
			{
				final String fullPath = Tree.getCurrentNode().getFullPath();
				// int colonIdx;
				// if(fullPath.charAt(0) == '\\' && (colonIdx = fullPath.indexOf(':')) != -1)
				// //If is in the form \tree::tag
				// fullPath = "\\"+fullPath.substring(colonIdx+ 2);
				// return new StringSelection(topExperiment + ":" + fullPath);
				return new StringSelection(fullPath);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		@Override
		public int getSourceActions(JComponent comp)
		{
			return COPY_OR_MOVE;
		}
	}

	class MDSCellRenderer extends DefaultTreeCellRenderer
	{
		/**
			 *
			 */
		private static final long serialVersionUID = 1L;

		@Override
		public Component getTreeCellRendererComponent(JTree tree, Object value, boolean isSelected, boolean expanded,
				boolean hasFocus, int row, boolean leaf)
		{
			final Object usrObj = ((DefaultMutableTreeNode) value).getUserObject();
			Node node;
			if (usrObj instanceof String)
			{
				node = Tree.getCurrentNode();
				if (node.getTreeNode() == value)
				{
					final String newName = (((String) usrObj).trim()).toUpperCase();
					if (lastName == null || !lastName.equals(newName))
					{
						lastName = newName;
						node.rename(newName);
					}
					node.getTreeNode().setUserObject(node);
				}
			}
			else
				node = (Node) usrObj;
			if (isSelected)
				Tree.dialogs.update();
			return node.getIcon(isSelected);
		}
	}

	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	static jTraverser frame;
	static int curr_dialog_idx;
	static JTree curr_tree;
	private static Node curr_node;
	static MDSplus.Tree curr_experiment;
	static int context;

	public static Node addDevice(String name, String type, Node toNode)
	{
		DefaultMutableTreeNode new_tree_node = null;
		if (name == null || name.length() == 0 || name.length() > 12)
		{
			JOptionPane.showMessageDialog(frame, "Name length must range between 1 and 12 characters",
					"Error adding Node", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		if (type == null || type.length() == 0)
		{
			JOptionPane.showMessageDialog(frame, "Missing device type", "Error adding Node",
					JOptionPane.WARNING_MESSAGE);
			return null;
		}
		Node new_node = null;
		try
		{
			new_node = toNode.addDevice(name, type);
			final int num_children = toNode.getTreeNode().getChildCount();
			int i;
			if (num_children > 0)
			{
				String curr_name;
				for (i = 0; i < num_children; i++)
				{
					curr_name = Tree.getNode(toNode.getTreeNode().getChildAt(i)).getName();
					if (name.compareTo(curr_name) < 0)
						break;
				}
				new_node.setTreeNode(new_tree_node = new DefaultMutableTreeNode(new_node));
				final DefaultTreeModel tree_model = (DefaultTreeModel) curr_tree.getModel();
				tree_model.insertNodeInto(new_tree_node, Tree.getCurrTreeNode(), i);
				curr_tree.makeVisible(new TreePath(new_tree_node.getPath()));
				return new_node;
			}
		}
		catch (final Throwable e)
		{
			JOptionPane.showMessageDialog(frame,
					"Add routine for the selected device cannot be activated:\n" + e.getMessage(),
					"Error adding Device", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return new_node;
	}

	public static Node addNode(String usage, String name, Node toNode)
	{
		Node new_node;
		DefaultMutableTreeNode new_tree_node;
		final DefaultMutableTreeNode toTreeNode = toNode.getTreeNode();
		if (name == null || name.length() == 0 || name.length() > 12)
		{
			JOptionPane.showMessageDialog(frame, "Name length must range between 1 and 12 characters",
					"Error adding Node", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		try
		{
			new_node = toNode.addNode(name, usage);
			new_tree_node = new DefaultMutableTreeNode(new_node);
			new_node.setTreeNode(new_tree_node);
			Tree.addNodeToParent(new_tree_node, toTreeNode);
		}
		catch (final Exception e)
		{
			JOptionPane.showMessageDialog(frame, e.getMessage(), "Error adding Node", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return new_node;
	}

	static void addNodeToParent(DefaultMutableTreeNode TreeNode, DefaultMutableTreeNode toTreeNode)
	{
		final int num_children = toTreeNode.getChildCount();
		int i = 0;
		if (num_children > 0)
		{
			final String name = Tree.getNode(TreeNode).getName();
			String curr_name;
			for (i = 0; i < num_children; i++)
			{
				curr_name = ((Node) ((DefaultMutableTreeNode) toTreeNode.getChildAt(i)).getUserObject()).getName();
				if (name.compareTo(curr_name) < 0)
					break;
			}
		}
		final DefaultTreeModel tree_model = (DefaultTreeModel) Tree.curr_tree.getModel();
		tree_model.insertNodeInto(TreeNode, toTreeNode, i);
		Tree.curr_tree.expandPath(new TreePath(TreeNode.getPath()));
		Tree.curr_tree.treeDidChange();
	}

	static MDSplus.Data dataFromExpr(String expr)
	{
		return curr_experiment.tdiCompile(expr);
	}

	static void deleteNode(Node delNode)
	{
		if (delNode == null)
			return;
		final Node del_node = delNode;
		final int n_children = del_node.startDelete();
		String msg = "You are about to delete node " + del_node.getName().trim();
		if (n_children > 0)
			msg += " which has " + n_children + " descendents.\n Please confirm";
		else
			msg += "\n Please confirm";
		final int n = JOptionPane.showConfirmDialog(frame, msg, "Delete node(s)", JOptionPane.YES_NO_OPTION);
		if (n == JOptionPane.YES_OPTION)
		{
			final DefaultTreeModel tree_model = (DefaultTreeModel) curr_tree.getModel();
			tree_model.removeNodeFromParent(delNode.getTreeNode());
			del_node.executeDelete();
		}
	}

	public static Node getCurrentNode()
	{ return curr_node; }

	public static DefaultMutableTreeNode getCurrTreeNode()
	{ return Tree.getCurrentNode().getTreeNode(); }

	public static Node getNode(DefaultMutableTreeNode treenode)
	{
		return (Node) treenode.getUserObject();
	}

	public static Node getNode(javax.swing.tree.TreeNode treenode)
	{
		return Tree.getNode((DefaultMutableTreeNode) treenode);
	}

	public static final void setCurrentNode(final DefaultMutableTreeNode treenode)
	{
		final Node curr_node = Tree.getNode(treenode);
		if (curr_node == null)
			return;
		Tree.setCurrentNode(curr_node);
	}

	private static final void setCurrentNode(final int row)
	{
		if (Tree.curr_tree == null)
			return;
		Tree.curr_tree.setSelectionRow(row);
		final DefaultMutableTreeNode DMN = (DefaultMutableTreeNode) Tree.curr_tree.getLastSelectedPathComponent();
		if (DMN == null)
			return;
		Tree.setCurrentNode(DMN);
	}

	public static final void setCurrentNode(final Node curr_node)
	{
		Tree.curr_tree.setSelectionPath(new TreePath(curr_node.getTreeNode().getPath()));
		Tree.curr_node = curr_node;
	}

	boolean is_angled_style;
	private DefaultMutableTreeNode top;
	private JMenuItem menu_items[];
	private JPopupMenu pop;
	private DialogSet dialog_sets[];
	private final Stack<JTree> trees;
	private final Stack<MDSplus.Tree> experiments;
	private JDialog open_dialog, add_node_dialog, add_subtree_dialog;
	private JTextField open_exp, open_shot;
	private JRadioButton open_readonly, open_edit, open_normal;
	private JTextField add_node_name, add_node_tag, add_subtree_name;
	private String add_node_usage;
	private JDialog modify_tags_dialog;
	private JDialog add_device_dialog;
	private JList<String> modify_tags_list;
	private JTextField curr_tag_selection;
	private DefaultListModel<String> curr_taglist_model;
	private String[] tags;
	private JTextField add_device_type, add_device_name;
	private String lastName;
	// Temporary, to overcome Java's bugs on inner classes
	JMenuItem add_action_b, add_dispatch_b, add_numeric_b, add_signal_b, add_any_b, add_task_b, add_text_b, add_window_b,
			add_axis_b, add_device_b, add_child_b, add_subtree_b, delete_node_b, modify_tags_b;
	JButton ok_cb, add_node_ok;

	public Tree(JFrame _frame)
	{
		this((jTraverser) _frame);
	}

	public Tree(jTraverser _frame)
	{
		frame = _frame;
		trees = new Stack<>();
		experiments = new Stack<>();
		setPreferredSize(new Dimension(300, 400));
		setBackground(Color.white);
		curr_tree = null;
		curr_experiment = null;
		final String def_tree = System.getProperty("tree");
		if (def_tree != null)
		{
			final String def_shot = System.getProperty("shot");
			int shot;
			if (def_shot != null)
				shot = Integer.parseInt(def_shot);
			else
				shot = -1;
			open(def_tree, shot, false, false, false);
		}
	}

	// temporary: to overcome java's bugs for inner classes
	@Override
	public void actionPerformed(ActionEvent e)
	{
		final Object jb = e.getSource();
		if (jb == ok_cb)
			open_ok();
		if (jb == add_action_b)
			addNode("ACTION");
		if (jb == add_dispatch_b)
			addNode("DISPATCH");
		if (jb == add_numeric_b)
			addNode("NUMERIC");
		if (jb == add_signal_b)
			addNode("SIGNAL");
		if (jb == add_any_b)
			addNode("ANY");
		if (jb == add_task_b)
			addNode("TASK");
		if (jb == add_text_b)
			addNode("TEXT");
		if (jb == add_window_b)
			addNode("WINDOW");
		if (jb == add_axis_b)
			addNode("AXIS");
		if (jb == add_node_ok)
			addNode();
		if (jb == add_child_b)
			addNode("STRUCTURE");
		if (jb == add_subtree_b)
			addSubtree();
		if (jb == add_device_b)
			addDevice();
		if (jb == delete_node_b)
			deleteNode();
		if (jb == modify_tags_b)
			modifyTags();
	}

	public void addDevice()
	{
		final Node currnode = Tree.getCurrentNode();
		if (currnode == null)
			return;
		if (add_device_dialog == null)
		{
			add_device_dialog = new JDialog(frame);
			final JPanel jp = new JPanel();
			jp.setLayout(new BorderLayout());
			JPanel jp1 = new JPanel();
			jp1.add(new JLabel("Device: "));
			jp1.add(add_device_type = new JTextField(12));
			jp.add(jp1, "North");
			jp1 = new JPanel();
			jp1.add(new JLabel("Name:   "));
			jp1.add(add_device_name = new JTextField(12));
			jp.add(jp1, "South");
			jp1 = new JPanel();
			JButton ok_button;
			jp1.add(ok_button = new JButton("Ok"));
			ok_button.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					if (addDevice(add_device_name.getText().toUpperCase(), add_device_type.getText()) == null)
						;
					add_device_dialog.setVisible(false);
				}
			});
			final JButton cancel_b = new JButton("Cancel");
			cancel_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					add_device_dialog.setVisible(false);
				}
			});
			jp1.add(cancel_b);
			JPanel jp2;
			jp2 = new JPanel();
			jp2.setLayout(new BorderLayout());
			jp2.add(jp, "North");
			jp2.add(jp1, "South");
			add_device_dialog.getContentPane().add(jp2);
			add_device_dialog.addKeyListener(new KeyAdapter()
			{
				@Override
				public void keyTyped(KeyEvent e)
				{
					if (e.getKeyCode() == KeyEvent.VK_ENTER)
						if (addDevice(add_device_name.getText().toUpperCase(), add_device_type.getText()) == null)
							;
					add_device_dialog.setVisible(false);
				}
			});
			add_device_dialog.pack();
		}
		add_device_name.setText("");
		add_device_type.setText("");
		add_device_dialog.setTitle("Add device to: " + currnode.getFullPath());
		add_device_dialog.setLocation(frame.dialogLocation());
		add_device_dialog.setVisible(true);
	}

	public Node addDevice(String name, String type)
	{
		return Tree.addDevice(name, type, Tree.getCurrentNode());
	}

	public void addNode()
	{
		final Node newNode = addNode(add_node_usage, add_node_name.getText().toUpperCase());
		if (!add_node_tag.getText().trim().equals(""))
		{
			try
			{
				newNode.setTags(new String[]
				{ add_node_tag.getText().trim().toUpperCase() });
			}
			catch (final Exception exc)
			{
				jTraverser.stderr("Error adding tag", exc);
			}
		}
		add_node_dialog.setVisible(false);
	}

	public void addNode(String usage)
	{
		final Node currnode = Tree.getCurrentNode();
		if (currnode == null)
			return;
		add_node_usage = usage;
		if (add_node_dialog == null)
		{
			add_node_dialog = new JDialog(frame);
			final JPanel jp = new JPanel();
			jp.setLayout(new BorderLayout());
			JPanel jp1 = new JPanel();
			jp1.add(new JLabel("Node name: "));
			jp1.add(add_node_name = new JTextField(12));
			jp.add(jp1, "North");
			jp1 = new JPanel();
			jp1.add(new JLabel("Node tag: "));
			jp1.add(add_node_tag = new JTextField(12));
			jp.add(jp1, "Center");
			jp1 = new JPanel();
			jp1.add(add_node_ok = new JButton("Ok"));
			add_node_ok.addActionListener(this);
			final JButton cancel_b = new JButton("Cancel");
			cancel_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					add_node_dialog.setVisible(false);
				}
			});
			jp1.add(cancel_b);
			jp.add(jp1, "South");
			add_node_dialog.getContentPane().add(jp);
			add_node_dialog.addKeyListener(new KeyAdapter()
			{
				@Override
				public void keyTyped(KeyEvent e)
				{
					if (e.getKeyCode() == KeyEvent.VK_ENTER)
						addNode();
				}
			});
			add_node_dialog.pack();
			add_node_dialog.setVisible(true);
		}
		add_node_name.setText("");
		add_node_tag.setText("");
		add_node_dialog.setTitle("Add to: " + currnode.getFullPath());
		add_node_dialog.setLocation(frame.dialogLocation());
		add_node_dialog.setVisible(true);
	}

	public Node addNode(String usage, String name)
	{
		return Tree.addNode(usage, name, Tree.getCurrentNode());
	}

	public void addSubtree()
	{
		final Node currnode = Tree.getCurrentNode();
		if (currnode == null)
			return;
		if (add_subtree_dialog == null)
		{
			add_subtree_dialog = new JDialog(frame);
			final JPanel jp = new JPanel();
			jp.setLayout(new BorderLayout());
			JPanel jp1 = new JPanel();
			jp1.add(new JLabel("Node name: "));
			jp1.add(add_subtree_name = new JTextField(12));
			jp.add(jp1, "North");
			jp1 = new JPanel();
			final JButton ok = new JButton("Ok");
			ok.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					final Node node = addNode("SUBTREE", "." + add_subtree_name.getText().toUpperCase());
					try
					{
						node.setSubtree();
					}
					catch (final Exception exc)
					{
						jTraverser.stderr("Error setting subtree", exc);
					}
					add_subtree_dialog.setVisible(false);
				}
			});
			jp1.add(ok);
			final JButton cancel_b = new JButton("Cancel");
			cancel_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					add_subtree_dialog.setVisible(false);
				}
			});
			jp1.add(cancel_b);
			jp.add(jp1, "South");
			add_subtree_dialog.getContentPane().add(jp);
			add_subtree_dialog.pack();
			add_subtree_dialog.setVisible(true);
		}
		add_subtree_dialog.setTitle("Add Subtree to: " + currnode.getFullPath());
		add_subtree_dialog.setLocation(frame.dialogLocation());
		add_subtree_dialog.setVisible(true);
	}

	public void addTag()
	{
		final String[] out_tags = new String[curr_taglist_model.getSize()];
		for (int i = 0; i < curr_taglist_model.getSize(); i++)
		{
			out_tags[i] = curr_taglist_model.getElementAt(i);
		}
		try
		{
			Tree.getCurrentNode().setTags(out_tags);
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(frame, exc.getMessage(), "Error adding tags", JOptionPane.WARNING_MESSAGE);
		}
		modify_tags_dialog.setVisible(false);
	}

	void close()
	{
		if (curr_tree == null)
			return;
		try
		{
			curr_experiment.close();
		}
		catch (final Exception e)
		{
			boolean editable = false;
			String name = null;
			try
			{
				editable = curr_experiment.isOpenForEdit();
				name = curr_experiment.getName().trim();
			}
			catch (final Exception exc)
			{}
			if (editable)
			{
				final int n = JOptionPane.showConfirmDialog(frame,
						"Tree " + name + " open in edit mode has been changed: Write it before closing?",
						"Closing Tree ", JOptionPane.YES_NO_OPTION);
				if (n == JOptionPane.YES_OPTION)
				{
					try
					{
						curr_experiment.write();
						curr_experiment.close();
					}
					catch (final Exception exc)
					{
						JOptionPane.showMessageDialog(frame, "Error closing tree", exc.getMessage(),
								JOptionPane.WARNING_MESSAGE);
						return;
					}
				}
				else
				{
					try
					{
						curr_experiment.quit();
					}
					catch (final Exception exce)
					{
						JOptionPane.showMessageDialog(frame, "Error quitting tree", exce.getMessage(),
								JOptionPane.WARNING_MESSAGE);
					}
				}
			}
			else
			{
				JOptionPane.showMessageDialog(frame, "Error closing tree", e.getMessage(), JOptionPane.WARNING_MESSAGE);
				return;
			}
		}
		trees.pop();
		experiments.pop();
		if (!trees.empty())
		{
			curr_tree = trees.peek();
			curr_experiment = experiments.peek();
			Tree.setCurrentNode(0);
			setViewportView(curr_tree);
			try
			{
				frame.reportChange(curr_experiment.getName(), curr_experiment.getShot(),
						curr_experiment.isOpenForEdit(), curr_experiment.isReadOnly());
				if (jTraverser.editable != curr_experiment.isOpenForEdit())
					pop = null;
				jTraverser.editable = curr_experiment.isOpenForEdit();
			}
			catch (final Exception exc)
			{
				jTraverser.stderr("Error in RMI communication", exc);
			}
		}
		else
		{
			curr_tree = null;
			curr_experiment = null;
			curr_node = null;
			setViewportView(new JPanel());
			frame.reportChange(null, 0, false, false);
		}
		Database.closeSetups();
		pop = null;
		dialogs.update();
		frame.pack();
		repaint();
	}

	public void dataChanged()
	{
		reportChange();
	}

	void deleteNode()
	{
		deleteNode(Tree.getCurrentNode());
		Tree.setCurrentNode(0);
	}

	@Override
	public void keyPressed(KeyEvent e)
	{}

	@Override
	public void keyReleased(KeyEvent e)
	{}

	@Override
	public void keyTyped(KeyEvent e)
	{}

	public void modifyTags()
	{
		final Node currnode = Tree.getCurrentNode();
		if (currnode == null)
			return;
		try
		{
			tags = currnode.getTags();
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error getting tags", exc);
			tags = new String[0];
		}
		curr_taglist_model = new DefaultListModel<String>();
		for (final String tag : tags)
		{
			curr_taglist_model.addElement(tag);
		}
		if (modify_tags_dialog == null)
		{
			modify_tags_dialog = new JDialog(frame);
			final JPanel jp = new JPanel();
			jp.setLayout(new BorderLayout());
			final JPanel jp1 = new JPanel();
			jp1.setLayout(new BorderLayout());
			modify_tags_list = new JList<String>();
			modify_tags_list.addListSelectionListener(new ListSelectionListener()
			{
				@Override
				public void valueChanged(ListSelectionEvent e)
				{
					final int idx = modify_tags_list.getSelectedIndex();
					if (idx != -1)
						curr_tag_selection.setText(curr_taglist_model.getElementAt(idx));
				}
			});
			final JScrollPane scroll_list = new JScrollPane(modify_tags_list);
			jp1.add(new JLabel("Tag List:"), "North");
			jp1.add(scroll_list, "Center");
			final JPanel jp2 = new JPanel();
			jp2.setLayout(new GridLayout(2, 1));
			final JButton add_tag = new JButton("Add Tag");
			add_tag.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					final String curr_tag = curr_tag_selection.getText().toUpperCase();
					if (curr_tag == null || curr_tag.length() == 0)
						return;
					for (int i = 0; i < curr_taglist_model.getSize(); i++)
						if (curr_tag.equals(curr_taglist_model.getElementAt(i)))
							return;
					curr_taglist_model.addElement(curr_tag);
				}
			});
			jp2.add(add_tag);
			final JButton remove_tag = new JButton("Remove Tag");
			remove_tag.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					int idx;
					if ((idx = modify_tags_list.getSelectedIndex()) != -1)
					{
						curr_taglist_model.removeElementAt(idx);
					}
				}
			});
			jp2.add(remove_tag);
			final JPanel jp4 = new JPanel();
			jp4.add(jp2);
			jp1.add(jp4, "East");
			curr_tag_selection = new JTextField(30);
			final JPanel jp5 = new JPanel();
			jp5.add(new JLabel("Current Selection: "));
			jp5.add(curr_tag_selection);
			jp1.add(jp5, "South");
			jp.add(jp1, "North");
			final JPanel jp3 = new JPanel();
			final JButton ok_b = new JButton("Ok");
			ok_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					addTag();
				}
			});
			jp3.add(ok_b);
			final JButton reset_b = new JButton("Reset");
			reset_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					curr_taglist_model = new DefaultListModel<String>();
					for (final String tag : tags)
						curr_taglist_model.addElement(tag);
					modify_tags_list.setModel(curr_taglist_model);
				}
			});
			jp3.add(reset_b);
			final JButton cancel_b = new JButton("Cancel");
			cancel_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					modify_tags_dialog.setVisible(false);
				}
			});
			jp3.add(cancel_b);
			jp.add(jp3, "South");
			modify_tags_dialog.getContentPane().add(jp);
			modify_tags_dialog.addKeyListener(new KeyAdapter()
			{
				@Override
				public void keyTyped(KeyEvent e)
				{
					if (e.getKeyCode() == KeyEvent.VK_ENTER)
						addTag();
				}
			});
			modify_tags_dialog.pack();
			modify_tags_dialog.setVisible(true);
		}
		modify_tags_dialog.setTitle("Modify tags of " + currnode.getFullPath());
		modify_tags_list.setModel(curr_taglist_model);
		curr_tag_selection.setText("");
		modify_tags_dialog.setLocation(frame.dialogLocation());
		modify_tags_dialog.setVisible(true);
	}

	@Override
	public void mouseClicked(MouseEvent e)
	{}

	@Override
	public void mouseEntered(MouseEvent e)
	{}

	@Override
	public void mouseExited(MouseEvent e)
	{}

	@Override
	public void mousePressed(MouseEvent e)
	{
		if (curr_tree == null)
			return;
		int item_idx;
		final DefaultMutableTreeNode curr_tree_node = (DefaultMutableTreeNode) curr_tree
				.getClosestPathForLocation(e.getX(), e.getY()).getLastPathComponent();
		final Node currnode = Tree.getNode(curr_tree_node);
		Tree.setCurrentNode(currnode);
		if ((e.getModifiers() & InputEvent.BUTTON3_MASK) != 0)
		{
			final NodeBeanInfo nbi = currnode.getBeanInfo();
			final PropertyDescriptor[] node_properties = nbi.getPropertyDescriptors();
			final MethodDescriptor[] node_methods = nbi.getMethodDescriptors();
			if (pop == null)
			{
				JMenuItem mitem;
				dialog_sets = new DialogSet[node_properties.length];
				for (int i = 0; i < node_properties.length; i++)
					dialog_sets[i] = new DialogSet();
				pop = new JPopupMenu();
				menu_items = new JMenuItem[node_properties.length + node_methods.length];
				if (jTraverser.editable)
				{
					final JMenuItem jm = new JMenu("Add Node");
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
					pop.add(jm);
					pop.add(add_device_b = new JMenuItem("Add Device"));
					add_device_b.addActionListener(this);
					pop.add(add_child_b = new JMenuItem("Add Child"));
					add_child_b.addActionListener(this);
					pop.add(add_subtree_b = new JMenuItem("Add Subtree"));
					add_subtree_b.addActionListener(this);
					pop.add(delete_node_b = new JMenuItem("Delete Node"));
					delete_node_b.addActionListener(this);
					pop.add(modify_tags_b = new JMenuItem("Modify tags"));
					modify_tags_b.addActionListener(this);
					pop.add(mitem = new JMenuItem("Rename node"));
					mitem.addActionListener(new ActionListener()
					{
						@Override
						public void actionPerformed(ActionEvent e)
						{
							dialogs.Rename.show();
						}
					});
					pop.addSeparator();
				}
				item_idx = 0;
				for (final PropertyDescriptor node_propertie : node_properties)
				{
					pop.add(menu_items[item_idx] = new JMenuItem(node_propertie.getShortDescription()));
					menu_items[item_idx].addActionListener(new ActionListener()
					{
						@Override
						public void actionPerformed(ActionEvent e)
						{
							int idx;
							for (idx = 0; idx < node_properties.length
									&& !e.getActionCommand().equals(node_properties[idx].getShortDescription()); idx++);
							if (idx < node_properties.length)
							{
								Tree.curr_dialog_idx = idx;
								final TreeDialog curr_dialog = dialog_sets[idx].getDialog(
										node_properties[idx].getPropertyEditorClass(), Tree.getCurrentNode());
								curr_dialog.pack();
								curr_dialog.setLocation(frame.dialogLocation());
								curr_dialog.setVisible(true);
							}
						}
					});
					item_idx++;
				}
				for (final MethodDescriptor node_method : node_methods)
				{
					pop.add(menu_items[item_idx] = new JMenuItem(node_method.getShortDescription()));
					menu_items[item_idx].addActionListener(new ActionListener()
					{
						@Override
						public void actionPerformed(ActionEvent e)
						{
							int idx;
							for (idx = 0; idx < node_methods.length
									&& !e.getActionCommand().equals(node_methods[idx].getShortDescription()); idx++);
							if (idx < node_methods.length)
							{
								try
								{
									node_methods[idx].getMethod().invoke(Tree.getCurrentNode());
								}
								catch (final Exception exc)
								{
									System.out.println("Error executing " + exc);
								}
								// curr_tree.expandPath(new TreePath(curr_tree_node.getPath()));
								curr_tree.treeDidChange();
							}
						}
					});
					item_idx++;
				}
				pop.add(mitem = new JMenuItem("Flags"));
				mitem.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						dialogs.flags.show();
					}
				});
				pop.addSeparator();
				pop.add(mitem = new JMenuItem("Open"));
				mitem.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						open();
					}
				});
				pop.add(mitem = new JMenuItem("Close"));
				mitem.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						close();
					}
				});
				pop.add(mitem = new JMenuItem("Quit"));
				mitem.addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						quit();
					}
				});
			}
			item_idx = 0;
			for (final PropertyDescriptor node_propertie : node_properties)
			{
				if (!nbi.isSupported(node_propertie.getShortDescription()))
					menu_items[item_idx].setEnabled(false);
				else
					menu_items[item_idx].setEnabled(true);
				item_idx++;
			}
			for (final MethodDescriptor node_method : node_methods)
			{
				if (!nbi.isSupported(node_method.getShortDescription()))
					menu_items[item_idx].setEnabled(false);
				else
					menu_items[item_idx].setEnabled(true);
				item_idx++;
			}
			pop.show(curr_tree, e.getX(), e.getY());
		}
		curr_tree.treeDidChange();
		dialogs.update();
	}

	@Override
	public void mouseReleased(MouseEvent e)
	{}

	void open()
	{
		if (open_dialog == null)
		{
			open_dialog = new JDialog(frame);
			open_dialog.setTitle("Open new tree");
			final JPanel mjp = new JPanel();
			mjp.setLayout(new BorderLayout());
			JPanel jp = new JPanel();
			jp.setLayout(new GridLayout(2, 1));
			JPanel jpi = new JPanel();
			jpi.add(new JLabel("Tree: "));
			jpi.add(open_exp = new JTextField(16));
			jp.add(jpi, "East");
			jpi = new JPanel();
			jpi.add(new JLabel("Shot: "));
			jpi.add(open_shot = new JTextField(16));
			jp.add(jpi, "East");
			mjp.add(jp, "North");
			jp = new JPanel();
			jp.setLayout(new GridLayout(1, 3));
			jp.add(open_normal = new JRadioButton("normal"));
			jp.add(open_readonly = new JRadioButton("readonly"));
			jp.add(open_edit = new JRadioButton("edit/new"));
			final ButtonGroup bgMode = new ButtonGroup();
			bgMode.add(open_readonly);
			bgMode.add(open_normal);
			bgMode.add(open_edit);
			mjp.add(jp, "Center");
			jp = new JPanel();
			jp.add(ok_cb = new JButton("Ok"));
			ok_cb.addActionListener(this);
			ok_cb.setSelected(true);
			final JButton cancel = new JButton("Cancel");
			jp.add(cancel);
			cancel.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					open_dialog.setVisible(false);
				}
			});
			mjp.add(jp, "South");
			open_dialog.getContentPane().add(mjp);
			open_shot.addKeyListener(new KeyListener()
			{
				@Override
				public void keyPressed(KeyEvent e)
				{
					if (e.getKeyCode() == KeyEvent.VK_ENTER)
						open_ok();
				}

				@Override
				public void keyReleased(KeyEvent e)
				{}

				@Override
				public void keyTyped(KeyEvent e)
				{}
			});
			open_exp.addKeyListener(new KeyListener()
			{
				@Override
				public void keyPressed(KeyEvent e)
				{
					if (e.getKeyCode() == KeyEvent.VK_ENTER)
						open_ok();
				}

				@Override
				public void keyReleased(KeyEvent e)
				{}

				@Override
				public void keyTyped(KeyEvent e)
				{}
			});
			open_dialog.pack();
			open_dialog.setResizable(false);
			if (curr_experiment != null)
				try
				{
					open_exp.setText(curr_experiment.getName());
					open_shot.setText(new Integer(curr_experiment.getShot()).toString());
				}
				catch (final Exception exc)
				{}
		}
		open_dialog.setLocation(frame.dialogLocation());
		open_normal.setSelected(true);
		open_dialog.setVisible(true);
	}

	public void open(String exp, int shot, boolean editable, boolean readonly, boolean realtime)
	{
		Node top_node = null;
		// first we need to check if the tree is already open
		MDSplus.Tree loop_exp = null;
		for (int i = 0; i < trees.size(); i++)
		{
			loop_exp = experiments.elementAt(i);
			try
			{
				if (loop_exp.getName().equals(exp) && loop_exp.getShot() == shot)
				{
					trees.removeElementAt(i);
					experiments.removeElementAt(i);
					break;
				}
			}
			catch (final Exception exc)
			{}
		}
		curr_experiment = null;
		try
		{
			if (editable)
			{
                            try {
                                    curr_experiment = new MDSplus.Tree(exp, shot, "EDIT");
                            }catch(Exception exc)
                            {
                                    curr_experiment = new MDSplus.Tree(exp, shot, "NEW");
                            }
			}
			else if (readonly)
				curr_experiment = new MDSplus.Tree(exp, shot, "READONLY");
			else
				curr_experiment = new MDSplus.Tree(exp, shot);
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, exc,
					"Error OPENING TREE", JOptionPane.WARNING_MESSAGE);
                        return;
		}
		try
		{
			top_node = new Node(curr_experiment, this);
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(frame, exc.getMessage(), "Error opening " + exp, JOptionPane.ERROR_MESSAGE);
			return;
		}
		top_node.setTreeNode(top = new DefaultMutableTreeNode(top_node));
		try
		{
			top_node.expand();
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error expanding tree", exc);
		}
		final Node members[] = top_node.getMembers();
		for (final Node member : members)
		{
			DefaultMutableTreeNode currNode;
			top.add(currNode = new DefaultMutableTreeNode(member));
			member.setTreeNode(currNode);
		}
		final Node sons[] = top_node.getSons();
		for (final Node son : sons)
		{
			DefaultMutableTreeNode currNode;
			top.add(currNode = new DefaultMutableTreeNode(son));
			son.setTreeNode(currNode);
		}
		curr_tree = new JTree(top);
		Tree.setCurrentNode(0);
//GAB 2014 Add DragAndDrop capability
		curr_tree.setTransferHandler(new FromTransferHandler());
		curr_tree.setDragEnabled(true);
/////////////////////////////
		ToolTipManager.sharedInstance().registerComponent(curr_tree);
		curr_tree.addKeyListener(new KeyAdapter()
		{
			@Override
			public void keyTyped(KeyEvent e)
			{
				if (e.getKeyChar() == KeyEvent.VK_CANCEL) // i.e. Ctrl+C
				{
					TreeNode.copyToClipboard();
					TreeNode.copy();
				}
				else if (e.getKeyChar() == 24) // i.e. Ctrl+X
					TreeNode.cut();
				else if (e.getKeyChar() == 22) // i.e. Ctrl+V
					TreeNode.paste();
				else if (e.getKeyChar() == KeyEvent.VK_DELETE || e.getKeyChar() == KeyEvent.VK_BACK_SPACE)
					TreeNode.delete();
				jTraverser.tree.reportChange();
			}
		});
		if (is_angled_style)
			curr_tree.putClientProperty("JTree.lineStyle", "Angled");
		try
		{
			curr_tree.setEditable(curr_experiment.isOpenForEdit());
		}
		catch (final Exception exc)
		{
			curr_tree.setEditable(false);
		}
		curr_tree.setCellRenderer(new MDSCellRenderer());
		curr_tree.addTreeSelectionListener(this);
		curr_tree.addMouseListener(this);
		curr_tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
		setViewportView(curr_tree);
		trees.push(curr_tree);
		experiments.push(curr_experiment);
		try
		{
			editable = curr_experiment.isOpenForEdit();
		}
		catch (final Exception exc)
		{
			editable = false;
		}
		frame.reportChange(exp, shot, editable, readonly);
	}

	private void open_ok()
	{
		final String exp = open_exp.getText(), shot_t = open_shot.getText();
		if (exp == null || exp.length() == 0)
		{
			JOptionPane.showMessageDialog(open_dialog, "Missing experiment name", "Error opening tree",
					JOptionPane.WARNING_MESSAGE);
			repaint();
			return;
		}
		int shot;
		if (shot_t == null || shot_t.length() == 0)
		{
			JOptionPane.showMessageDialog(open_dialog, "Wrong shot number", "Error opening tree",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		try
		{
			shot = Integer.parseInt(shot_t);
		}
		catch (final Exception e)
		{
			JOptionPane.showMessageDialog(curr_tree, "Wrong shot number", "Error opening tree",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		if (open_edit.isSelected() && open_readonly.isSelected())
		{
			JOptionPane.showMessageDialog(curr_tree, "Tree cannot be open in both edit and readonly mode",
					"Error opening tree", JOptionPane.WARNING_MESSAGE);
			return;
		}
		if (jTraverser.editable != open_edit.isSelected())
			pop = null;
		open(exp.toUpperCase(), shot, open_edit.isSelected(), open_readonly.isSelected(), false);
		open_dialog.setVisible(false);
		dialogs.update();
		frame.pack();
		repaint();
	}

	void quit()
	{
		while (!experiments.empty())
		{
			curr_experiment = experiments.pop();
			try
			{
				curr_experiment.close();
			}
			catch (final Exception exc)
			{
				boolean editable = false;
				String name = null;
				try
				{
					editable = curr_experiment.isOpenForEdit();
					name = curr_experiment.getName();
				}
				catch (final Exception exc2)
				{}
				if (editable)
				{
					final int n = JOptionPane.showConfirmDialog(frame,
							"Tree has been changed: write it before closing?", "Tree " + name + " open in edit mode",
							JOptionPane.YES_NO_OPTION);
					if (n == JOptionPane.YES_OPTION)
					{
						try
						{
							curr_experiment.write();
							curr_experiment.close();
						}
						catch (final Exception exc2)
						{
							jTraverser.stderr("Error closing experiment", exc2);
						}
					}
				}
				else
					jTraverser.stderr("Error closing experiment", exc);
			}
		}
		System.exit(0);
	}

	public void reportChange()
	{
		if (curr_tree != null)
			curr_tree.treeDidChange();
		dialogs.update();
	}

	void setAngled(boolean is_angled)
	{
		is_angled_style = is_angled;
		for (int i = 0; i < trees.size(); i++)
		{
			final JTree curr_tree = trees.elementAt(i);
			if (is_angled_style)
				curr_tree.putClientProperty("JTree.lineStyle", "Angled");
			else
				curr_tree.putClientProperty("JTree.lineStyle", "None");
			curr_tree.treeDidChange();
		}
	}

	@Override
	public void valueChanged(TreeSelectionEvent e)
	{
		final DefaultMutableTreeNode curr_tree_node = (DefaultMutableTreeNode) e.getPath().getLastPathComponent();
		if (curr_tree_node.isLeaf())
		{
			final Node currnode = Tree.getNode(curr_tree_node);
			Tree.setCurrentNode(currnode);
			Node sons[], members[];
			DefaultMutableTreeNode last_node = null;
			try
			{
				curr_node.expand();
			}
			catch (final Exception exc)
			{
				jTraverser.stderr("Error expanding tree", exc);
			}
			sons = currnode.getSons();
			members = currnode.getMembers();
			for (final Node member : members)
			{
				curr_tree_node.add(last_node = new DefaultMutableTreeNode(member));
				member.setTreeNode(last_node);
			}
			for (final Node son : sons)
			{
				curr_tree_node.add(last_node = new DefaultMutableTreeNode(son));
				son.setTreeNode(last_node);
			}
			if (last_node != null)
				curr_tree.expandPath(new TreePath(last_node.getPath()));
		}
	}
}
