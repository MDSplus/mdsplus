
package jTraverser;
import javax.swing.*;

public class Node
{
    Database tree;
    Data data;
    NodeInfo info;
    NidData nid;
    Node parent;
    Node [] sons;
    Node [] members;
    boolean is_member;
    JLabel tree_label = null;
    NodeBeanInfo bean_info = null;
    
    public Node(Database tree) throws DatabaseException
    {
	this.tree = tree;
	nid = new NidData(0);
	info = tree.getInfo(nid);
	parent = null;
	is_member = false;
    }
    
    public Node(Database tree, Node parent, boolean is_member, NidData nid)
    {
	this.tree = tree;
	this.nid = nid;
	this.parent = parent;
	try {
	    info = tree.getInfo(nid);
	}catch(Exception e) {System.out.println("Error getting info " + e);}
    }
    
    public void updateData() throws DatabaseException
    {
	data = tree.getData(nid);
    }
    
    public void updateInfo() throws DatabaseException
    {
	info = tree.getInfo(nid);
    }
    
    public void expand() throws DatabaseException
    {
	int i;
	NidData sons_nid[] = tree.getSons(nid);
	NidData members_nid[] = tree.getMembers(nid);
	sons = new Node[sons_nid.length];
	members = new Node[members_nid.length];
	for(i = 0; i < sons_nid.length; i++)
	    sons[i] = new Node(tree, this, false, sons_nid[i]);
	for(i = 0; i < members_nid.length; i++)
	    members[i] = new Node(tree, this, true, members_nid[i]);
    }
    
    public void setDefault() throws DatabaseException
    {
	tree.setDefault(nid);
    }
    public void toggle() throws DatabaseException
    {
	if(tree.isOn(nid))
	    tree.setOn(nid, false);
	else
	    tree.setOn(nid, true);
    }
    
    public void turnOn()
    {
	try {
	    tree.setOn(nid, true);
	}catch(Exception e) {System.out.println("Error turning on " + e.getMessage());}
    }    
    public void turnOff()
    {
	try {
	    tree.setOn(nid, false);
	}catch(Exception e) {System.out.println("Error turning on " + e.getMessage());}
    }    
    
    public void doAction() throws DatabaseException
    {}
    
    public void setData(Data data) throws DatabaseException 
    {
	this.data = data;
	tree.putData(nid, data);
    }
    public Data getData() throws DatabaseException
    {
	data = tree.getData(nid);
	return data;
    }
    public NodeInfo getInfo()throws DatabaseException
    {
	if(info == null)
	    info = tree.getInfo(nid);
	return info;
    }
    public void setInfo(NodeInfo info)throws DatabaseException
    {
    }
    public boolean isOn()
    {
	try {
	 return tree.isOn(nid);
	 }catch (Exception e) {System.out.println("Error checking state "+e);}
	 return false;
    }
    public void setupDevice() {}
        
    public boolean isDefault()
    {
	NidData curr_nid = null;
	try {
	    curr_nid = tree.getDefault();
	    } catch(Exception e) {System.out.println("Error getting default " + e);return false;}
	return curr_nid.datum == nid.datum;
    }
    public int getUsage() {return info.usage; }
    
    public NodeBeanInfo getBeanInfo()
    {
	if(bean_info == null)
	    bean_info = new NodeBeanInfo(tree, info.usage, info.name);
	return bean_info;
    }
    public String [] getTags() {return tree.getTags(nid); }
    public void setTags(String[] tags) throws DatabaseException
    {
	tree.setTags(nid, tags);
    }
    public String getFullPath()
    {
    	if(info == null)
	    try {
		info = tree.getInfo(nid);
	    } catch (Exception e) {System.out.println("Error getting NCI " + e); }
	return info.getFullPath(); 
    }
    public String getName() 
    {
    	if(info == null)
	    try {
		info = tree.getInfo(nid);
	    } catch (Exception e) {System.out.println("Error getting NCI " + e); }
	return info.getName(); 
    }
    public Node[] getSons() {return sons; }
    public Node[] getMembers() {return members; }
    public Node addNode(int usage, String name) throws DatabaseException
    {
	NidData prev_default = tree.getDefault(), new_nid = null;
	tree.setDefault(nid);
	try {
	    if(info == null)
		info = tree.getInfo(nid);
	    new_nid = tree.addNode(name, usage);
	} finally {
	    tree.setDefault(prev_default); }
	return new Node(tree, this, true, new_nid);
    }
	 
    public Node addChild(String name) throws DatabaseException
    {
	NidData prev_default = tree.getDefault(), new_nid;
	tree.setDefault(nid);
	if(info == null)
	    info = tree.getInfo(nid);
	new_nid = tree.addNode(name, NodeInfo.USAGE_STRUCTURE);
	tree.setDefault(prev_default);
	return new Node(tree, this, true, new_nid);
    }
	 
    public int startDelete()
    {
	NidData []nids = {nid};
	try {
	    return tree.startDelete(nids).length;
	}catch(Exception e) {System.out.println("Starting delete: " + e.getMessage());}
	return 0;
    }
    
    public void executeDelete()
    {
	NidData []nids = {nid};
	try {
	    tree.executeDelete();
	}catch(Exception e) {System.out.println("Error executing delete: " + e.getMessage());}
    }
    
    void rename(String new_name) throws DatabaseException 
    {
        tree.renameNode(nid, new_name);
	info = tree.getInfo(nid);
    }


    public JLabel getIcon()
    {
	if(tree_label != null)
	    return tree_label;
    
	ImageIcon icon = null;
	String base = System.getProperty("icon_base");
    	switch (info.usage) {
	    case NodeInfo.USAGE_NONE: icon = new ImageIcon(base + "/structure.gif");break;
	    case NodeInfo.USAGE_ACTION: icon = new ImageIcon(base + "/action.gif");break;
	    case NodeInfo.USAGE_DEVICE: icon = new ImageIcon(base + "/device.gif");break;
	    case NodeInfo.USAGE_DISPATCH: icon = new ImageIcon(base + "/dispatch.gif");break;
	    case NodeInfo.USAGE_ANY:
	    case NodeInfo.USAGE_NUMERIC: icon = new ImageIcon(base + "/numeric.gif");break;
	    case NodeInfo.USAGE_TASK: icon = new ImageIcon(base + "/task.gif");break;
	    case NodeInfo.USAGE_TEXT: icon = new ImageIcon(base + "/text.gif");break;
	    case NodeInfo.USAGE_WINDOW: icon = new ImageIcon(base + "/window.gif");break;
	    case NodeInfo.USAGE_AXIS: icon = new ImageIcon(base + "/axis.gif");break;
	    case NodeInfo.USAGE_SIGNAL: icon = new ImageIcon(base + "/signal.gif");break;
	    case NodeInfo.USAGE_SUBTREE: icon = new ImageIcon(base + "/subtree.gif");break;
	    case NodeInfo.USAGE_COMPOUND_DATA: icon = new ImageIcon(base + "/compound.gif");break;
	}
	
	if(is_member)
	    tree_label = new TreeNode(this, ": " + info.name, icon);
	else
	    tree_label = new TreeNode(this, info.name, icon);
	return tree_label;
    }
    public String toString() {return getFullPath(); }
   
}
    
        
	
	
	    	    