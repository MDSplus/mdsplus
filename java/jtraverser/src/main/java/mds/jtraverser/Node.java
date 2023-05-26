package mds.jtraverser;

import java.awt.image.BufferedImage;
import java.awt.image.IndexColorModel;
import java.net.URL;
import java.nio.ByteBuffer;
//package jTraverser;
import java.text.SimpleDateFormat;
import java.util.HashMap;

import javax.swing.*;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;

public class Node
{
	static long VMS_OFFSET = 0x7c95674beb4000L;
	static Node copiedNode;

	public static void copySubtreeContent(Node fromNode, Node toNode)
	{
		try
		{
			fromNode.expand();
			toNode.expand();
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error expanding nodes", exc);
		}
		try
		{
			final MDSplus.Data data = fromNode.getData();
			if (data != null)
			{
				if (!(data instanceof MDSplus.Action))
					toNode.setData(data);
			}
		}
		catch (final Throwable exc)
		{}
		for (int i = 0; i < fromNode.sons.length; i++)
			copySubtreeContent(fromNode.sons[i], toNode.sons[i]);
		for (int i = 0; i < fromNode.members.length; i++)
			copySubtreeContent(fromNode.members[i], toNode.members[i]);
	}

	public static String getUniqueName(String name, String[] usedNames)
	{
		int i;
		for (i = 0; i < usedNames.length && !name.equals(usedNames[i]); i++);
		if (i == usedNames.length)
			return name;
		for (i = name.length() - 1; i > 0 && (name.charAt(i) >= '0' && name.charAt(i) <= '9'); i--);
		name = name.substring(0, i + 1);
		String prevName;
		if (name.length() < 10)
			prevName = name;
		else
			prevName = name.substring(0, 9);
		for (i = 1; i < 1000; i++)
		{
			final String newName = prevName + i;
			int j;
			for (j = 0; j < usedNames.length && !newName.equals(usedNames[j]); j++);
			if (j == usedNames.length)
				return newName;
		}
		return "XXXXXXX"; // Dummy name, hopefully will never reach this
	}

	static public void pasteSubtree(Node fromNode, Node toNode, boolean isMember)
	{
		final DefaultMutableTreeNode savedTreeNode = Tree.getCurrTreeNode();
		try
		{
			fromNode.expand();
			final String[] usedNames = new String[toNode.sons.length + toNode.members.length];
			// collect names used so far
			int idx = 0;
			for (final Node son : toNode.sons)
				usedNames[idx++] = son.getName();
			for (final Node member : toNode.members)
				usedNames[idx++] = member.getName();
			if (fromNode.getUsage().equals("DEVICE"))
			{
				final MDSplus.Conglom conglom = (MDSplus.Conglom) fromNode.getData();
				final Node newNode = Tree.addDevice(
						(isMember ? ":" : ".") + getUniqueName(fromNode.getName(), usedNames),
						conglom.getModel().getString(), toNode);
				newNode.expand();
				copySubtreeContent(fromNode, newNode);
			}
			else
			{
				final Node newNode = Tree.addNode(fromNode.getUsage(),
						(isMember ? ":" : ".") + getUniqueName(fromNode.getName(), usedNames), toNode);
				if (newNode == null)
					return;
				newNode.expand();
				try
				{
					final MDSplus.Data data = fromNode.getData();
					if (data != null && !fromNode.getUsage().equals("ACTION"))
						newNode.setData(data);
				}
				catch (final Exception exc)
				{}
				for (final Node son : fromNode.sons)
				{
					pasteSubtree(son, newNode, false);
				}
				for (final Node member : fromNode.members)
				{
					pasteSubtree(member, newNode, true);
				}
			}
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, "" + exc, "Error copying subtree",
					JOptionPane.WARNING_MESSAGE);
		}
		Tree.setCurrentNode(savedTreeNode);
	}

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

	public Node addChild(String name) throws MDSplus.MdsException
	{
		final MDSplus.TreeNode prev_default = experiment.getDefault();
		MDSplus.TreeNode new_nid;
		experiment.setDefault(nid);
		if (info == null)
			getInfo();
		if (!name.startsWith(":") && !name.startsWith("."))
			name = "." + name;
		new_nid = experiment.addNode(name, "STRUCTURE");
		experiment.setDefault(prev_default);
		return new Node(experiment, hierarchy, this, true, new_nid);
	}

	public Node addDevice(String name, String type) throws MDSplus.MdsException
	{
		final MDSplus.TreeNode prev_default = experiment.getDefault();
		MDSplus.TreeNode new_nid = null;
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
		final Node newNode = new Node(experiment, hierarchy, this, true, new_nid);
		if (name.charAt(0) == '.')
		{
			final Node[] newNodes = new Node[sons.length + 1];
			for (int i = 0; i < sons.length; i++)
				newNodes[i] = sons[i];
			newNodes[sons.length] = newNode;
			sons = newNodes;
		}
		else
		{
			final Node[] newNodes = new Node[members.length + 1];
			for (int i = 0; i < members.length; i++)
				newNodes[i] = members[i];
			newNodes[members.length] = newNode;
			members = newNodes;
		}
		return newNode;
	}

	public Node addNode(String name, String usage) throws MDSplus.MdsException
	{
		final MDSplus.TreeNode prev_default = experiment.getDefault();
		MDSplus.TreeNode new_nid = null;
		experiment.setDefault(nid);
		try
		{
			if (info == null)
				info = getInfo();
			if (usage.equals("STRUCTURE") && !name.startsWith(".") && !name.startsWith(":"))
				name = "." + name;
			new_nid = experiment.addNode(name, usage);
		}
		finally
		{
			experiment.setDefault(prev_default);
		}
		final Node newNode = new Node(experiment, hierarchy, this, true, new_nid);
		if (name.charAt(0) == '.')
		{
			final Node[] newNodes = new Node[sons.length + 1];
			for (int i = 0; i < sons.length; i++)
				newNodes[i] = sons[i];
			newNodes[sons.length] = newNode;
			sons = newNodes;
		}
		else
		{
			final Node[] newNodes = new Node[members.length + 1];
			for (int i = 0; i < members.length; i++)
				newNodes[i] = members[i];
			newNodes[members.length] = newNode;
			members = newNodes;
		}
		return newNode;
	}

	private boolean changePath(Node newParent, String newName)
	{
		if ((newParent == parent) && (newName == getName()))
			return false; // nothing to do
		if (newName.length() > 12 || newName.length() == 0)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, "Node name lengh must be between 1 and 12 characters",
					"Error renaming node: " + newName.length(), JOptionPane.WARNING_MESSAGE);
			return false;
		}
		try
		{
			nid.rename(newName);
			info = getInfo();
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, "Error changing node path: " + exc,
					"Error changing node path", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		if (newParent != parent)
		{
			parent = newParent;
			final DefaultTreeModel tree_model = (DefaultTreeModel) Tree.curr_tree.getModel();
			tree_model.removeNodeFromParent(getTreeNode());
			Tree.addNodeToParent(getTreeNode(), parent.getTreeNode());
		}
		return true;
	}

	void checkInfo()
	{
		if (info == null)
		{
			try
			{
				info = getInfo();
			}
			catch (final Exception exc)
			{
				System.out.println(exc);
			}
		}
	}

	public void clearFlag(byte idx) throws MDSplus.MdsException
	{
		checkInfo();
		nid.clearNciFlag(idx);
		info.setFlags(nid.getNciFlags());
	}

	public void doAction() throws MDSplus.MdsException
	{
		try
		{
			nid.doAction();
		}
		catch (final Exception e)
		{
			JOptionPane.showMessageDialog(null, e.getMessage(), "Error executing message", JOptionPane.WARNING_MESSAGE);
		}
	}

	public void executeDelete()
	{
		try
		{
			experiment.deleteNode(nid.getPath());
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error executing delete", exc);
		}
	}

	public void expand() throws MDSplus.MdsException
	{
		int i;
		final MDSplus.TreeNodeArray sons_nid = nid.getChildren();
		final MDSplus.TreeNodeArray members_nid = nid.getMembers();
		sons = new Node[sons_nid.size()];
		members = new Node[members_nid.size()];
		for (i = 0; i < sons_nid.size(); i++)
			sons[i] = new Node(experiment, hierarchy, this, false, sons_nid.getElementAt(i));
		for (i = 0; i < members_nid.size(); i++)
			members[i] = new Node(experiment, hierarchy, this, true, members_nid.getElementAt(i));
	}

	public NodeBeanInfo getBeanInfo()
	{
		if (bean_info == null)
			bean_info = new NodeBeanInfo(experiment, getUsage(), getName());
		return bean_info;
	}

	public final int getConglomerateElt()
	{
		checkInfo();
		return info.getConglomerateElt();
	}

	public final int getConglomerateNids()
	{
		checkInfo();
		return info.getConglomerateNids();
	}

	public MDSplus.Data getData() throws MDSplus.MdsException
	{
		data = nid.getData();
		return data;
	}

	public final String getDate()
	{
		checkInfo();
		return info.getDate();
	}

	public final String getDClass()
	{
		checkInfo();
		return info.getDClass();
	}

	// info interface
	public final String getDType()
	{
		checkInfo();
		return info.getDType();
	}

	public int getFlags()
	{
		checkInfo();
		try
		{
			info.setFlags(nid.getNciFlags());
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error updating flags", exc);
		}
		return info.getFlags();
	}

	public final String getFullPath()
	{
		checkInfo();
		return info.getFullPath();
	}

	private static ImageIcon ICON_LOADER(String gifname)
	{
		final URL url = Node.class.getClassLoader().getResource("mds/jtraverser/" + gifname + ".gif");
		if (url != null)
			return new ImageIcon(url);
		final ByteBuffer hash = ByteBuffer.allocate(Integer.BYTES);
		hash.putInt(gifname.hashCode());
		final byte r[] = new byte[]
		{ (byte) (hash.get(0) ^ hash.get(1)) };
		final byte g[] = new byte[]
		{ (byte) (hash.get(0) ^ hash.get(2)) };
		final byte b[] = new byte[]
		{ (byte) (hash.get(0) ^ hash.get(3)) };
		final IndexColorModel cm = new IndexColorModel(1, 1, r, g, b);
		return new ImageIcon(new BufferedImage(16, 16, BufferedImage.TYPE_BYTE_BINARY, cm));
	}

	private static final HashMap<String, ImageIcon> ICONS = new HashMap<>();
	static
	{
		final ImageIcon ICON_STRUCTURE = ICON_LOADER("structure");
		final ImageIcon ICON_NUMERIC = ICON_LOADER("numeric");
		ICONS.put("NONE", ICON_STRUCTURE);
		ICONS.put("STRUCTURE", ICON_STRUCTURE);
		ICONS.put("ACTION", ICON_LOADER("action"));
		ICONS.put("DEVICE", ICON_LOADER("device"));
		ICONS.put("DISPATCH", ICON_LOADER("dispatch"));
		ICONS.put("ANY", ICON_LOADER("any"));
		ICONS.put("NUMERIC", ICON_NUMERIC);
		ICONS.put("TASK", ICON_LOADER("task"));
		ICONS.put("TEXT", ICON_LOADER("text"));
		ICONS.put("WINDOW", ICON_LOADER("window"));
		ICONS.put("AXIS", ICON_LOADER("axis"));
		ICONS.put("SIGNAL", ICON_LOADER("signal"));
		ICONS.put("SUBTREE", ICON_LOADER("subtree"));
		ICONS.put("COMPOUND", ICON_LOADER("compound"));
		ICONS.put("WINDOW", ICON_LOADER("window"));
	}

	public JLabel getIcon(boolean isSelected)
	{
		if (info == null)
			checkInfo();
		final String usage = getUsage().trim().toUpperCase();
		ImageIcon icon = ICONS.get(usage);
		if (icon == null)
			icon = ICON_LOADER(usage);
		tree_label = new TreeNode(this, getName(), icon, isSelected);
		return tree_label;
	}

	public NodeInfo getInfo() throws MDSplus.MdsException
	{
		long time = nid.getTimeInserted();
		time = (time - VMS_OFFSET) / 10000L;
		final java.util.Calendar cal = java.util.Calendar.getInstance();
		cal.setTimeInMillis(time);
		final SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy hh:mm:ss");
		final String dateStr = sdf.format(cal.getTime());
 		NodeInfo info =  new NodeInfo(nid.getDclass(), nid.getDtype(), nid.getUsage(), nid.getNciFlags(), nid.getOwnerId(),
				nid.getLength(), nid.getNumElts(), nid.getConglomerateElt(), dateStr,
				nid.getNodeName(), nid.getFullPath(), nid.getMinPath(), nid.getPath(), nid.getNumSegments());
                return info;
	}

	public final int getLength()
	{
		checkInfo();
		return info.getLength();
	}

	public final Node[] getMembers()
	{ return members; }

	public final String getMinPath()
	{
		checkInfo();
		return info.getMinPath();
	}

	public final String getName()
	{
		checkInfo();
		return info.getName();
	}

	public int getNumSegments()
	{
		checkInfo();
		return info.getNumSegments();
	}

	public final int getOwner()
	{
		checkInfo();
		return info.getOwner();
	}

	public final String getPath()
	{
		checkInfo();
		return info.getPath();
	}

	public final Node[] getSons()
	{ return sons; }

	public String[] getTags()
	{
		try
		{
			return nid.findTags();
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	public DefaultMutableTreeNode getTreeNode()
	{ return treenode; }

	public final String getUsage()
	{
		checkInfo();
		return info.getUsage();
	}

	public final boolean isCached()
	{
		checkInfo();
		return info.isCached();
	}

	public final boolean isCompressible()
	{
		checkInfo();
		return info.isCompressible();
	}

	public final boolean isCompressOnPut()
	{
		checkInfo();
		return info.isCompressOnPut();
	}

	public final boolean isCompressSegments()
	{
		checkInfo();
		return info.isCompressSegments();
	}

	public boolean isDefault()
	{
		MDSplus.TreeNode curr_nid = null;
		try
		{
			curr_nid = experiment.getDefault();
		}
		catch (final Exception exc)
		{
			//jTraverser.stderr("Error getting default", exc);
			//exc.printStackTrace();
			return false;
		}
		try
		{
			return curr_nid.getNid() == nid.getNid();
		}
		catch (final Exception exc)
		{
			System.out.println(exc);
			return false;
		}
	}

	public final boolean isDoNotCompress()
	{
		checkInfo();
		return info.isDoNotCompress();
	}

	public final boolean isEssential()
	{
		checkInfo();
		return info.isEssential();
	}

	public final boolean isIncludeInPulse()
	{
		checkInfo();
		return info.isIncludeInPulse();
	}

	public final boolean isNidReference()
	{
		checkInfo();
		return info.isNidReference();
	}

	public final boolean isNoWriteModel()
	{
		checkInfo();
		return info.isNoWriteModel();
	}

	public final boolean isNoWriteShot()
	{
		checkInfo();
		return info.isNoWriteShot();
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
			catch (final Exception exc)
			{
				jTraverser.stderr("Error checking state", exc);
			}
		}
		return is_on;
	}

	public final boolean isParentState()
	{
		checkInfo();
		return info.isParentState();
	}

	public final boolean isPathReference()
	{
		checkInfo();
		return info.isPathReference();
	}

	public final boolean isSegmented()
	{
		checkInfo();
		return info.isSegmented();
	}

	public final boolean isSetup()
	{
		checkInfo();
		return info.isSetup();
	}

	public final boolean isState()
	{
		checkInfo();
		return info.isState();
	}

	public final boolean isVersion()
	{
		checkInfo();
		return info.isVersion();
	}

	public final boolean isWriteOnce()
	{
		checkInfo();
		return info.isWriteOnce();
	}

	boolean move(Node newParent)
	{
		return changePath(newParent, getName());
	}

	boolean rename(String newName)
	{
		return changePath(parent, newName);
	}

	void setAllOnUnchecked()
	{
		Node currNode = this;
		while (currNode.parent != null)
			currNode = currNode.parent;
		currNode.setOnUnchecked();
	}

	public void setData(MDSplus.Data data) throws MDSplus.MdsException
	{
		this.data = data;
		nid.putData(data);
	}

	public void setDefault() throws MDSplus.MdsException
	{
		experiment.setDefault(nid);
	}

	public final void setFlag(byte idx) throws MDSplus.MdsException
	{
		checkInfo();
		nid.setNciFlag(idx);
		info.setFlags(nid.getNciFlags());
	}

	public void setInfo(NodeInfo info) throws MDSplus.MdsException
	{}

	void setOnUnchecked()
	{
		needsOnCheck = true;
		for (final Node son : sons)
			son.setOnUnchecked();
		for (final Node member : members)
			member.setOnUnchecked();
	}

	public void setSubtree() throws MDSplus.MdsException
	{
		nid.setSubtree(true);
		tree_label = null;
	}

	public void setTags(String[] tags) throws MDSplus.MdsException
	{
		final String[] oldTags = nid.findTags();
		for (final String oldTag : oldTags)
			nid.removeTag(oldTag);
		for (final String tag : tags)
			nid.addTag(tag);
	}

	public void setTreeNode(DefaultMutableTreeNode treenode)
	{ this.treenode = treenode; }

	public void setupDevice()
	{
		MDSplus.Conglom conglom = null;
		try
		{
			conglom = (MDSplus.Conglom) nid.getData();
		}
		catch (final Exception e)
		{
			JOptionPane.showMessageDialog(FrameRepository.frame, e.getMessage(), "Error in device setup 1",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		if (conglom == null)
		{
			JOptionPane.showMessageDialog(null, "Missing conglom in head node", "Error in device setup 2",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		final MDSplus.Data model = conglom.getModel();
		if (model == null)
		{
			JOptionPane.showMessageDialog(null, "Missing model in descriptor", "Error in device setup 3",
					JOptionPane.WARNING_MESSAGE);
			return;
		}
		try
		{
			Database.showSetup(nid.getNid(), model.getString(), hierarchy);
		}
		catch (final Exception e)
		{
			/*
			 * try { nid.doMethod("dw_setup") ; } catch(Exception exc)
			 */
			{
				JOptionPane.showMessageDialog(FrameRepository.frame, "Class " + e.getMessage() + " not found",
						"Error in device setup 4", JOptionPane.WARNING_MESSAGE);
				// e.printStackTrace();
				return;
			}
		}
	}

	public int startDelete()
	{
		try
		{
			return experiment.getNodeWild(nid.getPath() + "***").size();
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error starting delete", exc);
		}
		return 0;
	}

	public void toggle() throws MDSplus.MdsException
	{
		if (nid.isOn())
			nid.setOn(false);
		else
			nid.setOn(true);
		setOnUnchecked();
	}

	@Override
	public final String toString()
	{
		return getName();
	}

	public void turnOff()
	{
		try
		{
			nid.setOn(false);
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error turning off", exc);
		}
		setOnUnchecked();
	}

	public void turnOn()
	{
		try
		{
			nid.setOn(true);
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error turning on", exc);
		}
		setOnUnchecked();
	}

	public void updateCell()
	{}

	public void updateData() throws MDSplus.MdsException
	{
		data = nid.getData();
	}
}
