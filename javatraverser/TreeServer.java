import java.rmi.*;
import java.rmi.server.*;

class TreeServer extends UnicastRemoteObject implements RemoteTree
{
    String experiment;
    int shot;
    transient Database tree;
    
    public TreeServer() throws RemoteException
    {
        super();
    }
    public void setTree(String experiment, int shot) throws RemoteException
    {
        tree = new Database(experiment, shot);
    }
    
    public void setEditable(boolean editable) {tree.setEditable(editable);}
    public void setReadonly(boolean readonly) {tree.setReadonly(readonly);}
    public String getName() {return tree.getName(); }
    public int getShot(){return  tree.getShot();}
    public boolean isOpen() {return tree.isOpen();}
    public boolean isEditable(){return tree.isEditable();}
    public boolean isReadonly() {return tree.isReadonly();}
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public void open() throws DatabaseException {tree.open();}
    public void write() throws DatabaseException {tree.write();}
    public void close() throws DatabaseException {tree.close();}
    public void quit() throws DatabaseException {tree.quit();}
    public Data getData(NidData nid) throws DatabaseException {return tree.getData(nid);}
    public Data evaluateData(NidData nid) throws DatabaseException {return tree.evaluateData(nid);}
    public void putData(NidData nid, Data data) throws DatabaseException {tree.putData(nid, data); }
    //public native DatabaseInfo getInfo(); throws DatabaseException;
    public NodeInfo getInfo(NidData nid) throws DatabaseException {return tree.getInfo(nid);}
    public void setTags(NidData nid, String tags[]) throws DatabaseException {tree.setTags(nid, tags);}
    public String[] getTags(NidData nid)throws DatabaseException {return tree.getTags(nid);}
    public void renameNode(NidData nid, String name)throws DatabaseException {tree.renameNode(nid, name);}
    public NidData addNode(String name, int usage) throws DatabaseException {return tree.addNode(name, usage);}
    public NidData[] startDelete(NidData nid[])throws DatabaseException {return tree.startDelete(nid);}
    public void executeDelete()throws DatabaseException  {tree.executeDelete();}
    public NidData[] getSons(NidData nid) throws DatabaseException {return tree.getSons(nid);}
    public NidData[] getMembers(NidData nid) throws DatabaseException {return tree.getMembers(nid);}
    public boolean isOn(NidData nid) throws DatabaseException {return tree.isOn(nid);}
    public void setOn(NidData nid, boolean on) throws DatabaseException {tree.setOn(nid, on);}
    public NidData resolve(PathData pad) throws DatabaseException {return tree.resolve(pad);}
    public void setDefault(NidData nid)throws DatabaseException  {tree.setDefault(nid);}
    public NidData getDefault()throws DatabaseException  {return tree.getDefault();}
    public NidData addDevice(String path, String model)throws DatabaseException  {return tree.addDevice(path, model);}
    public void doAction(NidData nid) throws DatabaseException {tree.doAction(nid);}
    public void doDeviceMethod(NidData nid, String method) throws DatabaseException {tree.doDeviceMethod(nid, method);}
    public NidData [] getWild(int usage_mask) throws DatabaseException {return tree.getWild(usage_mask);}
    public void create(int shot) throws DatabaseException  {tree.create(shot);}
    public String dataToString(Data data){return data.toString();}
    public Data dataFromExpr(String expr){return Data.fromExpr(expr);}
    
    
    
    public static void main(String args[])
    {
        if (System.getSecurityManager() == null)
            System.setSecurityManager(new RMISecurityManager()
                {
                    public void checkLink(String lib){}
                });
        RemoteTree server = null;
        try {
            server = new TreeServer();
       }catch(Exception exc) 
       {
            System.err.println("Error creating TreeServer: " + exc);
            System.exit(0);
       }
       try {
        Naming.rebind("TreeServer", server);
       }catch(Exception exc) {System.err.println("Error in Naming.rebind: " + exc);}
    }
}  
    
    