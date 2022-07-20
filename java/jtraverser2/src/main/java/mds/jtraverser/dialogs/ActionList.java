package mds.jtraverser.dialogs;

import java.awt.Color;

import javax.swing.JCheckBox;
import javax.swing.tree.DefaultMutableTreeNode;

import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;
import mds.jtraverser.Node;
import mds.jtraverser.TreeManager;

public class ActionList extends CheckBoxList
{
	private static final long serialVersionUID = 1L;

	public ActionList(final TreeManager treeman)
	{
		super(treeman, "Action Nodes in Tree");
	}

	@Override
	protected void apply()
	{
		for (int i = 0; i < this.checkboxes.size(); i++)
		{
			final JCheckBox cb = this.checkboxes.getElementAt(i);
			if (cb.isSelected() != ((Boolean) cb.getClientProperty(CheckBoxList.PROP_OLD)).booleanValue())
			{
				final Nid nid = (Nid) cb.getClientProperty(CheckBoxList.PROP_NID);
				final DefaultMutableTreeNode treenode = this.treeview
						.findPath((String) cb.getClientProperty(CheckBoxList.PROP_FULLPATH));
				try
				{
					nid.setOn(cb.isSelected());
					if (treenode != null)
						((Node) treenode.getUserObject()).setOnUnchecked();
				}
				catch (final MdsException me)
				{
					MdsException.stderr("ActionList.apply", me);
				}
			}
		}
		this.treeview.reportChange();
		this.update();
	}

	@Override
	public final void update()
	{
		super.update();
		if (this.checkboxes.size() > 0)
		{
			final int[] nid = new int[this.checkboxes.size()];
			for (int i = 0; i < this.checkboxes.size(); i++)
				nid[i] = ((Nid) this.checkboxes.getElementAt(i).getClientProperty(CheckBoxList.PROP_NID))
						.getNidNumber();
			byte[] isoff;
			int[] status;
			try
			{
				try (final TREE tree = this.treeview.getTree())
				{
					isoff = tree.getMds().getByteArray(this.treeview.getTree().ctx,
							"_n=$;OR(GETNCI(_n,'PARENT_STATE'),GETNCI(_n,'STATE'))", new Int32Array(nid));
					status = tree.getMds().getIntegerArray(this.treeview.getTree().ctx, "GETNCI($,'STATUS')",
							new Int32Array(nid));
					for (int i = 0; i < this.checkboxes.size(); i++)
					{
						final JCheckBox cb = this.checkboxes.getElementAt(i);
						cb.setSelected(isoff[i] == 0);
						cb.putClientProperty(CheckBoxList.PROP_OLD, Boolean.valueOf(isoff[i] == 0));
						if (this.treeview.isModel())
							continue;
						cb.setToolTipText(Action.getStatusMsg(status[i]));
						cb.setForeground((status[i] & 1) == 0 ? Color.RED : Color.BLACK);
					}
				}
			}
			catch (final MdsException e)
			{
				MdsException.stderr("ActionList.update", e);
			}
		}
		this.repaint();
	}

	@Override
	protected final void updatelist()
	{
		this.updatelist(NODE.USAGE_ACTION);
	}
}
