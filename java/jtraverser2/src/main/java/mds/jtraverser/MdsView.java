package mds.jtraverser;

import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.nio.channels.ReadableByteChannel;
import java.util.Stack;
import java.util.Vector;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.plaf.basic.BasicButtonUI;

import mds.*;
import mds.data.TREE;
import mds.jtraverser.TreeManager.Job;
import mds.mdsip.MdsIp;

public class MdsView extends JTabbedPane implements TransferEventListener
{
	class ClosableTab extends JPanel
	{
		class CloseButton extends JButton implements MouseListener
		{
			private static final long serialVersionUID = 1L;
			final static int size = 8;
			final static int top = 3;
			final static int left = 2;

			public CloseButton()
			{
				this.setPreferredSize(
						new Dimension(CloseButton.size + CloseButton.left, CloseButton.size + CloseButton.top));
				this.setToolTipText("close Tree");
				this.setContentAreaFilled(false);
				this.setBorderPainted(false);
				this.setFocusable(false);
				this.setUI(new BasicButtonUI());
				this.addMouseListener(this);
				this.setRolloverEnabled(true);
			}

			/** when click button, tab will close */
			@Override
			public void mouseClicked(final MouseEvent e)
			{
				final int index = MdsView.this.indexOfTabComponent(ClosableTab.this);
				if (index != -1)
					MdsView.this.closeTree(index, false);
			}

			@Override
			public void mouseEntered(final MouseEvent e)
			{
				this.setForeground(Color.RED);
			}

			@Override
			public void mouseExited(final MouseEvent e)
			{
				this.setForeground(Color.BLACK);
			}

			@Override
			public void mousePressed(final MouseEvent e)
			{
				/**/}

			@Override
			public void mouseReleased(final MouseEvent e)
			{
				/**/}

			@Override
			protected void paintComponent(final Graphics g)
			{
				super.paintComponent(g);
				final Graphics2D g2 = (Graphics2D) g.create();
				if (this.getModel().isPressed())
					g2.translate(1, 1);
				g2.setStroke(new BasicStroke(2));
				g2.setColor(new Color(126, 118, 91));
				if (this.getModel().isRollover())
					g2.setColor(Color.RED);
				g2.drawLine(CloseButton.left + 1, CloseButton.top + 1, this.getWidth() - 2, this.getHeight() - 2);
				g2.drawLine(this.getWidth() - 2, CloseButton.top + 1, CloseButton.left + 1, this.getHeight() - 2);
				g2.dispose();
			}

			@Override
			public void updateUI()
			{
				/**/}
		}

		private static final long serialVersionUID = 1L;

		public ClosableTab()
		{
			this.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
			this.setOpaque(false);
			final JLabel label = new JLabel()
			{
				private static final long serialVersionUID = 1L;

				/** set text for JLabel, it will title of tab */
				@Override
				public String getText()
				{
					final int index = MdsView.this.indexOfTabComponent(ClosableTab.this);
					if (index != -1)
						return MdsView.this.getTitleAt(index);
					return null;
				}
			};
			this.add(label);
			this.add(new CloseButton());
		}
	}

	private static final long serialVersionUID = 1L;
	final TreeManager treeman;
	private final Mds mds;
	private final Stack<TreeView> trees = new Stack<>();
	Vector<Job> change_report_listeners = new Vector<>();

	public MdsView(final TreeManager treeman, final Mds mds)
	{
		this.treeman = treeman;
		this.mds = mds;
		try
		{
			mds.getAPI().setenv("MDSPLUS_DEFAULT_RESAMPLE_MODE", "MinMax");
		}
		catch (final MdsException e)
		{
			MdsException.stderr("MDSPLUS_DEFAULT_RESAMPLE_MODE not set!", e);
		}
		mds.addTransferEventListener(this);
		this.setPreferredSize(new Dimension(300, 400));
		this.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
		this.addChangeListener(new ChangeListener()
		{
			@Override
			public void stateChanged(final ChangeEvent ce)
			{
				@SuppressWarnings("resource")
				final TREE tree = MdsView.this.getCurrentTree();
				if (tree == null)
					return;
				try
				{
					tree.setActive();
				}
				catch (final MdsException e)
				{
					/**/}
				MdsView.this.reportChange();
			}
		});
	}

	public void addChangeReportListener(final Job job)
	{
		this.change_report_listeners.add(job);
	}

	public final MdsView close(final boolean quit)
	{
		while (!this.trees.empty())
			this.trees.pop().close(quit);
		this.mds.close();
		if (this.mds instanceof MdsIp)
			MdsIp.removeSharedConnection((MdsIp) this.mds);
		return this;
	}

	public final void closeTree(final boolean quit)
	{
		final TreeView treeview = this.getCurrentTreeView();
		if (treeview == null)
			return;
		this.closeTree(this.getSelectedIndex(), quit);
	}

	private void closeTree(final int idx, final boolean quit)
	{
		if (idx >= this.getTabCount() || idx < 0)
			return;
		this.trees.remove(this.getTreeAt(idx).close(quit));
		this.removeTabAt(idx);
		if (this.getTabCount() == 0)
			this.treeman.reportChange();
	}

	public void dispatchChangeReportListener()
	{
		for (final Job job : this.change_report_listeners)
			job.program();
	}

	public final Node getCurrentNode()
	{
		final TreeView tree = this.getCurrentTreeView();
		if (tree == null)
			return null;
		return tree.getCurrentNode();
	}

	public final TREE getCurrentTree()
	{
		final TreeView tree = this.getCurrentTreeView();
		if (tree == null)
			return null;
		return tree.getTree();
	}

	public final TreeView getCurrentTreeView()
	{
		if (this.getTabCount() == 0)
			return null;
		return (TreeView) ((JScrollPane) this.getSelectedComponent()).getViewport().getView();
	}

	public final Mds getMds()
	{
		return this.mds;
	}

	private final TreeView getTreeAt(final int index)
	{
		return (TreeView) ((JScrollPane) this.getComponentAt(index)).getViewport().getView();
	}

	@Override
	public void handleTransferEvent(final ReadableByteChannel is, final String info, final int read, final int to_read)
	{
		if (to_read == 0)
		{
			this.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
			this.treeman.setProgress(this, 0, 1);
		}
		else
		{
			this.setCursor(new Cursor(Cursor.WAIT_CURSOR));
			this.treeman.setProgress(this, read, to_read);
		}
	}

	public final void openTree(final String expt, int shot, final TREE.MODE mode)
	{
		// first we need to check if the tree is already open
		if (shot == 0)
			try
			{
				shot = this.mds.getAPI().treeGetCurrentShotId(null, expt);
			}
			catch (final MdsException e)
			{
				/**/}
		int index = -1;
		for (int i = this.getTabCount(); i-- > 0;)
		{
			final TreeView tree = this.getTreeAt(i);
			if (!tree.getExpt().equalsIgnoreCase(expt))
				continue;
			if (tree.getShot() != shot)
				continue;
			tree.close(false);
			this.remove(i);
			index = i;
		}
		if (index < 0)
			index = this.getTabCount();
		TreeView tree;
		try
		{
			tree = new TreeView(this, expt, shot, mode);
		}
		catch (final MdsException e)
		{
			JOptionPane.showMessageDialog(this.treeman, e.getMessage(), "Error opening tree " + expt,
					JOptionPane.ERROR_MESSAGE);
			return;
		}
		tree.addChangeReportListener(new Job()
		{
			@Override
			public void program()
			{
				MdsView.this.reportChange();
			}
		});
		tree.expandRow(0);
		this.add(new JScrollPane(tree), tree.getLabel(), index);
		this.setTabComponentAt(index, new ClosableTab());
		this.setSelectedIndex(index);
	}

	public void removeChangeReportListener(final Job job)
	{
		this.change_report_listeners.remove(job);
	}

	synchronized public final void reportChange()
	{
		this.treeman.reportChange();
	}

	public void set_copy_format(final int format)
	{
		for (final TreeView tree : this.trees)
			tree.set_copy_format(format);
	}

	@Override
	public final String toString()
	{
		return this.mds.toString();
	}
}
