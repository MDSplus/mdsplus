//package jTraverser;

public class Database implements RemoteTree{
    String name;
    int shot;
    boolean is_open = false;
    boolean is_readonly = false;
    boolean is_editable = false;
    static {
        try {
	    System.loadLibrary("JavaMds");
	    }catch(Exception e) {System.out.println("Cannot load library " + e); }
    }
    public Database() {super();}
    public Database(String name, int shot)
    {
	this.name = name.toUpperCase();
	this.shot = shot;	
    }
    public void setTree(String name, int shot)
    {
        this.name = name.toUpperCase();
        this.shot = shot;
    }
    
    public void setEditable(boolean editable)
    {
        this.is_editable = editable;
    }
    
    public void setReadonly(boolean readonly)
    {
        this.is_readonly = readonly;
    }
    public Data evaluateData(Data data, int ctx) throws DatabaseException
    {
        return evaluateSimpleData(data, ctx);
    }
    final public String getName() {return name; }
    final public int getShot() {return shot;}
    public boolean isOpen() {return is_open; }
    public boolean isEditable() {return is_editable; }
    public boolean isReadonly() {return is_readonly;}
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public native int open() throws DatabaseException;
    public native void write(int ctx) throws DatabaseException;
    public native void close(int ctx)throws DatabaseException;
    public native void quit(int ctx)throws DatabaseException;
    public native Data getData(NidData nid, int ctx) throws DatabaseException;
    public native Data evaluateData(NidData nid, int ctx) throws DatabaseException;
    public native Data evaluateSimpleData(Data data, int ctx) throws DatabaseException;
    public native void putData(NidData nid, Data data, int ctx) throws DatabaseException;
    //public native DatabaseInfo getInfo(); throws DatabaseException;
    public native NodeInfo getInfo(NidData nid, int ctx) throws DatabaseException;
    public native void setTags(NidData nid, String tags[], int ctx) throws DatabaseException;
    public native String[] getTags(NidData nid, int ctx);
    public native void renameNode(NidData nid, String name, int ctx) throws DatabaseException;
    public native NidData addNode(String name, int usage, int ctx) throws DatabaseException;
    public native void setSubtree(NidData nid, int ctx) throws DatabaseException;
    public native NidData[] startDelete(NidData nid[], int ctx) throws DatabaseException;
    public native void executeDelete(int ctx) throws DatabaseException;
    public native NidData[] getSons(NidData nid, int ctx) throws DatabaseException;
    public native NidData[] getMembers(NidData nid, int ctx) throws DatabaseException;
    public native boolean isOn(NidData nid, int ctx) throws DatabaseException;
    public native void setOn(NidData nid, boolean on, int ctx) throws DatabaseException;
    public native NidData resolve(PathData pad, int ctx) throws DatabaseException;
    public native void setDefault(NidData nid, int ctx) throws DatabaseException;
    public native NidData getDefault(int ctx) throws DatabaseException;
    public native NidData addDevice(String path, String model, int ctx) throws DatabaseException;
    public native void doAction(NidData nid, int ctx) throws DatabaseException;
    public native void doDeviceMethod(NidData nid, String method, int ctx) throws DatabaseException;
    public native NidData [] getWild(int usage_mask, int ctx) throws DatabaseException;
    public native int create(int shot) throws DatabaseException;
    public String dataToString(Data data){return "ERROR this method cannot be called";}
    public Data dataFromExpr(String expr){return null;}
    public native long saveContext();
    public native void restoreContext(long context);
    public int getCurrentShot() { return getCurrentShot(name);}
    public native int getCurrentShot(String experiment);
    public void setCurrentShot(int shot) {setCurrentShot(name, shot);}
    public native void setCurrentShot(String experiment, int shot);
    public native String getOriginalPartName(NidData nid) throws DatabaseException;
  }  
    
    