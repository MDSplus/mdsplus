package mds.jtraverser.dialogs;

import javax.swing.JCheckBox;
import javax.swing.tree.DefaultMutableTreeNode;

import mds.MdsException;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;
import mds.jtraverser.Node;
import mds.jtraverser.TreeManager;
import mds.jtraverser.TreeView;

public class SubTreeList extends CheckBoxList
{
	private static final long serialVersionUID = 1L;

	public SubTreeList(final TreeManager treeman)
	{
		super(treeman, "SubTrees include in Pulse");
	}

	@Override
	protected void apply()
	{
		final TreeView tree = SubTreeList.this.treeman.getCurrentTreeView();
		for (int i = 0; i < this.checkboxes.size(); i++)
		{
			final JCheckBox cb = this.checkboxes.getElementAt(i);
			if (cb.isSelected() != ((Boolean) cb.getClientProperty(CheckBoxList.PROP_OLD)).booleanValue())
			{
				final Nid nid = (Nid) cb.getClientProperty(CheckBoxList.PROP_NID);
				final DefaultMutableTreeNode treenode = tree
						.findPath((String) cb.getClientProperty(CheckBoxList.PROP_FULLPATH));
				try
				{
					if (cb.isSelected())
						nid.setFlags(NODE.Flags.INCLUDE_IN_PULSE);
					else
						nid.clearFlags(NODE.Flags.INCLUDE_IN_PULSE);
					if (treenode != null)
						((Node) treenode.getUserObject()).readFlags();
				}
				catch (final MdsException me)
				{
					MdsException.stderr("SubTreeList.apply", me);
				}
			}
		}
		tree.reportChange();
		this.update();
	}

	@Override
	public final void update()
	{
		super.update();
		if (this.treeview.getMds().isLowLatency())
		{
			final int[] nid = new int[this.checkboxes.size()];
			for (int i = 0; i < this.checkboxes.size(); i++)
				nid[i] = ((Nid) this.checkboxes.getElementAt(i).getClientProperty(CheckBoxList.PROP_NID))
						.getNidNumber();
			try
			{
				final int[] flag = this.treeview.getMds().getIntegerArray(this.treeview.getTree().ctx,
						"GETNCI($,'GET_FLAGS')", new Int32Array(nid));
				for (int i = 0; i < this.checkboxes.size(); i++)
					this.checkboxes.getElementAt(i).setSelected((flag[i] & NODE.Flags.INCLUDE_IN_PULSE) != 0);
			}
			catch (final MdsException e)
			{
				MdsException.stderr("SubTreeList.update", e);
			}
		}
		else
			for (int i = 0; i < this.checkboxes.size(); i++)
			{
				final JCheckBox cb = this.checkboxes.getElementAt(i);
				final Nid nid = (Nid) cb.getClientProperty(CheckBoxList.PROP_NID);
				try
				{
					cb.setSelected((nid.getNciFlags() & NODE.Flags.INCLUDE_IN_PULSE) != 0);
				}
				catch (final MdsException e)
				{/**/}
			}
		for (int i = 0; i < this.checkboxes.size(); i++)
			this.checkboxes.getElementAt(i).putClientProperty(CheckBoxList.PROP_OLD,
					Boolean.valueOf(this.checkboxes.getElementAt(i).isSelected()));
		this.repaint();
	}

	@Override
	protected final void updatelist()
	{
		this.updatelist(NODE.USAGE_SUBTREE);
	}
}
