package mds.devices;

import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.*;

import mds.Mds;
import mds.MdsException;
import mds.data.TREE;
import mds.data.TREE.NodeInfo;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_s.*;
import mds.data.descriptor_s.NODE.Flags;
import mds.jtraverser.editor.*;
import mds.mdsip.MdsIp;

public class Device implements Interface
{
	public static void main(String... args) throws Exception
	{
		showDeviceSetup(args[0], Integer.valueOf(args[1]), args[2]);
	}

	public static void showDeviceSetup(String experiment, int shot, String path) throws Exception
	{
		final MdsIp mds = new MdsIp();
		try (final TREE tree = new TREE(mds, experiment, shot))
		{
			tree.open(TREE.NORMAL);
			final Nid nid = tree.getNode(path);
			final JDialog dialog = showDialog(null, nid, !tree.is_readonly());
			if (dialog != null)
			{
				dialog.addWindowListener(new WindowAdapter()
				{
					@Override
					public void windowClosed(WindowEvent e)
					{
						synchronized (dialog)
						{
							dialog.notifyAll();
						}
					}
				});
				synchronized (dialog)
				{
					while (dialog.isShowing())
						dialog.wait(1000);
				}
			}
		}
	}

	public static Device getEditor(final Frame frame, final Nid nid, final boolean editable) throws Exception
	{
		return Device.getEditor(frame, nid, editable, getModel(nid));
	}

	public static Device getEditor(final Frame frame, final Nid nid, final boolean editable, String model)
			throws Exception
	{
		Class<?> device_cls;
		try
		{
			device_cls = Class.forName(new StringBuilder().append("mds.devices.").append(model.toString()).toString());
		}
		catch (final ClassNotFoundException e)
		{
			device_cls = Device.class;
		}
		return (Device) device_cls.getConstructor(Frame.class, NODE.class, boolean.class).newInstance(frame, nid,
				Boolean.valueOf(editable));
	}

	private static final String getModel(Nid nid) throws Exception
	{
		final Conglom conglom = (Conglom) nid.getRecord();
		if (conglom == null)
			throw new Exception("Record not a Conglom");
		final StringDsc model = (StringDsc) conglom.getModel().getDataS();
		if (model == null)
			throw new Exception("No model string");
		return model.toString();
	}

	public static JDialog showDialog(final Frame frame, final Nid nid, final boolean editable) throws Exception
	{
		final String model = Device.getModel(nid);
		try
		{
			final int nidnum = nid.getNidNumber();
			final Class<?> devicesetup = Class.forName("DeviceSetup");
			final Setup setup = (Setup) devicesetup.getMethod("getSetup", int.class, boolean.class) //
					.invoke(null, nidnum, !editable);
			if (setup == null)
				return (JDialog) devicesetup
						.getMethod("newSetup", int.class, String.class, Interface.class, Object.class, boolean.class) //
						.invoke(null, nidnum, model, new Device(nid), frame, !editable);
			return null;
		}
		catch (final Exception e)
		{
			return Device.getEditor(frame, nid, editable, model).showDialog();
		}
	}

	protected final NODE<?> head;
	protected final Editor[] edit;
	protected final NODE<?>[] node;
	protected final boolean editable;
	protected JComponent pane;
	protected final Frame frame;
	private final TREE tree;
	private final Mds mds;

	public Device(final Frame frame, final NODE<?> head, final boolean editable)
	{
		this.frame = frame;
		this.pane = new JPanel();
		this.head = head;
		this.editable = editable;
		this.tree = head.getTree();
		this.mds = this.tree.getMds();
		NodeInfo[] node_infos;
		NODE<?>[] nodes;
		try
		{
			final Nid olddefault = tree.getDefaultC();
			this.head.setDefault();
			try
			{
				node_infos = TREE.NodeInfo.getDeviceNodeInfos(head, mds, tree);
			}
			finally
			{
				olddefault.setDefault();
			}
			nodes = new NODE[node_infos.length];
		}
		catch (final MdsException e)
		{
			System.err.println(e);
			nodes = new NODE<?>[]
			{ head };
			node_infos = null;
		}
		this.node = nodes;
		this.edit = new Editor[nodes.length];
		this.pane.setLayout(new GridLayout(0, 1));
		if (node_infos == null)
			return;
		for (int i = 1; i < this.node.length; i++)
		{
			final byte usage = node_infos[i].usage;
			final int flags = node_infos[i].get_flags;
			if (usage == NODE.USAGE_STRUCTURE)
				continue;
			if ((flags & (Flags.WRITE_ONCE | Flags.NO_WRITE_MODEL)) > 0)
				continue;
			if ((flags & (Flags.NO_WRITE_SHOT | Flags.SETUP)) == 0)
				continue;
			String orig_name;
			final String name = node_infos[i].minpath;
			this.node[i] = new Nid(node_infos[i].nid_number, tree);
			this.node[i].setNodeInfoC(node_infos[i]);
			try
			{
				orig_name = this.node[i].getNciOriginalPartName();
			}
			catch (final MdsException e)
			{
				orig_name = name;
			}
			this.addExpr(i, name, orig_name, false, false);
		}
		if (this.pane.getComponentCount() == 0)
		{
			for (int i = 1; i < this.node.length; i++)
			{
				final byte usage = node_infos[i].usage;
				if (usage == NODE.USAGE_STRUCTURE)
					continue;
				String orig_name;
				final String name = node_infos[i].minpath;
				this.node[i] = new Nid(node_infos[i].nid_number, tree);
				this.node[i].setNodeInfoC(node_infos[i]);
				try
				{
					orig_name = this.node[i].getNciOriginalPartName();
				}
				catch (final MdsException e)
				{
					orig_name = name;
				}
				this.addExpr(i, name, orig_name, false, false);
			}
		}
		final JScrollPane sp = new JScrollPane(this.pane);
		sp.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		final int lineheight = this.pane.getComponent(0).getPreferredSize().height;
		final int maxheight = lineheight * 7 + 3;
		sp.getVerticalScrollBar().setUnitIncrement(lineheight);
		sp.setMaximumSize(new Dimension(-1, maxheight));
		final Dimension prefsize = sp.getPreferredSize();
		if (prefsize.height > maxheight)
			prefsize.height = maxheight;
		prefsize.width *= 3;
		sp.setPreferredSize(prefsize);
		this.pane = sp;
	}

	protected Device(final Frame frame, final NODE<?> head, final boolean editable, final int nargs)
	{
		this.frame = frame;
		this.pane = new JPanel();
		this.head = head;
		this.editable = editable;
		this.edit = new Editor[nargs];
		this.node = new NODE[nargs];
		this.tree = head.getTree();
		this.mds = this.tree.getMds();
	}

	public Device(final NODE<?> head)
	{
		this.head = head;
		this.tree = head.getTree();
		this.mds = this.tree.getMds();
		this.edit = null;
		this.node = null;
		this.frame = null;
		this.editable = false;
	}

	protected final void addEnum(final int idx, final String path, final String tooltip, final EnumEditor.MODE mode,
			final String... items)
	{
		try
		{
			if (this.node[idx] == null)
				this.node[idx] = this.head.getNode(path);
			final NodeInfo ni = this.node[idx].getNodeInfoC();
			this.edit[idx] = Editor.addLabel(String.format("%s   [%s]", path, NODE.getUsageStr(ni.usage)),
					new EnumEditor(this.node[idx].getRecord(), this.editable, this.head.getTree(), tooltip, mode,
							items));
			this.pane.add(Editor.addButtons(this.edit[idx], this.node[idx]));
		}
		catch (final MdsException e)
		{
			JOptionPane.showMessageDialog(this.frame, e + "\n" + e.getMessage(), "addEnum " + path,
					JOptionPane.WARNING_MESSAGE);
		}
	}

	protected final void addExpr(final int idx, final String path, final String tooltip,
			final boolean default_to_string, final boolean isField)
	{
		try
		{
			if (this.node[idx] == null)
				this.node[idx] = this.head.getNode(path);
			final NodeInfo ni = this.node[idx].getNodeInfoC();
			this.edit[idx] = Editor.addLabel(String.format("%s   [%s]", path, NODE.getUsageStr(ni.usage)),
					new ExprEditor(this.node[idx].getRecord(), this.editable, this.head.getTree(), tooltip,
							default_to_string, isField));
			this.pane.add(Editor.addButtons(this.edit[idx], this.node[idx]));
		}
		catch (final MdsException e)
		{
			JOptionPane.showMessageDialog(this.frame, e + "\n" + e.getMessage(), "addExpr " + path,
					JOptionPane.WARNING_MESSAGE);
		}
	}

	@Override
	public void dataChanged(int... nids)
	{
		// NOP
	}

	@Override
	public final void doDeviceMethod(int nid, String method) throws Exception
	{
		tree.doDeviceMethod(nid, method);
	}

	@Override
	public String execute(String expr) throws Exception
	{
		return mds.getDescriptor(tree, expr).decompile();
	}

	@Override
	public final String getDataExpr(int nid) throws Exception
	{
		return tree.getRecord(nid).decompile();
	}

	@Override
	public final int getDefault() throws Exception
	{ return tree.getDefault(); }

	@Override
	public double getDouble(String expr) throws Exception
	{
		return mds.getDouble(tree, expr);
	}

	@Override
	public float getFloat(String expr) throws Exception
	{
		return mds.getFloat(tree, expr);
	}

	@Override
	public float[] getFloatArray(String expr) throws Exception
	{
		return mds.getFloatArray(tree, expr);
	}

	@Override
	public final String getFullPath(int nid) throws Exception
	{
		return tree.getNciFullPath(nid);
	}

	@Override
	public int getInt(String expr) throws Exception
	{
		return mds.getInteger(tree, expr);
	}

	@Override
	public int[] getIntArray(String expr) throws Exception
	{
		return mds.getIntegerArray(tree, expr);
	}

	@Override
	public final String getName()
	{ return tree.expt; }

	@Override
	public int getNode(String path) throws Exception
	{
		return tree.getNode(path).getNciNidNumber();
	}

	@Override
	public String getNodeName(int nid) throws Exception
	{
		return tree.getNciNodeName(nid);
	}

	@Override
	public int getNumConglomerateNids(int nid) throws Exception
	{
		return tree.getNciNumberOfElts(nid);
	}

	public JComponent getPane()
	{ return this.pane; }

	@Override
	public final int getShot()
	{ return tree.shot; }

	@Override
	public String getString(String expr) throws Exception
	{
		return mds.getString(tree, expr);
	}

	@Override
	public String[] getStringArray(String expr) throws Exception
	{
		return mds.getStringArray(tree, expr);
	}

	@Override
	public String getUsage(int nid) throws Exception
	{
		return NODE.getUsageStr(tree.getNciUsage(nid));
	}

	@Override
	public boolean isOn(int nid) throws Exception
	{
		return tree.getNciState(nid) == 0;
	}

	@Override
	public void putDataExpr(int nid, String expr) throws Exception
	{
		tree.putRecord(nid, mds.getAPI().tdiCompile(tree, expr).getData());
	}

	@Override
	public final void setDefault(int nid) throws Exception
	{
		tree.setDefault(nid);
	}

	@Override
	public void setOn(int nid, boolean on) throws Exception
	{
		tree.getNode(nid).setOn(on);
	}

	public final JDialog showDialog()
	{
		String path;
		try
		{
			path = this.head.getNciPath();
		}
		catch (final MdsException e)
		{
			path = this.head.toString();
		}
		final String title = new StringBuilder().append(this.getClass().getSimpleName()).append(": ").append(path)
				.toString();
		final JDialog dialog = new JDialog(this.frame, title);
		dialog.add(this.pane);
		dialog.pack();
		dialog.setVisible(true);
		return dialog;
	}
}
