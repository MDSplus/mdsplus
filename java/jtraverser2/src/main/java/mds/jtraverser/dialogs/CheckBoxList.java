package mds.jtraverser.dialogs;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.EmptyBorder;

import mds.MdsException;
import mds.TreeShr.NodeRefStatus;
import mds.data.TREE;
import mds.data.descriptor_a.NidArray;
import mds.data.descriptor_s.Nid;
import mds.jtraverser.TreeManager;
import mds.jtraverser.TreeView;

public abstract class CheckBoxList extends JDialog
{
	public final class CheckBoxListener implements ActionListener
	{
		@Override
		public void actionPerformed(final ActionEvent ce)
		{
			/* stub */}
	}

	public final class JCheckList extends JList<JCheckBox>
	{
		protected class CellRenderer implements ListCellRenderer<JCheckBox>
		{
			@Override
			public Component getListCellRendererComponent(final JList<? extends JCheckBox> list, final JCheckBox value,
					final int index, final boolean isSelected, final boolean cellHasFocus)
			{
				value.setBackground(
						isSelected ? JCheckList.this.getSelectionBackground() : JCheckList.this.getBackground());
				// value.setEnabled(JCheckList.this.isEnabled());
				value.setFont(JCheckList.this.getFont());
				value.setFocusPainted(false);
				value.setBorderPainted(true);
				value.setBorder(isSelected ? UIManager.getBorder("List.focusCellHighlightBorder")
						: new EmptyBorder(1, 1, 1, 1));
				return value;
			}
		}

		private static final long serialVersionUID = 1L;
		private boolean readonly = true;

		public JCheckList(final ListModel<JCheckBox> model)
		{
			this.setCellRenderer(new CellRenderer());
			this.setVisibleRowCount(16);
			this.addMouseListener(new MouseAdapter()
			{
				@Override
				public void mousePressed(final MouseEvent e)
				{
					if (JCheckList.this.readonly)
						return;
					final int index = JCheckList.this.locationToIndex(e.getPoint());
					if (index != -1)
					{
						if ((e.getModifiers() & InputEvent.BUTTON1_MASK) != 0)
						{
							final JCheckBox checkbox = JCheckList.this.getModel().getElementAt(index);
							if (checkbox.isEnabled())
								checkbox.setSelected(!checkbox.isSelected());
						}
						else if ((e.getModifiers() & InputEvent.BUTTON3_MASK) != 0)
							JCheckList.this.setRange(index);
						else
							return;
						JCheckList.this.repaint();
					}
				}
			});
			this.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
			this.setModel(model);
		}

		private void setRange(final int index)
		{
			final int selected = JCheckList.this.getSelectedIndex();
			if (selected < 0)
				return;
			final boolean state = JCheckList.this.getModel().getElementAt(selected).isSelected();
			final int start, stop;
			if (index < selected)
			{
				start = index;
				stop = selected;
			}
			else
			{
				start = selected + 1;
				stop = index + 1;
			}
			for (int i = start; i < stop; i++)
				JCheckList.this.getModel().getElementAt(i).setSelected(state);
		}
	}

	private static final long serialVersionUID = 1L;
	protected static final String PROP_FULLPATH = "fullpath";
	protected static final String PROP_NID = "nid";
	protected static final String PROP_OLD = "old";
	protected final JButton discard_b = new JButton("Discard");
	protected final DefaultListModel<JCheckBox> checkboxes = new DefaultListModel<>();
	protected final JCheckList checklist = new JCheckList(this.checkboxes);
	protected final TreeManager treeman;
	protected final TreeView treeview;
	protected final JButton apply_b = new JButton("Apply");
	protected final JButton update_b = new JButton("Refresh");
	protected int default_nid = -1;

	public CheckBoxList(final TreeManager treeman, final String title)
	{
		super(JOptionPane.getRootFrame(), title);
		this.discard_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent ae)
			{
				CheckBoxList.this.dispose();
			}
		});
		this.update_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent ae)
			{
				CheckBoxList.this.update();
			}
		});
		this.apply_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent ae)
			{
				CheckBoxList.this.apply();
			}
		});
		this.treeman = treeman;
		this.treeview = treeman.getCurrentTreeView();
		final JScrollPane sp = new JScrollPane();
		sp.getVerticalScrollBar().setUnitIncrement(16);
		sp.setViewportView(this.checklist);
		this.add(sp);
		final JPanel buttons = new JPanel(new GridLayout(1, 3));
		buttons.add(this.apply_b);
		buttons.add(this.update_b);
		buttons.add(this.discard_b);
		this.add(buttons, BorderLayout.SOUTH);
		this.pack();
	}

	private void addCheckBox(final Nid nid, final String fullpath)
	{
		final JCheckBox cb = new JCheckBox(fullpath);
		cb.putClientProperty(CheckBoxList.PROP_NID, nid);
		cb.putClientProperty(CheckBoxList.PROP_FULLPATH, fullpath);
		cb.addActionListener(new CheckBoxListener());
		cb.addMouseListener(new MouseAdapter()
		{
			@Override
			public void mouseEntered(final MouseEvent e)
			{
				cb.setText(cb.getToolTipText());
			}

			@Override
			public void mouseExited(final MouseEvent e)
			{
				cb.setText(cb.getName());
			}
		});
		this.checkboxes.addElement(cb);
	}

	protected abstract void apply();

	protected void checkReadOnly()
	{
		final boolean readonly = this.treeview != null && this.treeview.isReadOnly();
		this.checklist.readonly = readonly;
		this.apply_b.setEnabled(!readonly);
	}

	public final void open()
	{
		this.setLocationRelativeTo(JOptionPane.getRootFrame());
		this.checkReadOnly();
		this.setVisible(true);
		new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				CheckBoxList.this.updatelist();
			}
		}).start();
	}

	public void update()
	{
		this.checkReadOnly();
		if (this.default_nid != this.treeview.getDefault().getNidNumber())
			this.updatelist();
	}

	protected abstract void updatelist();

	protected final void updatelist(final byte usage)
	{
		this.default_nid = this.treeview.getDefault().getNidNumber();
		if (this.treeview == null)
			this.checklist.removeAll();
		else
			try
			{
				this.checkboxes.clear();
				this.checklist.readonly = true;
				try (final TREE tree = this.treeview.getTree())
				{
					if (tree.getMds().isLowLatency())
						synchronized (tree.getMds())
						{
							final int usage_mask = 1 << usage;
							tree.holdDbid();
							try
							{
								NodeRefStatus ref = NodeRefStatus.init;
								while ((ref = tree.api.treeFindNodeWild(null, "***", usage_mask, ref)).ok())
								{
									final Nid nid = new Nid(ref.data, tree);
									try
									{
										this.addCheckBox(nid, nid.getNciFullPath());
									}
									catch (final MdsException e)
									{
										System.err.println(nid.decompile());
									}
								}
							}
							finally
							{
								tree.releaseDbid();
							}
						}
					else
					{
						final Nid[] nid = tree.findNodesWild(usage);
						final String[] fp = tree.getMds().getStringArray(tree.ctx, "GETNCI($,'FULLPATH')",
								new NidArray(nid));
						for (int i = 0; i < nid.length; i++)
							this.addCheckBox(nid[i], fp[i]);
					}
				}
			}
			catch (final MdsException e)
			{
				this.checklist.removeAll();
			}
		this.update();
	}
}
