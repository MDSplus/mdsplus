
import java.text.SimpleDateFormat;
import java.util.Vector;
import mds.devices.Interface;

public class Database implements Interface{
    static long VMS_OFFSET = 0x7c95674beb4000L;
    String name;
    int shot;
    boolean is_open = false;
    boolean is_readonly = false;
    boolean is_editable = false;
    MDSplus.Tree tree;
    public Database() {super();}
    static Vector<TreeInterfaceEvent> events = new Vector<TreeInterfaceEvent>();
    public Database(MDSplus.Tree tree)
    {
	this.name = tree.getName();
	this.shot = tree.getShot();
        this.tree = tree;
    }
    public void setTree(String name, int shot)
    {
	this.name = name.toUpperCase();
	this.shot = shot;
        try {
            tree = new MDSplus.Tree(name, shot);
        }catch(Exception exc)
        {
            tree = null;
        }
    }

    public void setEditable(boolean editable)
    {
	this.is_editable = editable;
    }

    public void setReadonly(boolean readonly)
    {
	this.is_readonly = readonly;
    }
    final public String getName() {return name; }
    final public int getShot() {return shot;}
    public boolean isOpen() {return is_open; }
    public boolean isEditable() {return is_editable; }
    public boolean isReadonly() {return is_readonly;}
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public void open() throws Exception{
        if(!is_editable)
        {
            if(is_readonly)
                tree = new MDSplus.Tree(name, shot, "READONLY");
            else
                tree = new MDSplus.Tree(name, shot);
        }
        else
        {
            try {
                tree = new MDSplus.Tree(name, shot, "EDIT");
             } catch(Exception exc)
             {
                 tree = new MDSplus.Tree(name, shot, "NEW");
             }
        }
            
    }
    public void write() throws Exception
    {
        tree.write();
    }
    public  void close()throws Exception
    {
        tree.close();
    }
    public  void quit()throws Exception
    {
        tree.quit();
    }
    public  String getDataExpr(int nid) throws Exception
    {
        return new MDSplus.TreeNode(nid, tree).getData().toString();
    }
    public void putDataExpr(int nid, String expr) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        if(expr == null || expr.trim().length() == 0)
            node.deleteData();
        else
        {
            MDSplus.Data data = tree.tdiCompile(expr);
            new MDSplus.TreeNode(nid, tree).putData(data);
        }
    }
    public  MDSplus.Data getData(int nid) throws Exception
    {
        return new MDSplus.TreeNode(nid, tree).getData();
    }
    public void putData(int nid, MDSplus.Data data) throws Exception
    {
        new MDSplus.TreeNode(nid, tree).putData(data);
    }
    public NodeInfo getInfo(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        
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
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        for(int i = 0; i < tags.length; i++)
            node.addTag(tags[i]);
    }
    public String[] getTags(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.findTags();
    }
    public void renameNode(int nid, String name) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        node.rename(name);
    }
    public int addNode(String name, String usage) throws Exception
    {
        return tree.addNode(name, usage).getNid();
    }
    public void setSubtree(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        node.setSubtree(true);
    }
    
    public void deleteNode(int nid) throws Exception
    {
       MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
       tree.deleteNode(node.getFullPath());
    }
    public int[] getSons(int nid) throws Exception
    {
      MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
      MDSplus.TreeNodeArray sons = node.getChildren();
      int[] sonNids = new int[sons.size()];
      for(int i = 0; i < sons.size(); i++)
          sonNids[i] = sons.getElementAt(i).getNid();
      return sonNids;        
    }
    public  int[] getMembers(int nid) throws Exception
    {
      MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
      MDSplus.TreeNodeArray members = node.getMembers();
      int[] memberNids = new int[members.size()];
      for(int i = 0; i < members.size(); i++)
          memberNids[i] = members.getElementAt(i).getNid();
      return memberNids;        
    }
    public boolean isOn(int nid) throws Exception
    {
       MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
       return node.isOn();
    }
    public void setOn(int nid, boolean on) throws Exception
    {
       MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
       node.setOn(on);
    }
    public int getNode(String path) throws Exception
    {
        return tree.getNode(path).getNid();
    }
    public void setDefault(int nid) throws Exception
    {
       MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
       tree.setDefault(node);
    }
    public int getDefault() throws Exception
    {
        return tree.getDefault().getNid();
    }
    public int addDevice(String path, String model) throws Exception
    {
        tree.addDevice(path, model);
        return tree.getNode(path).getNid();
    }
    public int doAction(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.doAction();
    }
    public void doDeviceMethod(int nid, String method) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        node.doMethod(method);
    }
    public int [] getWild(int usage_mask) throws Exception
    {
        MDSplus.TreeNodeArray nodes = tree.getNodeWild("***", usage_mask);
        int [] nids = new int[nodes.size()];
        for(int i = 0; i < nodes.size(); i++)
            nids[i] = nodes.getElementAt(i).getNid();
        return nids;
    }
    public int [] getWild(int nid, int usage_mask) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        MDSplus.TreeNodeArray nodes = tree.getNodeWild(node.getFullPath()+"***", usage_mask);
        int [] nids = new int[nodes.size()];
        for(int i = 0; i < nodes.size(); i++)
            nids[i] = nodes.getElementAt(i).getNid();
        return nids;
    }
    public void clearNciFlag(int nid, int flagMask) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        node.clearNciFlag(flagMask);
       
    }
     public void setNciFlag(int nid, int flagMask) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        node.setNciFlag(flagMask);
       
    }
    public boolean getNciFlag(int nid, int flagMask) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getNciFlag(flagMask);
    }
   public int getNciFlags(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getNciFlags();
    }
    public int getInt(int nid)  throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getInt();
     }
    public float getFloat(int nid)  throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getFloat();
    }
    public int[] getIntArray(int nid)  throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getIntArray();
     }
    public float[] getFloatArray(int nid)  throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getFloatArray();
    }
    public String getFullPath(int nid)  throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getFullPath();
    }
     
    public int[] getIntArray(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getIntArray();
    }
    public float[] getFloatArray(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getFloatArray();
    }

    public double getDouble(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getDouble();
    }

    public float getFloat(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getFloat();
    }

    public int getInt(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getInt();
    }

    public String getString(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getString();
    }
    public String[] getStringArray(String expr) throws Exception
    {
        return tree.tdiCompile(expr).getStringArray();
    }
    public String execute(String expr) throws Exception
    {
        return tree.tdiExecute(expr).toString();
    }
    public String evaluate(String expr) throws Exception
    {
        return tree.tdiEvaluate(tree.tdiCompile(expr)).toString();
    }
    public boolean isString(String expr)
    {
        try {
            return tree.tdiExecute(expr) instanceof MDSplus.String;
        }catch(Exception exc){return false;}
    }
    public String getUsage(int nid)
    {
        try {
            MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
            return node.getUsage();
        }catch(Exception exc){return "NONE";}
    }
    public int getNumConglomerateNids(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getConglomerateNodes().size();
    }
    public String getNodeName(int nid) throws Exception
    {
        MDSplus.TreeNode node = new MDSplus.TreeNode(nid, tree);
        return node.getNodeName();
       
    }
    public int  registerMdsEvent(String eventName, DataChangeListener listener)
    {
        try {
            events.add(new TreeInterfaceEvent(eventName, listener));
            return events.size() - 1;
        }catch(Exception exc)
        {
            return -1;
        }
        
    }
    public void unregisterMdsEvent(int id)
    {
        try {
            events.elementAt(id).dispose();
        }catch(Exception exc){}
    }
    class TreeInterfaceEvent extends MDSplus.Event
    {
       DataChangeListener listener;
       public  TreeInterfaceEvent(String eventName, DataChangeListener listener) throws Exception
       {
            super(eventName);
            this.listener = listener;
       }
       public void run()
       {
           listener.dataChanged(new DataChangeEvent(Database.this.getName(), 
                   Database.this.getShot(), this.getName()));
       }
    }
    
  }
 