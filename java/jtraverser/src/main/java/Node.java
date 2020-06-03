//package jTraverser;
import java.text.SimpleDateFormat;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;

public class Node
{
	static long VMS_OFFSET = 0x7c95674beb4000L;

	MDSplus.Tree experiment;
	MDSplus.Data data;
	MDSplus.TreeNode nid;
	Node parent;
	Node[] sons;
	Node[] members;
	NodeInfo info;
	boolean is_member;
	JLabel tree_label;
	NodeBeanInfo bean_info;
	Tree hierarchy;
	static Node copiedNode;
	boolean needsOnCheck = true;
	boolean is_on;
	private DefaultMutableTreeNode treenode;

	public Node(MDSplus.Tree experiment, Tree hierarchy) throws MDSplus.MdsException
	{
		this.experiment = experiment;
		this.hierarchy = hierarchy;
		nid = new MDSplus.TreeNode(0, Tree.curr_experiment);
		parent = null;
		is_member = false;
		sons = new Node[0];
		members = new Node[0];
	}

	public Node(MDSplus.Tree experiment, Tree hierarchy, Node parent, boolean is_member, MDSplus.TreeNode nid)
	{
		this.experiment = experiment;
		this.hierarchy = hierarchy;
		this.parent = parent;
		this.is_member = is_member;
		this.nid = nid;
		sons = new Node[0];
		members = new Node[0];
	}

	public void setTreeNode(DefaultMutableTreeNode treenode){this.treenode = treenode;}
	public DefaultMutableTreeNode getTreeNode(){return treenode;}
	void checkInfo()
	{
		if(info == null)
		{
			try {
				info = getInfo();
			}catch(Exception exc){System.out.println(exc);}
		}
	}
	void setOnUnchecked()
	{
		needsOnCheck = true;
		for (int i = 0; i < sons.length; i++)
			sons[i].setOnUnchecked();
		for (int i = 0; i < members.length; i++)
			members[i].setOnUnchecked();
	}
	void setAllOnUnchecked()
	{
		Node currNode = this;
		while(currNode.parent != null)
			currNode = currNode.parent;
		currNode.setOnUnchecked();
	}

	public void setSubtree() throws MDSplus.MdsException
	{
		nid.setSubtree(true);
		tree_label = null;
	}

	public void updateCell()
	{
		;
	}


	public void updateData() throws MDSplus.MdsException
	{
		data = nid.getData();
	}


	public void expand() throws MDSplus.MdsException
	{
		int i;
		MDSplus.TreeNodeArray sons_nid = nid.getChildren();
		MDSplus.TreeNodeArray  members_nid = nid.getMembers();
		sons = new Node[sons_nid.size()];
		members = new Node[members_nid.size()];
		for (i = 0; i < sons_nid.size(); i++)
			sons[i] = new Node(experiment, hierarchy, this, false, sons_nid.getElementAt(i));
		for (i = 0; i < members_nid.size(); i++)
			members[i] = new Node(experiment, hierarchy, this, true,
					members_nid.getElementAt(i));
	}

	public void setDefault() throws MDSplus.MdsException
	{
		experiment.setDefault(nid);
	}

	public void toggle() throws MDSplus.MdsException
	{
		if (nid.isOn())
			nid.setOn(false);
		else
			nid.setOn(true);
		setOnUnchecked();
	}

	public void turnOn()
	{
		try{nid.setOn(true);}
		catch (Exception exc){jTraverser.stderr("Error turning on", exc);}
		setOnUnchecked();
	}

	public void turnOff()
	{
		try{nid.setOn(false);}
		catch (Exception exc){jTraverser.stderr("Error turning off", exc);}
		setOnUnchecked();
	}

	public void doAction() throws MDSplus.MdsException
	{
		try
		{
			nid.doAction();
		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(null, e.getMessage(),
					"Error executing message",
					JOptionPane.WARNING_MESSAGE);
		}
	}

	public void setData(MDSplus.Data data) throws MDSplus.MdsException
	{
		this.data = data;
		nid.putData(data);
	}

	public MDSplus.Data getData() throws MDSplus.MdsException
	{
		data = nid.getData();
		return data;
	}

	public NodeInfo getInfo() throws MDSplus.MdsException
	{
		long time = nid.getTimeInserted();
		time = (time - VMS_OFFSET)/10000L;
		java.util.Calendar cal = java.util.Calendar.getInstance();
		cal.setTimeInMillis(time);
		SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy hh:mm:ss");
		String dateStr = sdf.format(cal.getTime());
		return new NodeInfo(nid.getDclass(), nid.getDtype(), nid.getUsage(), nid.getNciFlags(),
				nid.getOwnerId(), nid.getLength(), nid.getConglomerateNodes().size(),
				nid.getConglomerateElt(), dateStr, nid.getNodeName(),
				nid.getFullPath(), nid.getMinPath(), nid.getPath(), nid.getNumSegments());

	}

	public void setInfo(NodeInfo info) throws MDSplus.MdsException{}

	public final Node[] getSons(){return sons;}
	public final Node[] getMembers(){return members;}
	@Override
	public final String toString(){return getName();}
	// info interface
	public final String getDType(){checkInfo(); return info.getDType();}
	public final String getDClass(){checkInfo(); return info.getDClass();}
	public final String getUsage(){checkInfo();return info.getUsage();}
	public final int getOwner() {checkInfo();return info.getOwner();}
	public final int getLength() {checkInfo();return info.getLength();}
	public final int getConglomerateNids() {checkInfo();return info.getConglomerateNids(); }
	public final int getConglomerateElt() {checkInfo(); return info.getConglomerateElt();}
	public final String getDate(){checkInfo();return info.getDate();}
	public final String getName(){checkInfo();return info.getName();}
	public final String getFullPath(){checkInfo();return info.getFullPath();}
	public final String getMinPath(){checkInfo();return info.getMinPath();}
	public final String getPath(){checkInfo();return info.getPath();}
	public final boolean isState(){checkInfo();return info.isState();}
	public final boolean isParentState(){checkInfo();return info.isParentState();}
	public final boolean isEssential(){checkInfo();return info.isEssential();}
	public final boolean isCached(){checkInfo();return info.isCached();}
	public final boolean isVersion(){checkInfo();return info.isVersion();}
	public final boolean isSegmented(){checkInfo();return info.isSegmented();}
	public final boolean isSetup(){checkInfo();return info.isSetup();}
	public final boolean isWriteOnce(){checkInfo();return info.isWriteOnce();}
	public final boolean isCompressible(){checkInfo();return info.isCompressible();}
	public final boolean isDoNotCompress(){checkInfo();return info.isDoNotCompress();}
	public final boolean isCompressOnPut(){checkInfo();return info.isCompressOnPut();}
	public final boolean isNoWriteModel(){checkInfo();return info.isNoWriteModel();}
	public final boolean isNoWriteShot(){checkInfo();return info.isNoWriteShot();}
	public final boolean isPathReference(){checkInfo();return info.isPathReference();}
	public final boolean isNidReference(){checkInfo();return info.isNidReference();}
	public final boolean isCompressSegments(){checkInfo();return info.isCompressSegments();}
	public final boolean isIncludeInPulse(){checkInfo();return info.isIncludeInPulse();}

	public final void setFlag(byte idx) throws MDSplus.MdsException
	{
		checkInfo();
		nid.setNciFlag(idx);
		info.setFlags(nid.getNciFlags());
	}
	public void clearFlag(byte idx) throws MDSplus.MdsException
	{
		checkInfo();
		nid.clearNciFlag(idx);
		info.setFlags(nid.getNciFlags());
	}
	public int getFlags()

	{
		checkInfo();
		try{info.setFlags(nid.getNciFlags());}
		catch(Exception exc){jTraverser.stderr("Error updating flags",exc);}
		return info.getFlags();
	}

	public boolean isOn()
	{
		if (needsOnCheck)
		{
			needsOnCheck = false;
			try
			{
				is_on = nid.isOn();
			}
			catch (Exception exc){jTraverser.stderr("Error checking state",exc);}
		}
		return is_on;
	}

	public void setupDevice()
	{
		MDSplus.Conglom conglom = null;
		try
		{
			conglom = (MDSplus.Conglom) nid.getData();
		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, e.getMessage(),
					"Error in device setup 1",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		if (conglom == null) {
			JOptionPane.showMessageDialog(null, "Missing conglom in head node",
					"Error in device setup 2",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		MDSplus.Data model = conglom.getModel();
		if (model == null) {
			JOptionPane.showMessageDialog(null, "Missing model in descriptor",
					"Error in device setup 3",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		try
		{
			Database.showSetup(nid.getNid(), model.getString(), hierarchy);
		}
		catch (Exception e)
		{
			/*
			try {
    			nid.doMethod("dw_setup") ;
			}
			catch(Exception exc)
			 */
			{

				JOptionPane.showMessageDialog(FrameRepository.frame,
						"Class "+e.getMessage()+" not found",
						"Error in device setup 4",
						JOptionPane.WARNING_MESSAGE);
				//e.printStackTrace();
				return;
			}
		}
	}

	public boolean isDefault()
	{
		MDSplus.TreeNode curr_nid = null;
		try
		{
			curr_nid = experiment.getDefault();
		}
		catch (Exception exc){
			jTraverser.stderr("Error getting default", exc);
			return false;
		}
		try {
			return curr_nid.getNid() == nid.getNid();
		}catch(Exception exc)
		{
			System.out.println(exc);
			return false;
		}
	}

	public NodeBeanInfo getBeanInfo()
	{
		if (bean_info == null)
			bean_info = new NodeBeanInfo(experiment, getUsage(), getName());
		return bean_info;
	}

	public String[] getTags()
	{
		try
		{
			return nid.findTags();
		}
		catch (Exception exc)
		{
			return null;
		}
	}

	public void setTags(String[] tags) throws MDSplus.MdsException
	{
		String[] oldTags = nid.findTags();
		for(int i = 0; i < oldTags.length; i++)
			nid.removeTag(oldTags[i]);
		for (int i = 0; i < tags.length; i++)
			nid.addTag(tags[i]);
	}

	public Node addNode(String name, String usage) throws MDSplus.MdsException
	{
		MDSplus.TreeNode prev_default = experiment.getDefault(), new_nid = null;
		experiment.setDefault(nid);
		try
		{
			if (info == null)
				info = getInfo();
			if (usage.equals("STRUCTURE") && !name.startsWith(".") &&
					!name.startsWith(":"))
				name = "." + name;
			new_nid = experiment.addNode(name, usage);
		}
		finally
		{
			experiment.setDefault(prev_default);
		}
		Node newNode = new Node(experiment, hierarchy, this, true, new_nid);
		if (name.charAt(0) == '.')
		{
			Node[] newNodes = new Node[sons.length + 1];
			for (int i = 0; i < sons.length; i++)
				newNodes[i] = sons[i];
			newNodes[sons.length] = newNode;
			sons = newNodes;
		}
		else
		{
			Node[] newNodes = new Node[members.length + 1];
			for (int i = 0; i < members.length; i++)
				newNodes[i] = members[i];
			newNodes[members.length] = newNode;
			members = newNodes;
		}
		return newNode;
	}

	public Node addDevice(String name, String type) throws MDSplus.MdsException
	{
		MDSplus.TreeNode prev_default = experiment.getDefault(), new_nid = null;
		experiment.setDefault(nid);
		try
		{
			if (info == null)
				getInfo();
			experiment.addDevice(name, type);
			new_nid = experiment.getNode(name);
		}
		finally
		{
			experiment.setDefault(prev_default);
		}
		Node newNode = new Node(experiment, hierarchy, this, true, new_nid);
		if (name.charAt(0) == '.')
		{
			Node[] newNodes = new Node[sons.length + 1];
			for (int i = 0; i < sons.length; i++)
				newNodes[i] = sons[i];
			newNodes[sons.length] = newNode;
			sons = newNodes;
		}
		else
		{
			Node[] newNodes = new Node[members.length + 1];
			for (int i = 0; i < members.length; i++)
				newNodes[i] = members[i];
			newNodes[members.length] = newNode;
			members = newNodes;
		}
		return newNode;
	}

	public Node addChild(String name) throws MDSplus.MdsException
	{
		MDSplus.TreeNode prev_default = experiment.getDefault(), new_nid;
		experiment.setDefault(nid);
		if (info == null)
			getInfo();
		if (!name.startsWith(":") && !name.startsWith("."))
			name = "." + name;
		new_nid = experiment.addNode(name, "STRUCTURE");
		experiment.setDefault(prev_default);
		return new Node(experiment, hierarchy, this, true, new_nid);
	}

	public int startDelete()
	{
		try
		{
			return experiment.getNodeWild(nid.getPath()+"***").size();
		}
		catch (Exception exc){jTraverser.stderr("Error starting delete", exc);}
		return 0;
	}

	public void executeDelete()
	{
		try {
			experiment.deleteNode(nid.getPath());
		} catch (Exception exc) {
			jTraverser.stderr("Error executing delete", exc);
		}
	}


	boolean move(Node newParent){return changePath(newParent, getName());}
	boolean rename(String newName){return changePath(parent, newName);}
	private boolean changePath(Node newParent, String newName)
	{
		if ((newParent==parent) && (newName==getName())) return false; // nothing to do
		if(newName.length() > 12 || newName.length() == 0)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, "Node name lengh must be between 1 and 12 characters",
					"Error renaming node: "+newName.length(), JOptionPane.WARNING_MESSAGE);
			return false;
		}
		try
		{
			nid.rename(newName);
			info = getInfo();
		}
		catch(Exception exc)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, "Error changing node path: "+ exc,
					"Error changing node path", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		if (newParent!=parent)
		{
			parent = newParent;
			DefaultTreeModel tree_model = (DefaultTreeModel)Tree.curr_tree.getModel();
			tree_model.removeNodeFromParent(getTreeNode());
			Tree.addNodeToParent(getTreeNode(),parent.getTreeNode());
		}
		return true;
	}

	public int getNumSegments() {checkInfo(); return info.getNumSegments();}

	private ImageIcon loadIcon(String gifname)
	{
		String base = System.getProperty("icon_base");
		if (base == null)
			return new ImageIcon(getClass().getClassLoader().getResource(gifname));
		else
			return new ImageIcon(base + "/" + gifname);
	}

	public JLabel getIcon(boolean isSelected)
	{
		if (info == null)
			checkInfo();
		ImageIcon icon = null;
		if(getUsage().equals("NONE")||getUsage().equals("STRUCTURE"))
		{
			icon = loadIcon("structure.gif");
		}
		else if(getUsage().equals("ACTION"))
		{
			icon = loadIcon("action.gif");
		}
		else if(getUsage().equals("DEVICE"))
		{
			icon = loadIcon("device.gif");
		}
		else if(getUsage().equals("DISPATCH"))
		{
			icon = loadIcon("dispatch.gif");
		}
		else if(getUsage().equals("ANY")|| getUsage().equals("NUMERIC"))
		{
			icon = loadIcon("numeric.gif");
		}
		else if(getUsage().equals("TASK"))
		{
			icon = loadIcon("task.gif");
		}
		else if(getUsage().equals("TEXT"))
		{
			icon = loadIcon("text.gif");
		}
		else if(getUsage().equals("WINDOW"))
		{
			icon = loadIcon("window.gif");
		}
		else if(getUsage().equals("AXIS"))
		{
			icon = loadIcon("axis.gif");
		}
		else if(getUsage().equals("SIGNAL"))
		{
			icon = loadIcon("signal.gif");
		}
		else if(getUsage().equals("SUBTREE"))
		{
			icon = loadIcon("subtree.gif");
		}
		else if(getUsage().equals("COMPOUND"))
		{
			icon = loadIcon("compound.gif");
		}
		if(icon == null)
			System.out.println("BUM");
		tree_label = new TreeNode(this, getName(), icon, isSelected);
		if(tree_label == null)
			System.out.println("BAM");
		return tree_label;
	}

	static public void pasteSubtree(Node fromNode, Node toNode, boolean isMember)
	{
		DefaultMutableTreeNode savedTreeNode = Tree.getCurrTreeNode();
		try
		{
			fromNode.expand();
			String[] usedNames = new String[toNode.sons.length +
			                                toNode.members.length];
			//collect names used so far
			int idx = 0;
			for (int i = 0; i < toNode.sons.length; i++)
				usedNames[idx++] = toNode.sons[i].getName();
			for (int i = 0; i < toNode.members.length; i++)
				usedNames[idx++] = toNode.members[i].getName();

			if(fromNode.getUsage().equals("DEVICE"))
			{
				MDSplus.Conglom conglom = (MDSplus.Conglom) fromNode.getData();
				Node newNode = Tree.addDevice( (isMember ? ":" : ".") +
						getUniqueName(fromNode.getName(), usedNames),
						conglom.getModel().getString(),
						toNode);
				newNode.expand();
				copySubtreeContent(fromNode, newNode);
			}
			else
			{
				Node newNode = Tree.addNode(fromNode.getUsage(),
						(isMember ? ":" : ".") +
						getUniqueName(fromNode.getName(), usedNames), toNode);
				if (newNode == null)return;
				newNode.expand();
				try
				{
					MDSplus.Data data = fromNode.getData();
					if (data != null &&
							!fromNode.getUsage().equals("ACTION"))
						newNode.setData(data);
				}
				catch (Exception exc)
				{}
				for (int i = 0; i < fromNode.sons.length; i++)
				{
					pasteSubtree(fromNode.sons[i], newNode, false);
				}
				for (int i = 0; i < fromNode.members.length; i++)
				{
					pasteSubtree(fromNode.members[i], newNode, true);
				}
			}
		}
		catch (Exception exc)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, "" + exc,
					"Error copying subtree",
					JOptionPane.WARNING_MESSAGE);
		}
		Tree.setCurrentNode(savedTreeNode);
	}

	public static void copySubtreeContent(Node fromNode, Node toNode)
	{
		try
		{
			fromNode.expand();
			toNode.expand();
		}
		catch (Exception exc){jTraverser.stderr("Error expanding nodes", exc);}
		try
		{
			MDSplus.Data data = fromNode.getData();
			if (data != null)
			{
				if (! (data instanceof MDSplus.Action))
					toNode.setData(data);
			}
		}
		catch (Throwable exc){}
		for (int i = 0; i < fromNode.sons.length; i++)
			copySubtreeContent(fromNode.sons[i], toNode.sons[i]);
		for (int i = 0; i < fromNode.members.length; i++)
			copySubtreeContent(fromNode.members[i], toNode.members[i]);
	}

	public static String getUniqueName(String name, String[] usedNames)
	{
		int i;
		for (i = 0; i < usedNames.length && !name.equals(usedNames[i]); i++);
		if (i == usedNames.length)return name;
		for (i = name.length() - 1;
				i > 0 && (name.charAt(i) >= '0' && name.charAt(i) <= '9'); i--);
		name = name.substring(0, i + 1);
		String prevName;
		if (name.length() < 10)
			prevName = name;
		else
			prevName = name.substring(0, 9);
		for (i = 1; i < 1000; i++)
		{
			String newName = prevName + i;
			int j;
			for (j = 0; j < usedNames.length && !newName.equals(usedNames[j]);
					j++);
			if (j == usedNames.length)return newName;
		}
		return "XXXXXXX"; //Dummy name, hopefully will never reach this
	}

}

