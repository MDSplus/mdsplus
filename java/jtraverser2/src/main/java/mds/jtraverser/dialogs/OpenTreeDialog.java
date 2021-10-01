package mds.jtraverser.dialogs;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor_s.StringDsc;
import mds.jtraverser.*;

public class OpenTreeDialog extends JDialog
{
	private static final long serialVersionUID = 1L;
	private final JRadioButton readonly, edit, normal;
	private final JTextField expt_path = new JTextField(16);
	private final JTextField expt = new JTextField(16);
	JComboBox<String> shot_list = new JComboBox<>();
	private final TreeManager treeman;

	/**
	 * Create the dialog.
	 */
	public OpenTreeDialog(final TreeManager treeman)
	{
		super(JOptionPane.getRootFrame());
		this.setModal(true);
		this.treeman = treeman;
		final KeyAdapter keyadapter = new KeyAdapter()
		{
			@Override
			public void keyPressed(final KeyEvent e)
			{
				if (e.getKeyCode() == KeyEvent.VK_ENTER)
					OpenTreeDialog.this.ok();
			}
		};
		this.setTitle("Open new tree");
		final JPanel mjp = new JPanel();
		mjp.setLayout(new BorderLayout());
		final JPanel grid = new JPanel();
		grid.setBorder(new EmptyBorder(5, 5, 5, 5));
		grid.setLayout(new GridLayout(3, 2)
		{
			private static final long serialVersionUID = 1L;

			@Override
			public void layoutContainer(final Container parent)
			{
				synchronized (parent.getTreeLock())
				{
					final Insets insets = parent.getInsets();
					final int ncomponents = parent.getComponentCount();
					int nrows = this.getRows();
					int ncols = this.getColumns();
					if (ncomponents == 0)
						return;
					if (nrows > 0)
						ncols = (ncomponents + nrows - 1) / nrows;
					else
						nrows = (ncomponents + ncols - 1) / ncols;
					final int hgap = this.getHgap();
					final int vgap = this.getVgap();
					// scaling factors
					final Dimension pd = this.preferredLayoutSize(parent);
					final double sw = (1.0 * parent.getWidth()) / pd.width;
					final double sh = (1.0 * parent.getHeight()) / pd.height;
					// scale
					final int[] w = new int[ncols];
					final int[] h = new int[nrows];
					for (int i = 0; i < ncomponents; i++)
					{
						final int r = i / ncols;
						final int c = i % ncols;
						final Component comp = parent.getComponent(i);
						final Dimension d = comp.getPreferredSize();
						d.width = (int) (sw * d.width);
						d.height = (int) (sh * d.height);
						if (w[c] < d.width)
							w[c] = d.width;
						if (h[r] < d.height)
							h[r] = d.height;
					}
					for (int c = 0, x = insets.left; c < ncols; c++)
					{
						for (int r = 0, y = insets.top; r < nrows; r++)
						{
							final int i = r * ncols + c;
							if (i < ncomponents)
								parent.getComponent(i).setBounds(x, y, w[c], h[r]);
							y += h[r] + vgap;
						}
						x += w[c] + hgap;
					}
				}
			}

			@Override
			public Dimension preferredLayoutSize(final Container parent)
			{
				synchronized (parent.getTreeLock())
				{
					final Insets insets = parent.getInsets();
					final int ncomponents = parent.getComponentCount();
					int nrows = this.getRows();
					int ncols = this.getColumns();
					if (nrows > 0)
						ncols = (ncomponents + nrows - 1) / nrows;
					else
						nrows = (ncomponents + ncols - 1) / ncols;
					final int[] w = new int[ncols];
					final int[] h = new int[nrows];
					for (int i = 0; i < ncomponents; i++)
					{
						final int r = i / ncols;
						final int c = i % ncols;
						final Component comp = parent.getComponent(i);
						final Dimension d = comp.getPreferredSize();
						if (w[c] < d.width)
							w[c] = d.width;
						if (h[r] < d.height)
							h[r] = d.height;
					}
					int nw = 0;
					for (int j = 0; j < ncols; j++)
						nw += w[j];
					int nh = 0;
					for (int i = 0; i < nrows; i++)
						nh += h[i];
					return new Dimension(insets.left + insets.right + nw + (ncols - 1) * this.getHgap(),
							insets.top + insets.bottom + nh + (nrows - 1) * this.getVgap());
				}
			}
		});
		grid.add(new JLabel("path: "));
		grid.add(this.expt_path);
		this.expt_path.addKeyListener(keyadapter);
		grid.add(new JLabel("expt: "));
		grid.add(this.expt);
		this.expt.addKeyListener(keyadapter);
		grid.add(new JLabel("shot: "));
		grid.add(this.shot_list);
		this.shot_list.setEditable(true);
		this.shot_list.addPopupMenuListener(new PopupMenuListener()
		{
			@Override
			public void popupMenuCanceled(final PopupMenuEvent e)
			{
				/* stub */}

			@Override
			public void popupMenuWillBecomeInvisible(final PopupMenuEvent e)
			{
				/* stub */}

			@Override
			public void popupMenuWillBecomeVisible(final PopupMenuEvent e)
			{
				OpenTreeDialog.this.shot_list.removeAllItems();
				OpenTreeDialog.this.shot_list.addItem("model");
				int[] shots;
				final String exp = OpenTreeDialog.this.expt.getText().trim();
				OpenTreeDialog.this.setTreePath(exp);
				if (OpenTreeDialog.this.treeman == null)
					shots = new int[0];
				else
					try
					{
						shots = OpenTreeDialog.this.treeman.getMds().getIntegerArray("getShotDB($)",
								new StringDsc(exp));
					}
					catch (final MdsException exc)
					{
						MdsException.stderr("getShotDB", exc);
						shots = new int[0];
					}
				for (final int shot : shots)
					OpenTreeDialog.this.shot_list.addItem(Integer.toString(shot));
			}
		});
		mjp.add(grid, BorderLayout.NORTH);
		final JPanel access = new JPanel(new GridLayout(1, 3));
		access.add(this.readonly = new JRadioButton("readonly"));
		access.add(this.normal = new JRadioButton("normal"));
		access.add(this.edit = new JRadioButton("edit/new"));
		final ButtonGroup bgMode = new ButtonGroup();
		bgMode.add(this.readonly);
		bgMode.add(this.normal);
		bgMode.add(this.edit);
		this.readonly.addKeyListener(keyadapter);
		this.normal.addKeyListener(keyadapter);
		this.edit.addKeyListener(keyadapter);
		this.readonly.setSelected(true);
		mjp.add(access, "Center");
		final JPanel buttons = new JPanel();
		JButton but;
		buttons.add(but = new JButton("Ok"));
		this.getRootPane().setDefaultButton(but);
		but.setSelected(true);
		but.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				OpenTreeDialog.this.ok();
			}
		});
		buttons.add(but = new JButton("Cancel"));
		but.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				OpenTreeDialog.this.setVisible(false);
			}
		});
		mjp.add(buttons, "South");
		this.getContentPane().add(mjp);
		this.pack();
		this.setResizable(false);
	}

	void ok()
	{
		final String exp = this.expt.getText().trim();
		if (exp == null || exp.length() == 0)
		{
			JOptionPane.showMessageDialog(this, "Missing experiment name", "Error opening tree",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		int shot;
		final String shot_str = (this.shot_list.getSelectedItem() instanceof String)
				? ((String) this.shot_list.getSelectedItem()).trim() : "";
		if (shot_str.length() == 0 || shot_str.trim().equalsIgnoreCase("model"))
			shot = -1;
		else
			try
			{
				shot = Integer.parseInt(shot_str);
			}
			catch (final Exception e)
			{
				JOptionPane.showMessageDialog(this, "Wrong shot number", "Error opening tree",
						JOptionPane.WARNING_MESSAGE);
				return;
			}
		this.setVisible(false);
		final TREE.MODE mode;
		if (this.edit.isSelected())
			mode = TREE.EDITABLE;
		else if (this.readonly.isSelected())
			mode = TREE.READONLY;
		else
			mode = TREE.NORMAL;
		if (this.treeman != null)
		{
			this.setTreePath(exp);
			this.treeman.openTree(exp, shot, mode);
		}
	}

	public final void open()
	{
		this.readonly.setSelected(true);
		TreeView treeview = null;
		if (this.treeman != null)
		{
			Dialogs.setLocation(this);
			final MdsView mdsview = this.treeman.getCurrentMdsView();
			if (mdsview != null)
				treeview = this.treeman.getCurrentTreeView();
		}
		if (treeview != null)
			this.setFields(treeview.getExpt(), treeview.getShot());
		else
		{
			@SuppressWarnings("resource")
			final TREE tree = TREE.getActiveTree();
			if (tree != null)
				this.setFields(tree.expt, tree.shot);
		}
		if (this.expt.getText().length() > 0)
			this.shot_list.grabFocus();
		else
			this.expt.grabFocus();
		this.setVisible(true);
	}

	public final void setFields(final String expt, final int shot)
	{
		this.expt.setText(expt);
		this.shot_list.setSelectedItem(shot == -1 ? "model" : Integer.toString(shot));
	}

	private final void setTreePath(final String exp)
	{
		final String path = this.expt_path.getText().trim();
		if (this.treeman != null)
			this.treeman.setTreePathEnv(exp, path);
	}
}
