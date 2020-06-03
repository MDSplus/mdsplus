
import java.text.SimpleDateFormat;
import mds.devices.Interface;

public class Database implements Interface{
	static public Setup showSetup(int nid, String model, Tree tree) throws Exception {
		final Class<?> devicesetup = Class.forName("DeviceSetup");
		final Setup setup = (Setup) devicesetup.getMethod("getSetup", int.class) //
				.invoke(null, nid);
		if (setup != null)
			return setup;
		return (Setup) devicesetup.getMethod("newSetup", int.class, String.class, Interface.class, Object.class) //
				.invoke(null, nid, model, new Database(tree), FrameRepository.frame);
	}
	static public void closeSetups() {
		try {
			Class.forName("DeviceSetup").getMethod("closeSetups").invoke(null);
		} catch (Exception e) {}
	}
	static long VMS_OFFSET = 0x7c95674beb4000L;
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

	public void setEditable(boolean editable)
	{
		this.is_editable = editable;
	}

	public void setReadonly(boolean readonly)
	{
		this.is_readonly = readonly;
	}
	@Override
	final public String getName() {return name; }
	@Override
	final public int getShot() {return shot;}
	public boolean isOpen() {return is_open; }
	public boolean isEditable() {return is_editable; }
	public boolean isReadonly() {return is_readonly;}
	/* Low level MDS database management routines, will be  masked by the Node class*/
	public void open() throws Exception{
		if(!is_editable)
		{
			if(is_readonly)
				mdstree = new MDSplus.Tree(name, shot, "READONLY");
			else
				mdstree = new MDSplus.Tree(name, shot);
		}
		else
		{
			try {
				mdstree = new MDSplus.Tree(name, shot, "EDIT");
			} catch(Exception exc)
			{
				mdstree = new MDSplus.Tree(name, shot, "NEW");
			}
		}

	}
	public void write() throws Exception
	{
		mdstree.write();
	}
	public  void close()throws Exception
	{
		mdstree.close();
	}
	public  void quit()throws Exception
	{
		mdstree.quit();
	}
	@Override
	public  String getDataExpr(int nid) throws Exception
	{
		return new MDSplus.TreeNode(nid, mdstree).getData().toString();
	}
	@Override
	public void putDataExpr(int nid, String expr) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		if(expr == null || expr.trim().length() == 0)
			node.deleteData();
		else
		{
			MDSplus.Data data = mdstree.tdiCompile(expr);
			new MDSplus.TreeNode(nid, mdstree).putData(data);
		}
	}
	public  MDSplus.Data getData(int nid) throws Exception
	{
		return new MDSplus.TreeNode(nid, mdstree).getData();
	}
	public void putData(int nid, MDSplus.Data data) throws Exception
	{
		new MDSplus.TreeNode(nid, mdstree).putData(data);
	}
	public NodeInfo getInfo(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);

		long time = node.getTimeInserted();
		time = (time - VMS_OFFSET)/10000L;
		java.util.Calendar cal = java.util.Calendar.getInstance();
		cal.setTimeInMillis(time);
		SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy hh:mm:ss");
		String dateStr = sdf.format(cal);
		return new NodeInfo(node.getDclass(), node.getDtype(), node.getUsage(), node.getFlags(),
				node.getOwnerId(), node.getLength(), node.getConglomerateNodes().size(),
				node.getConglomerateElt(), dateStr, node.getNodeName(),
				node.getFullPath(), node.getMinPath(), node.getPath(), node.getNumSegments());

	}
	public void setTags(int nid, String tags[]) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		for(int i = 0; i < tags.length; i++)
			node.addTag(tags[i]);
	}
	public String[] getTags(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.findTags();
	}
	public void renameNode(int nid, String name) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.rename(name);
	}
	public int addNode(String name, String usage) throws Exception
	{
		return mdstree.addNode(name, usage).getNid();
	}
	public void setSubtree(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.setSubtree(true);
	}

	public void deleteNode(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		mdstree.deleteNode(node.getFullPath());
	}
	public int[] getSons(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		MDSplus.TreeNodeArray sons = node.getChildren();
		int[] sonNids = new int[sons.size()];
		for(int i = 0; i < sons.size(); i++)
			sonNids[i] = sons.getElementAt(i).getNid();
		return sonNids;
	}
	public  int[] getMembers(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		MDSplus.TreeNodeArray members = node.getMembers();
		int[] memberNids = new int[members.size()];
		for(int i = 0; i < members.size(); i++)
			memberNids[i] = members.getElementAt(i).getNid();
		return memberNids;
	}
	@Override
	public boolean isOn(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.isOn();
	}
	@Override
	public void setOn(int nid, boolean on) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.setOn(on);
	}
	@Override
	public int getNode(String path) throws Exception
	{
		return mdstree.getNode(path).getNid();
	}
	@Override
	public void setDefault(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		mdstree.setDefault(node);
	}
	@Override
	public int getDefault() throws Exception
	{
		return mdstree.getDefault().getNid();
	}
	public int addDevice(String path, String model) throws Exception
	{
		mdstree.addDevice(path, model);
		return mdstree.getNode(path).getNid();
	}
	public int doAction(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.doAction();
	}
	@Override
	public void doDeviceMethod(int nid, String method) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.doMethod(method);
	}
	public int [] getWild(int usage_mask) throws Exception
	{
		MDSplus.TreeNodeArray nodes = mdstree.getNodeWild("***", usage_mask);
		int [] nids = new int[nodes.size()];
		for(int i = 0; i < nodes.size(); i++)
			nids[i] = nodes.getElementAt(i).getNid();
		return nids;
	}
	public int [] getWild(int nid, int usage_mask) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		MDSplus.TreeNodeArray nodes = mdstree.getNodeWild(node.getFullPath()+"***", usage_mask);
		int [] nids = new int[nodes.size()];
		for(int i = 0; i < nodes.size(); i++)
			nids[i] = nodes.getElementAt(i).getNid();
		return nids;
	}
	public void clearNciFlag(int nid, int flagMask) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.clearNciFlag(flagMask);

	}
	public void setNciFlag(int nid, int flagMask) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		node.setNciFlag(flagMask);

	}
	public boolean getNciFlag(int nid, int flagMask) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getNciFlag(flagMask);
	}
	public int getNciFlags(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getNciFlags();
	}
	public int getInt(int nid)  throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getInt();
	}
	public float getFloat(int nid)  throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getFloat();
	}
	public int[] getIntArray(int nid)  throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getIntArray();
	}
	public float[] getFloatArray(int nid)  throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getFloatArray();
	}
	@Override
	public String getFullPath(int nid)  throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getFullPath();
	}

	@Override
	public int[] getIntArray(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getIntArray();
	}
	@Override
	public float[] getFloatArray(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getFloatArray();
	}

	@Override
	public double getDouble(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getDouble();
	}

	@Override
	public float getFloat(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getFloat();
	}

	@Override
	public int getInt(String expr) throws Exception
	{
		return mdstree.tdiCompile(expr).getInt();
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
	@Override
	public String execute(String expr) throws Exception
	{
		return mdstree.tdiExecute(expr).toString();
	}
	@Override
	public String getUsage(int nid)
	{
		try {
			MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
			return node.getUsage();
		}catch(Exception exc){return "NONE";}
	}
	@Override
	public int getNumConglomerateNids(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getConglomerateNodes().size();
	}
	@Override
	public String getNodeName(int nid) throws Exception
	{
		MDSplus.TreeNode node = new MDSplus.TreeNode(nid, mdstree);
		return node.getNodeName();

	}
	@Override
	public void dataChanged(int... nids) {
		this.tree.dataChanged();

	}
}
