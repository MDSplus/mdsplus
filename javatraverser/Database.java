//package jTraverser;

public class Database {
    String name;
    int shot;
    boolean is_open = false;
    boolean is_readonly = false;
    boolean is_editable = false;
    static {
	System.loadLibrary("JavaMds");
    }
    public Database(String name, int shot)
    {
	this.name = name.toUpperCase();
	this.shot = shot;	
    }
    final public String getName() {return name; }
    final public int getShot() {return shot;}
    public boolean isOpen() {return is_open; }
    public boolean isEditable() {return is_editable; }
    public boolean isReadonly() {return is_readonly;}
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public native void open() throws DatabaseException;
    public native void write() throws DatabaseException;
    public native void close()throws DatabaseException;
    public native void quit()throws DatabaseException;
    public native Data getData(NidData nid) throws DatabaseException;
    public native void putData(NidData nid, Data data) throws DatabaseException;
    //public native DatabaseInfo getInfo(); throws DatabaseException;
    public native NodeInfo getInfo(NidData nid) throws DatabaseException;
    public native void setTags(NidData nid, String tags[]) throws DatabaseException;
    public native String[] getTags(NidData nid);
    public native void renameNode(NidData nid, String name) throws DatabaseException;
    public native NidData addNode(String name, int usage) throws DatabaseException;
    public native NidData[] startDelete(NidData nid[]) throws DatabaseException;
    public native void executeDelete() throws DatabaseException;
    public native NidData[] getSons(NidData nid) throws DatabaseException;
    public native NidData[] getMembers(NidData nid) throws DatabaseException;
    public native boolean isOn(NidData nid) throws DatabaseException;
    public native void setOn(NidData nid, boolean on) throws DatabaseException;
    public native NidData resolve(PathData pad) throws DatabaseException;
    public native void setDefault(NidData nid) throws DatabaseException;
    public native NidData getDefault() throws DatabaseException;
    public native NidData addDevice(String path, String model) throws DatabaseException;
  }  
    
    