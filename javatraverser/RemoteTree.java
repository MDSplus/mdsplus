import java.rmi.*;
import java.rmi.RemoteException.*;

public interface RemoteTree extends Remote {
    public void setTree(String experiment, int shot) throws RemoteException;
    public String getName() throws RemoteException;
    public int getShot() throws RemoteException;
    boolean isOpen() throws RemoteException;
    boolean isEditable() throws RemoteException;
    boolean isReadonly() throws RemoteException;
    void setEditable(boolean editable)throws RemoteException;
    void setReadonly(boolean readonly)throws RemoteException;
   /* Low level MDS database management routines, will be  masked by the Node class*/
    void open() throws RemoteException, DatabaseException;
    public void write() throws RemoteException, DatabaseException;
    public void close() throws RemoteException, DatabaseException;
    public void quit() throws RemoteException, DatabaseException;
    public Data getData(NidData nid)  throws RemoteException, DatabaseException;
    public Data evaluateData(NidData nid)  throws RemoteException, DatabaseException;
    public void putData(NidData nid, Data data)  throws RemoteException, DatabaseException;
    //public native DatabaseInfo getInfo(); throws DatabaseException;
    public NodeInfo getInfo(NidData nid)  throws RemoteException, DatabaseException;
    public void setTags(NidData nid, String tags[]) throws RemoteException, DatabaseException;
    public String[] getTags(NidData nid) throws RemoteException, DatabaseException;
    public void renameNode(NidData nid, String name) throws RemoteException, DatabaseException;
    public NidData addNode(String name, int usage) throws RemoteException, DatabaseException;
    public NidData[] startDelete(NidData nid[]) throws RemoteException, DatabaseException;
    public void executeDelete() throws RemoteException, DatabaseException;
    public NidData[] getSons(NidData nid) throws RemoteException, DatabaseException;
    public NidData[] getMembers(NidData nid) throws RemoteException, DatabaseException;
    public boolean isOn(NidData nid) throws RemoteException, DatabaseException;
    public void setOn(NidData nid, boolean on) throws RemoteException, DatabaseException;
    public NidData resolve(PathData pad) throws RemoteException, DatabaseException;
    public void setDefault(NidData nid) throws RemoteException, DatabaseException;
    public NidData getDefault() throws RemoteException, DatabaseException;
    public NidData addDevice(String path, String model) throws RemoteException, DatabaseException;
    public void doAction(NidData nid) throws RemoteException, DatabaseException;
    public void doDeviceMethod(NidData nid, String method) throws RemoteException, DatabaseException;
    public NidData [] getWild(int usage_mask) throws RemoteException, DatabaseException;
    public void create(int shot) throws RemoteException, DatabaseException;
  }  
    
    