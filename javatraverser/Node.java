//package jTraverser;
import javax.swing.*;
import java.net.URL;
import java.rmi.*;
import java.rmi.RemoteException.*;
import javax.swing.tree.*;
import java.awt.*;


public class Node
{
    RemoteTree experiment;
    Data data;
    NodeInfo info;
    NidData nid;
    Node parent;
    Node[] sons;
    Node[] members;
    boolean is_member;
    JLabel tree_label;
    NodeBeanInfo bean_info;
    Tree hierarchy;
    static Node copiedNode;
    boolean needsOnCheck = true;
    boolean is_on;
    private DefaultMutableTreeNode treenode;

    public Node(RemoteTree experiment, Tree hierarchy) throws DatabaseException, RemoteException
    {
        this.experiment = experiment;
        this.hierarchy = hierarchy;
        if (experiment.isRealtime())
            nid = new NidData(1);
        else
            nid = new NidData(0);
        info = experiment.getInfo(nid, Tree.context);
        parent = null;
        is_member = false;
        sons = new Node[0];
        members = new Node[0];
    }

    public Node(RemoteTree experiment, Tree hierarchy, Node parent, boolean is_member, NidData nid)
    {
        this.experiment = experiment;
        this.hierarchy = hierarchy;
        this.parent = parent;
        this.is_member = is_member;
        this.nid = nid;
        try{info = experiment.getInfo(nid, Tree.context);}
        catch (Exception exc){jTraverser.stderr("Error getting info", exc);}
        sons = new Node[0];
        members = new Node[0];
    }

    public void setTreeNode(DefaultMutableTreeNode treenode){this.treenode = treenode;}
    public DefaultMutableTreeNode getTreeNode(){return treenode;}

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

    public void setSubtree() throws DatabaseException, RemoteException
    {
        experiment.setSubtree(nid, Tree.context);
        try
        {
            info = experiment.getInfo(nid, Tree.context);
            tree_label = null;
        }
        catch (Exception exc){jTraverser.stderr("Error getting info", exc);}
    }

    public void updateCell()
    {
        ;
    }


    public void updateData() throws DatabaseException, RemoteException
    {
        data = experiment.getData(nid, Tree.context);
    }

    public void updateInfo() throws DatabaseException, RemoteException
    {
        info = experiment.getInfo(nid, Tree.context);
    }

    public void expand() throws DatabaseException, RemoteException
    {
        int i;
        NidData sons_nid[] = experiment.getSons(nid, Tree.context);
        if (sons_nid == null) sons_nid = new NidData[0];
        NidData members_nid[] = experiment.getMembers(nid, Tree.context);
        if (members_nid == null) members_nid = new NidData[0];
        sons = new Node[sons_nid.length];
        members = new Node[members_nid.length];
        for (i = 0; i < sons_nid.length; i++)
            sons[i] = new Node(experiment, hierarchy, this, false, sons_nid[i]);
        for (i = 0; i < members_nid.length; i++)
            members[i] = new Node(experiment, hierarchy, this, true,
                                  members_nid[i]);
    }

    public void setDefault() throws DatabaseException, RemoteException
    {
        experiment.setDefault(nid, Tree.context);
    }

    public void toggle() throws DatabaseException, RemoteException
    {
        if (experiment.isOn(nid, Tree.context))
            experiment.setOn(nid, false, Tree.context);
        else
            experiment.setOn(nid, true, Tree.context);
        setOnUnchecked();
    }

    public void turnOn()
    {
        try{experiment.setOn(nid, true, Tree.context);}
        catch (Exception exc){jTraverser.stderr("Error turning on", exc);}
        setOnUnchecked();
    }

    public void turnOff()
    {
        try{experiment.setOn(nid, false, Tree.context);}
        catch (Exception exc){jTraverser.stderr("Error turning off", exc);}
        setOnUnchecked();
    }

    public void doAction() throws DatabaseException, RemoteException
    {
        try
        {
            experiment.doAction(nid, Tree.context);
        }
        catch (Exception e)
        {
            JOptionPane.showMessageDialog(null, e.getMessage(),
                                          "Error executing message",
                                          JOptionPane.WARNING_MESSAGE);
        }
    }

    public void setData(Data data) throws DatabaseException, RemoteException
    {
        this.data = data;
        experiment.putData(nid, data, Tree.context);
    }

    public Data getData() throws DatabaseException, RemoteException
    {
        data = experiment.getData(nid, Tree.context);
        return data;
    }

    public NodeInfo getInfo() throws DatabaseException, RemoteException
    {
        try{
            info = experiment.getInfo(nid, Tree.context);
        }
        catch (Exception exc){jTraverser.stderr("Error checking info",exc);}
        return info;
    }

    public void setInfo(NodeInfo info) throws DatabaseException, RemoteException{}

    public final Node[] getSons(){return sons;}
    public final Node[] getMembers(){return members;}
    public final String toString(){return getName();}
    // info interface
    public final byte getDType(){return info.getDType();}
    public final byte getDClass(){return info.getDClass();}
    public final byte getUsage(){return info.getUsage();}
    public final int getOwner() {return info.getOwner();}
    public final int getLength() {return info.getLength();}
    public final int getConglomerateNids() {return info.getConglomerateNids(); }
    public final int getConglomerateElt() { return info.getConglomerateElt();}
    public final String getDate(){return info.getDate();}
    public final String getName(){return info.getName();}
    public final String getFullPath(){return info.getFullPath();}
    public final String getMinPath(){return info.getMinPath();}
    public final String getPath(){return info.getPath();}
    public final boolean isState(){return info.isState();}
    public final boolean isParentState(){return info.isParentState();}
    public final boolean isEssential(){return info.isEssential();}
    public final boolean isCached(){return info.isCached();}
    public final boolean isVersion(){return info.isVersion();}
    public final boolean isSegmented(){return info.isSegmented();}
    public final boolean isSetup(){return info.isSetup();}
    public final boolean isWriteOnce(){return info.isWriteOnce();}
    public final boolean isCompressible(){return info.isCompressible();}
    public final boolean isDoNotCompress(){return info.isDoNotCompress();}
    public final boolean isCompressOnPut(){return info.isCompressOnPut();}
    public final boolean isNoWriteModel(){return info.isNoWriteModel();}
    public final boolean isNoWriteShot(){return info.isNoWriteShot();}
    public final boolean isPathReference(){return info.isPathReference();}
    public final boolean isNidReference(){return info.isNidReference();}
    public final boolean isCompressSegments(){return info.isCompressSegments();}
    public final boolean isIncludeInPulse(){return info.isIncludeInPulse();}

    public final void setFlag(byte idx) throws DatabaseException, RemoteException
    {
        experiment.setFlags(nid, 1<<idx);
        info.setFlags(experiment.getFlags(nid));
    }
    public void clearFlag(byte idx) throws DatabaseException, RemoteException
    {
        experiment.clearFlags(nid, 1<<idx);
        info.setFlags(experiment.getFlags(nid));
    }
    public int getFlags()
    {   try{info.setFlags(experiment.getFlags(nid));}
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
                is_on = experiment.isOn(nid, Tree.context);
            }
            catch (Exception exc){jTraverser.stderr("Error checking state",exc);}
        }
        return is_on;
    }

    public void setupDevice()
    {
        ConglomData conglom = null;
        try
        {
            conglom = (ConglomData) experiment.getData(nid, Tree.context);
        }
        catch (Exception e)
        {
            JOptionPane.showMessageDialog(FrameRepository.frame, e.getMessage(),
                                          "Error in device setup 1",
                                          JOptionPane.WARNING_MESSAGE);
        }
        if (conglom != null)
        {
            Data model = conglom.getModel();
            if (model != null)
            {
                try
                {
                    DeviceSetup ds = DeviceSetup.getDevice(nid.getInt());
                    if (ds == null)
                    {
                        String deviceClassName = model.getString() + "Setup";
                        Class deviceClass = Class.forName(deviceClassName);
                        ds = (DeviceSetup) deviceClass.newInstance();
                        Dimension prevDim = ds.getSize();
                        ds.addDataChangeListener(hierarchy);
                        ds.configure(experiment, nid.getInt(), this);
                        if (ds.getContentPane().getLayout() != null)
                            ds.pack();
			Point p = hierarchy.getMousePosition();
			if( p != null )
                            ds.setLocation(p);
                        ds.setSize(prevDim);
                        ds.setVisible(true);
                    }
                    else
                        ds.setVisible(true);
                    return;
                }
                catch (Exception e)
                {                   
             		try {
                		experiment.doDeviceMethod(nid, "dw_setup", Tree.context) ;
            		}catch(Exception exc) {
                   
                     	JOptionPane.showMessageDialog(FrameRepository.frame,
                                                  e.getMessage(),
                                                  "Error in device setup: " + e,
                                                  JOptionPane.WARNING_MESSAGE);
                    	e.printStackTrace();
                    	return;
                	}
				}
            }
        }
        JOptionPane.showMessageDialog(null, "Missing model in descriptor",
                                      "Error in device setup 3",
                                      JOptionPane.WARNING_MESSAGE);
    }

    public boolean isDefault()
    {
        NidData curr_nid = null;
        try
        {
            curr_nid = experiment.getDefault(Tree.context);
        }
        catch (Exception exc){
            jTraverser.stderr("Error getting default", exc);
            return false;
        }
        return curr_nid.datum == nid.datum;
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
            return experiment.getTags(nid, Tree.context);
        }
        catch (Exception exc)
        {
            return null;
        }
    }

    public void setTags(String[] tags) throws DatabaseException, RemoteException
    {
        experiment.setTags(nid, tags, Tree.context);
    }

    public Node addNode(int usage, String name) throws DatabaseException,
        RemoteException
    {
        NidData prev_default = experiment.getDefault(Tree.context), new_nid = null;
        experiment.setDefault(nid, Tree.context);
        try
        {
            if (info == null)
                info = experiment.getInfo(nid, Tree.context);
            if (usage == NodeInfo.USAGE_STRUCTURE && !name.startsWith(".") &&
                !name.startsWith(":"))
                name = "." + name;
            new_nid = experiment.addNode(name, usage, Tree.context);
        }
        finally
        {
            experiment.setDefault(prev_default, Tree.context);
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

    public Node addDevice(String name, String type) throws DatabaseException,
        RemoteException
    {
        NidData prev_default = experiment.getDefault(Tree.context), new_nid = null;
        experiment.setDefault(nid, Tree.context);
        try
        {
            if (info == null)
                info = experiment.getInfo(nid, Tree.context);
            new_nid = experiment.addDevice(name, type, Tree.context);
        }
        finally
        {
            experiment.setDefault(prev_default, Tree.context);
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

    public Node addChild(String name) throws DatabaseException, RemoteException
    {
        NidData prev_default = experiment.getDefault(Tree.context), new_nid;
        experiment.setDefault(nid, Tree.context);
        if (info == null)
            info = experiment.getInfo(nid, Tree.context);
        if (!name.startsWith(":") && !name.startsWith("."))
            name = "." + name;
        new_nid = experiment.addNode(name, NodeInfo.USAGE_STRUCTURE,
                                     Tree.context);
        experiment.setDefault(prev_default, Tree.context);
        return new Node(experiment, hierarchy, this, true, new_nid);
    }

    public int startDelete()
    {
        NidData[] nids ={nid};
        try
        {
            return experiment.startDelete(nids, Tree.context).length;
        }
        catch (Exception exc){jTraverser.stderr("Error starting delete", exc);}
        return 0;
    }

    public void executeDelete()
    {
        NidData[] nids = {nid};
        try
        {
            experiment.executeDelete(Tree.context);
        }
        catch (Exception exc){jTraverser.stderr("Error executing delete", exc);}
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
            String sep = is_member ? ":" : "."; 
            experiment.renameNode(nid, newParent.getFullPath()+sep+newName, Tree.context);
            info = experiment.getInfo(nid, Tree.context);
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
        if (info == null)return null;
        ImageIcon icon = null;
        switch (getUsage())
        {
            case NodeInfo.USAGE_NONE:
                icon = loadIcon("structure.gif");
                break;
            case NodeInfo.USAGE_ACTION:
                icon = loadIcon("action.gif");
                break;
            case NodeInfo.USAGE_DEVICE:
                icon = loadIcon("device.gif");
                break;
            case NodeInfo.USAGE_DISPATCH:
                icon = loadIcon("dispatch.gif");
                break;
            case NodeInfo.USAGE_ANY:
            case NodeInfo.USAGE_NUMERIC:
                icon = loadIcon("numeric.gif");
                break;
            case NodeInfo.USAGE_TASK:
                icon = loadIcon("task.gif");
                break;
            case NodeInfo.USAGE_TEXT:
                icon = loadIcon("text.gif");
                break;
            case NodeInfo.USAGE_WINDOW:
                icon = loadIcon("window.gif");
                break;
            case NodeInfo.USAGE_AXIS:
                icon = loadIcon("axis.gif");
                break;
            case NodeInfo.USAGE_SIGNAL:
                icon = loadIcon("signal.gif");
                break;
            case NodeInfo.USAGE_SUBTREE:
                icon = loadIcon("subtree.gif");
                break;
            case NodeInfo.USAGE_COMPOUND_DATA:
                icon = loadIcon("compound.gif");
                break;
        }
        tree_label = new TreeNode(this, getName(), icon, isSelected);
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

            if (fromNode.getUsage() == NodeInfo.USAGE_DEVICE)
            {
                ConglomData conglom = (ConglomData) fromNode.getData();
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
                    Data data = fromNode.getData();
                    if (data != null &&
                        fromNode.getUsage() != NodeInfo.USAGE_ACTION)
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
            Data data = fromNode.getData();
            if (data != null)
            {
                if (! (data instanceof ActionData))
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

