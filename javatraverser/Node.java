//package jTraverser;
import javax.swing.*;
import java.net.URL;

public class Node
{
    Database experiment;
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
    
    public Node(Database experiment, Tree hierarchy) throws DatabaseException
    {
	this.experiment = experiment;
	this.hierarchy = hierarchy;
	nid = new NidData(0);
	info = experiment.getInfo(nid);
	parent = null;
	is_member = false;
    }
    
    public Node(Database experiment, Tree hierarchy, Node parent, boolean is_member, NidData nid)
    {
	this.experiment = experiment;
	this.hierarchy = hierarchy;
	this.nid = nid;
	this.parent = parent;
	try {
	    info = experiment.getInfo(nid);
	}catch(Exception e) {System.out.println("Error getting info " + e);}
    }
    
    public void updateData() throws DatabaseException
    {
	data = experiment.getData(nid);
    }
    
    public void updateInfo() throws DatabaseException
    {
	info = experiment.getInfo(nid);
    }
    
    public void expand() throws DatabaseException
    {
	int i;
	NidData sons_nid[] = experiment.getSons(nid);
	NidData members_nid[] = experiment.getMembers(nid);
	sons = new Node[sons_nid.length];
	members = new Node[members_nid.length];
	for(i = 0; i < sons_nid.length; i++)
	    sons[i] = new Node(experiment, hierarchy, this, false, sons_nid[i]);
	for(i = 0; i < members_nid.length; i++)
	    members[i] = new Node(experiment, hierarchy, this, true, members_nid[i]);
    }
    
    public void setDefault() throws DatabaseException
    {
	experiment.setDefault(nid);
    }
    public void toggle() throws DatabaseException
    {
	if(experiment.isOn(nid))
	    experiment.setOn(nid, false);
	else
	    experiment.setOn(nid, true);
    }
    
    public void turnOn()
    {
	try {
	    experiment.setOn(nid, true);
	}catch(Exception e) {System.out.println("Error turning on " + e.getMessage());}
    }    
    public void turnOff()
    {
	try {
	    experiment.setOn(nid, false);
	}catch(Exception e) {System.out.println("Error turning on " + e.getMessage());}
    }    
    
    public void doAction() throws DatabaseException
    {
        try {
            experiment.doAction(nid);
        }catch(Exception e) {
		    JOptionPane.showMessageDialog(null, e.getMessage(), 
		        "Error executing message", JOptionPane.WARNING_MESSAGE);
		}
        
    }
    
    public void setData(Data data) throws DatabaseException 
    {
	this.data = data;
	experiment.putData(nid, data);
    }
    public Data getData() throws DatabaseException
    {
	data = experiment.getData(nid);
	return data;
    }
    public NodeInfo getInfo()throws DatabaseException
    {
	if(info == null)
	    info = experiment.getInfo(nid);
	return info;
    }
    public void setInfo(NodeInfo info)throws DatabaseException
    {
    }
    public boolean isOn()
    {
	try {
	 return experiment.isOn(nid);
	 }catch (Exception e) {System.out.println("Error checking state "+e);}
	 return false;
    }
    public void setupDevice() 
    {
        ConglomData conglom = null;
        try{
            conglom = (ConglomData)experiment.getData(nid);
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
	    curr_nid = experiment.getDefault();
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
    public String [] getTags() {return experiment.getTags(nid); }
    public void setTags(String[] tags) throws DatabaseException
    {
	experiment.setTags(nid, tags);
    }
    public String getFullPath()
    {
    	if(info == null)
	    try {
		info = experiment.getInfo(nid);
	    } catch (Exception e) {System.out.println("Error getting NCI " + e); }
	return info.getFullPath(); 
    }
    public String getName() 
    {
    	if(info == null)
	    try {
		info = experiment.getInfo(nid);
	    } catch (Exception e) {System.out.println("Error getting NCI " + e); }
	return info.getName(); 
    }
    public Node[] getSons() {return sons; }
    public Node[] getMembers() {return members; }
    public Node addNode(int usage, String name) throws DatabaseException
    {
	NidData prev_default = experiment.getDefault(), new_nid = null;
	experiment.setDefault(nid);
	try {
	    if(info == null)
		info = experiment.getInfo(nid);
	    new_nid = experiment.addNode(name, usage);
	} finally {
	    experiment.setDefault(prev_default); }
	return new Node(experiment, hierarchy, this, true, new_nid);
    }

    public Node addDevice(String name, String type) throws DatabaseException
    {
	    NidData prev_default = experiment.getDefault(), new_nid = null;
	    experiment.setDefault(nid);
	    try {
	        if(info == null)
		    info = experiment.getInfo(nid);
	        new_nid = experiment.addDevice(name, type);
	    } finally {
	        experiment.setDefault(prev_default); 
	    }
	    return new Node(experiment, hierarchy, this, true, new_nid);
    }

	 
	 
    public Node addChild(String name) throws DatabaseException
    {
	NidData prev_default = experiment.getDefault(), new_nid;
	experiment.setDefault(nid);
	if(info == null)
	    info = experiment.getInfo(nid);
	new_nid = experiment.addNode(name, NodeInfo.USAGE_STRUCTURE);
	experiment.setDefault(prev_default);
	return new Node(experiment, hierarchy, this, true, new_nid);
    }
	 
    public int startDelete()
    {
	NidData []nids = {nid};
	try {
	    return experiment.startDelete(nids).length;
	}catch(Exception e) {System.out.println("Starting delete: " + e.getMessage());}
	return 0;
    }
    
    public void executeDelete()
    {
	NidData []nids = {nid};
	try {
	    experiment.executeDelete();
	}catch(Exception e) {System.out.println("Error executing delete: " + e.getMessage());}
    }
    
    void rename(String new_name) throws DatabaseException 
    {
        experiment.renameNode(nid, new_name);
	info = experiment.getInfo(nid);
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

    public String toString() {return getFullPath(); }
   
}
    
        
	
	
	    	    