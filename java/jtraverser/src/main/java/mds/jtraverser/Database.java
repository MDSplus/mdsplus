package mds.jtraverser;
import java.text.SimpleDateFormat;
import mds.devices.Interface;

public class Database implements Interface
{
	static long VMS_OFFSET = 0x7c95674beb4000L;

	static public void closeSetups()
	{
		try
		{
			Class.forName("DeviceSetup").getMethod("closeSetups").invoke(null);
		}
		catch (final Exception e)
		{}
	}

	static public Setup showSetup(int nid, String model, Tree tree) throws Exception
	{
		final boolean readOnly = Tree.curr_experiment.isReadOnly();
		final Class<?> devicesetup = Class.forName("DeviceSetup");
		final Setup setup = (Setup) devicesetup.getMethod("getSetup", int.class, boolean.class) //
				.invoke(null, nid, readOnly);
		if (setup != null)
			return setup;
		return (Setup) devicesetup
				.getMethod("newSetup", int.class, String.class, Interface.class, Object.class, boolean.class) //
				.invoke(null, nid, model, new Database(tree), FrameRepository.frame, readOnly);
	}

	String name;
	int shot;
	boolean is_open = false;
	boolean is_readonly = false;
	boolean is_editable = false;
	Tree tree;
	MDSplus.Tree mdstree;

	public Database(Tree tree)
	{
		this.tree = tree;
		this.mdstree = Tree.curr_experiment;
		this.name = this.mdstree.getName();
		this.shot = this.mdstree.getShot();
	}

	public int addDevice(String path, String model) throws Exception
	{
		mdstree.addDevice(path, model);
		return mdstree.getNode(path).getNid();
	}

	public int addNode(String name, String usage) throws Exception
	{
		return mdstree.addNode(name, usage).getNid();
	}

	public void clearNciFlag(int nid, int flagMask) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.clearNciFlag(flagMask);
	}

	public void close() throws Exception
	{
		mdstree.close();
	}

	@Override
	public void dataChanged(int... nids)
	{
		this.tree.dataChanged();
	}

	public void deleteNode(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		mdstree.deleteNode(node.getFullPath());
	}

	public int doAction(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.doAction();
	}

	@Override
	public void doDeviceMethod(int nid, String method) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.doMethod(method);
	}

	@Override
	public String execute(String expr) throws Exception
	{
		return mdstree.tdiExecute(expr).toString();
	}

	public MDSplus.Data getData(int nid) throws Exception
	{
		return new MDSplus.TreeNode(nid, mdstree).getData();
	}

	@Override
	public String getDataExpr(int nid) throws Exception
	{
		return new MDSplus.TreeNode(nid, mdstree).getData().toString();
	}

	@Override
	public int getDefault() throws Exception
	{ return mdstree.getDefault().getNid(); }

	@Override
	public double getDouble(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getDouble();
	}

	public float getFloat(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getFloat();
	}

	@Override
	public float getFloat(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getFloat();
	}

	public float[] getFloatArray(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getFloatArray();
	}

	@Override
	public float[] getFloatArray(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getFloatArray();
	}

	@Override
	public String getFullPath(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getFullPath();
	}

	public NodeInfo getInfo(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		long time = node.getTimeInserted();
		time = (time - VMS_OFFSET) / 10000L;
		final java.util.Calendar cal = java.util.Calendar.getInstance();
		cal.setTimeInMillis(time);
		final SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy hh:mm:ss");
		final String dateStr = sdf.format(cal);
 		return new NodeInfo(node.getDclass(), node.getDtype(), node.getUsage(), node.getFlags(), node.getOwnerId(),
				node.getLength(), node.getConglomerateNodes().size(), node.getConglomerateElt(), dateStr,
				node.getNodeName(), node.getFullPath(), node.getMinPath(), node.getPath(), node.getNumSegments());
	}

	public int getInt(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getInt();
	}

	@Override
	public int getInt(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getInt();
	}

	public int[] getIntArray(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getIntArray();
	}

	@Override
	public int[] getIntArray(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getIntArray();
	}

	public int[] getMembers(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		final MDSplus.TreeNodeArray members = node.getMembers();
		final int[] memberNids = new int[members.size()];
		for (int i = 0; i < members.size(); i++)
			memberNids[i] = members.getElementAt(i).getNid();
		return memberNids;
	}

	@Override
	final public String getName()
	{ return name; }

	public boolean getNciFlag(int nid, int flagId) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getNciFlag(flagId);
	}

	public int getNciFlags(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getNciFlags();
	}

	@Override
	public int getNode(String path) throws Exception
	{
		return mdstree.getNode(path).getNid();
	}

	@Override
	public String getNodeName(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getNodeName();
	}

	@Override
	public int getNumConglomerateNids(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getConglomerateNodes().size();
	}

	@Override
	final public int getShot()
	{ return shot; }

	public int[] getSons(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		final MDSplus.TreeNodeArray sons = node.getChildren();
		final int[] sonNids = new int[sons.size()];
		for (int i = 0; i < sons.size(); i++)
			sonNids[i] = sons.getElementAt(i).getNid();
		return sonNids;
	}

	@Override
	public String getString(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getString();
	}

	@Override
	public String[] getStringArray(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getStringArray();
	}

	public String[] getTags(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.findTags();
	}

	@Override
	public String getUsage(int nid)
	{
		try
		{
			final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
			return node.getUsage();
		}
		catch (final Exception exc)
		{
			return "NONE";
		}
	}

	public int[] getWild(int usage_mask) throws Exception
	{
		final MDSplus.TreeNodeArray nodes = mdstree.getNodeWild("***", usage_mask);
		final int[] nids = new int[nodes.size()];
		for (int i = 0; i < nodes.size(); i++)
			nids[i] = nodes.getElementAt(i).getNid();
		return nids;
	}

	public int[] getWild(int nid, int usage_mask) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		final MDSplus.TreeNodeArray nodes = mdstree.getNodeWild(node.getFullPath() + "***", usage_mask);
		final int[] nids = new int[nodes.size()];
		for (int i = 0; i < nodes.size(); i++)
			nids[i] = nodes.getElementAt(i).getNid();
		return nids;
	}

	public boolean isEditable()
	{ return is_editable; }

	@Override
	public boolean isOn(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.isOn();
	}

	public boolean isOpen()
	{ return is_open; }

	public boolean isReadonly()
	{ return is_readonly; }

	/*
	 * Low level MDS database management routines, will be masked by the Node class
	 */
	public void open() throws Exception
	{
		if (!is_editable)
		{
			if (is_readonly)
				mdstree = new MDSplus.Tree(name, shot, "READONLY");
			else
				mdstree = new MDSplus.Tree(name, shot);
		}
		else
		{
			try
			{
				mdstree = new MDSplus.Tree(name, shot, "EDIT");
			}
			catch (final Exception exc)
			{
				mdstree = new MDSplus.Tree(name, shot, "NEW");
			}
		}
	}

	public void putData(int nid, MDSplus.Data data) throws Exception
	{
		new MDSplus.TreeNode(nid, mdstree).putData(data);
	}

	@Override
	public void putDataExpr(int nid, String expr) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		if (expr == null || expr.trim().length() == 0)
			node.deleteData();
		else
		{
			final MDSplus.Data data = mdstree.tdiCompile(expr);
			new MDSplus.TreeNode(nid, mdstree).putData(data);
		}
	}

	public void quit() throws Exception
	{
		mdstree.quit();
	}

	public void renameNode(int nid, String name) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.rename(name);
	}

	@Override
	public void setDefault(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		mdstree.setDefault(node);
	}

	public void setEditable(boolean editable)
	{ this.is_editable = editable; }

	public void setNciFlag(int nid, int flagId) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.setNciFlag(flagId);
	}

	@Override
	public void setOn(int nid, boolean on) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.setOn(on);
	}

	public void setReadonly(boolean readonly)
	{ this.is_readonly = readonly; }

	public void setSubtree(int nid) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.setSubtree(true);
	}

	public void setTags(int nid, String tags[]) throws Exception
	{
		final MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		for (int i = 0; i < tags.length; i++)
			node.addTag(tags[i]);
	}

	public void write() throws Exception
	{
		mdstree.write();
	}
}
