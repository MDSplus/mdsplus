//package jTraverser;
import javax.swing.*;
import java.net.URL;
import java.rmi.*;
import java.rmi.RemoteException.*;

public class Node
{
    RemoteTree experiment;
    Data data;
    NodeInfo info;
    NidData nid;
    Node parent;
    Node [] sons;
    Node [] members;
    boolean is_member;
    JLabel tree_label = null;
    NodeBeanInfo bean_info = null;
    Tree hierarchy;
    
    public Node(RemoteTree experiment, Tree hierarchy) throws DatabaseException, RemoteException
    {
	this.experiment = experiment;
	this.hierarchy = hierarchy;
	nid = new NidData(0);
	info = experiment.getInfo(nid, Tree.context);
	parent = null;
	is_member = false;
    }
    
    public Node(RemoteTree experiment, Tree hierarchy, Node parent, boolean is_member, NidData nid)
    {
	this.experiment = experiment;
	this.hierarchy = hierarchy;
	this.nid = nid;
	this.parent = parent;
	try {
	    info = experiment.getInfo(nid, Tree.context);
	}catch(Exception e) {System.out.println("Error getting info " + e);}
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
	    NidData members_nid[] = experiment.getMembers(nid, Tree.context);
	    sons = new Node[sons_nid.length];
	    members = new Node[members_nid.length];
	    for(i = 0; i < sons_nid.length; i++)
	        sons[i] = new Node(experiment, hierarchy, this, false, sons_nid[i]);
	    for(i = 0; i < members_nid.length; i++)
	        members[i] = new Node(experiment, hierarchy, this, true, members_nid[i]);
    }
    
    public void setDefault() throws DatabaseException, RemoteException
    {
	    experiment.setDefault(nid, Tree.context);
    }
    public void toggle() throws DatabaseException, RemoteException
    {
	    if(experiment.isOn(nid, Tree.context))
	        experiment.setOn(nid, false, Tree.context);
	    else
	        experiment.setOn(nid, true, Tree.context);
    }
    
    public void turnOn()
    {
	try {
	    experiment.setOn(nid, true, Tree.context);
	}catch(Exception e) {System.out.println("Error turning on " + e.getMessage());}
    }    
    public void turnOff()
    {
	try {
	    experiment.setOn(nid, false, Tree.context);
	}catch(Exception e) {System.out.println("Error turning on " + e.getMessage());}
    }    
    
    public void doAction() throws DatabaseException, RemoteException
    {
        try {
            experiment.doAction(nid, Tree.context);
        }catch(Exception e) {
		    JOptionPane.showMessageDialog(null, e.getMessage(), 
		        "Error executing message", JOptionPane.WARNING_MESSAGE);
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
    public NodeInfo getInfo()throws DatabaseException, RemoteException
    {
	if(info == null)
	    info = experiment.getInfo(nid, Tree.context);
	return info;
    }
    public void setInfo(NodeInfo info)throws DatabaseException, RemoteException
    {
    }
    public boolean isOn()
    {
	try {
	 return experiment.isOn(nid, Tree.context);
	 }catch (Exception e) {System.out.println("Error checking state "+e);}
	 return false;
    }
    public void setupDevice() 
    {
        ConglomData conglom = null;
        try{
            conglom = (ConglomData)experiment.getData(nid, Tree.context);
        } catch(Exception e) {
 		    JOptionPane.showMessageDialog(FrameRepository.frame, e.getMessage(), 
		        "Error in device setup", JOptionPane.WARNING_MESSAGE);
        }
        if(conglom != null)
        {
            Data model = conglom.getModel();
            if(model != null)
            {
                try
                {
                    String deviceClassName = model.getString()+"Setup";
                    Class deviceClass = Class.forName(deviceClassName);
                    DeviceSetup ds = (DeviceSetup)deviceClass.newInstance();
                    ds.addDataChangeListener(hierarchy);
                    ds.configure(experiment, nid.getInt());
                    if(ds.getContentPane().getLayout() != null)
                        ds.pack();
                    ds.setLocation(hierarchy.getMousePosition());
                    ds.show();
                    return;
                }catch(Exception e)
                {
 		            JOptionPane.showMessageDialog(FrameRepository.frame, e.getMessage(), 
		                "Error in device setup", JOptionPane.WARNING_MESSAGE);
		            return;
                }
            }
        }
 		JOptionPane.showMessageDialog(null, "Missing model in descriptor", 
		        "Error in device setup", JOptionPane.WARNING_MESSAGE);
    }
        
    public boolean isDefault()
    {
	NidData curr_nid = null;
	try {
	    curr_nid = experiment.getDefault(Tree.context);
	    } catch(Exception e) {System.out.println("Error getting default " + e);return false;}
	return curr_nid.datum == nid.datum;
    }
    public int getUsage() {return info.usage; }
    
    public NodeBeanInfo getBeanInfo()
    {
	if(bean_info == null)
	    bean_info = new NodeBeanInfo(experiment, info.usage, info.name);
	return bean_info;
    }
    public String [] getTags() {
        try {
            return experiment.getTags(nid, Tree.context); 
        }catch(Exception exc){return null; }
    }
        
        
    public void setTags(String[] tags) throws DatabaseException, RemoteException
    {
	experiment.setTags(nid, tags, Tree.context);
    }
    public String getFullPath()
    {
    	if(info == null)
	    try {
		info = experiment.getInfo(nid, Tree.context);
	    } catch (Exception e) {System.out.println("Error getting NCI " + e); }
	return info.getFullPath(); 
    }
    public String getName() 
    {
    	if(info == null)
	    try {
		info = experiment.getInfo(nid, Tree.context);
	    } catch (Exception e) {System.out.println("Error getting NCI " + e); }
	return info.getName(); 
    }
    public Node[] getSons() {return sons; }
    public Node[] getMembers() {return members; }
    public Node addNode(int usage, String name) throws DatabaseException, RemoteException
    {
	NidData prev_default = experiment.getDefault(Tree.context), new_nid = null;
	experiment.setDefault(nid, Tree.context);
	try {
	    if(info == null)
		info = experiment.getInfo(nid, Tree.context);
	    new_nid = experiment.addNode(name, usage, Tree.context);
	} finally {
	    experiment.setDefault(prev_default, Tree.context); }
	return new Node(experiment, hierarchy, this, true, new_nid);
    }

    public Node addDevice(String name, String type) throws DatabaseException, RemoteException
    {
	    NidData prev_default = experiment.getDefault(Tree.context), new_nid = null;
	    experiment.setDefault(nid, Tree.context);
	    try {
	        if(info == null)
		    info = experiment.getInfo(nid, Tree.context);
	        new_nid = experiment.addDevice(name, type, Tree.context);
	    } finally {
	        experiment.setDefault(prev_default, Tree.context); 
	    }
	    return new Node(experiment, hierarchy, this, true, new_nid);
    }

	 
	 
    public Node addChild(String name) throws DatabaseException, RemoteException
    {
	NidData prev_default = experiment.getDefault(Tree.context), new_nid;
	experiment.setDefault(nid, Tree.context);
	if(info == null)
	    info = experiment.getInfo(nid, Tree.context);
	new_nid = experiment.addNode(name, NodeInfo.USAGE_STRUCTURE, Tree.context);
	experiment.setDefault(prev_default, Tree.context);
	return new Node(experiment, hierarchy, this, true, new_nid);
    }
	 
    public int startDelete()
    {
	NidData []nids = {nid};
	try {
	    return experiment.startDelete(nids, Tree.context).length;
	}catch(Exception e) {System.out.println("Starting delete: " + e.getMessage());}
	return 0;
    }
    
    public void executeDelete()
    {
	NidData []nids = {nid};
	try {
	    experiment.executeDelete(Tree.context);
	}catch(Exception e) {System.out.println("Error executing delete: " + e.getMessage());}
    }
    
    void rename(String new_name) throws DatabaseException, RemoteException 
    {
        experiment.renameNode(nid, new_name, Tree.context);
	    info = experiment.getInfo(nid, Tree.context);
    }

    void renameLast(String newName)throws Exception
    {
        String prevName = getFullPath();
        int curr;
        for(curr = prevName.length()-1; curr > 0 && prevName.charAt(curr) != ':' && prevName.charAt(curr) != '.'; curr--);
        String newFullName = prevName.substring(0, curr + 1)+ newName;
        rename(newFullName);
    }
        


    private ImageIcon loadIcon(String gifname)
    {
      String base = System.getProperty("icon_base");
      return (base == null) ? new ImageIcon(ClassLoader.getSystemResource(gifname)) : new ImageIcon(base + "/" + gifname);
    }
        
    public JLabel getIcon()
    {
	if(tree_label != null)
	    return tree_label;
	ImageIcon icon = null;
    	switch (info.usage) {
	    case NodeInfo.USAGE_NONE: icon = loadIcon("structure.gif"); break;
	    case NodeInfo.USAGE_ACTION: icon = loadIcon("action.gif");break;
	    case NodeInfo.USAGE_DEVICE: icon = loadIcon("device.gif");break;
	    case NodeInfo.USAGE_DISPATCH: icon = loadIcon("dispatch.gif");break;
	    case NodeInfo.USAGE_ANY:
	    case NodeInfo.USAGE_NUMERIC: icon = loadIcon("numeric.gif");break;
	    case NodeInfo.USAGE_TASK: icon = loadIcon("task.gif");break;
	    case NodeInfo.USAGE_TEXT: icon = loadIcon("text.gif");break;
	    case NodeInfo.USAGE_WINDOW: icon = loadIcon("window.gif");break;
	    case NodeInfo.USAGE_AXIS: icon = loadIcon("axis.gif");break;
	    case NodeInfo.USAGE_SIGNAL: icon = loadIcon("signal.gif");break;
	    case NodeInfo.USAGE_SUBTREE: icon = loadIcon("subtree.gif");break;
	    case NodeInfo.USAGE_COMPOUND_DATA: icon = loadIcon("compound.gif");break;
	}
	
	if(is_member)
	    tree_label = new TreeNode(this, ": " + info.name, icon);
	else
	    tree_label = new TreeNode(this, info.name, icon);
	return tree_label;
    }

    public String toString() {return getName(); }
   
}
    
        
	
	
	    	    