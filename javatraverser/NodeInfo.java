//package jTraverser;

/** class NodeInfo carries all the NCI information */
import java.io.*;

public class NodeInfo implements Serializable
{

    boolean on, parent_on, setup, write_once, compressible, compress_on_put,
    no_write_model, no_write_shot;
    String date_inserted, name, fullpath, minpath, path;
    int owner, dtype, dclass, length, usage, conglomerate_elt, conglomerate_nids;
    static NodeInfo getNodeInfo(boolean on, boolean parent_on, boolean setup, boolean write_once,
	    boolean compressible, boolean compress_on_put, boolean no_write_model,
	    boolean no_write_shot, String date_inserted,
	    int owner, int dtype, int dclass, int length, int usage,
	    String name, String fullpath, String minpath, String path, int conglomerate_nids, int conglomerate_elt)

    {
	return new NodeInfo(on, parent_on, setup, write_once, compressible, compress_on_put,
	    no_write_model, no_write_shot, date_inserted, owner, dtype, dclass, length,
	    usage, name, fullpath, minpath, path, conglomerate_nids, conglomerate_elt);
    }

    NodeInfo(boolean on, boolean parent_on, boolean setup, boolean write_once,
	boolean compressible, boolean compress_on_put, boolean no_write_model,
	boolean no_write_shot,String date_inserted,
	int owner, int dtype, int dclass, int length, int usage,
	String name, String fullpath, String minpath, String path, int conglomerate_nids, int conglomerate_elt)
    {
	this.on = on;
	this.parent_on = parent_on;
	this.setup = setup;
	this.write_once = write_once;
	this.compressible = compressible;
	this.compress_on_put = compress_on_put;
	this.no_write_model = no_write_model;
	this.no_write_shot = no_write_shot;
	this.date_inserted = date_inserted;
	this.owner = owner;
	this.dtype = dtype;
	this.dclass = dclass;
	this.length = length;
	this.usage = usage;
	this.name = name;
	this.fullpath = fullpath;
	this.minpath = minpath;
    this.path = path;
	this.conglomerate_nids = conglomerate_nids;
	this.conglomerate_elt = conglomerate_elt;
    }
    public final boolean isOn() {return on;}
    public final boolean isParentOn() {return parent_on;}
    public final boolean isSetup() {return setup;}
    public final boolean isWriteOnce() { return write_once;}
    public final boolean isCompressible() { return compressible;}
    public final boolean isCompressOnPut() { return compress_on_put;}
    public final boolean isNoWriteModel() { return no_write_model;}
    public final boolean isNoWriteShot() { return no_write_shot;}

    public final String getDate() {return date_inserted;}
    public final int getOwner() {return owner;}
    public final int getDtype() {return dtype;}
    public final int getDClass() {return dclass;}
    public final int getLength() {return length;}
    public final String getName() {return name; }
    public final String getFullPath() {return fullpath; }
    public final String getMinpath() { return minpath;}
    public final String getPath() { return path;}
    public final int getConglomerateNids() {return conglomerate_nids; }
    public final int getConglomerateElt() { return conglomerate_elt;}
    public final int getUsage() {return usage; }
    public static final int USAGE_ANY = 0;
    public static final int USAGE_NONE = 1;
    public static final int USAGE_STRUCTURE= 1;
    public static final int USAGE_ACTION = 2;
    public static final int USAGE_DEVICE = 3;
    public static final int USAGE_DISPATCH = 4;
    public static final int USAGE_NUMERIC = 5;
    public static final int USAGE_SIGNAL = 6;
    public static final int USAGE_TASK = 7;
    public static final int USAGE_TEXT = 8;
    public static final int USAGE_WINDOW = 9;
    public static final int USAGE_AXIS = 10;
    public static final int USAGE_SUBTREE = 11;
    public static final int USAGE_COMPOUND_DATA = 12;
    public static final int USAGE_MAXIMUM = 12;



    public static final int WRITE_ONCE = 0x00000080;
    public static final int COMPRESSIBLE   =     0x00000100;
    public static final int DO_NOT_COMPRESS =    0x00000200;
    public static final int COMPRESS_ON_PUT  =   0x00000400;
    public static final int NO_WRITE_MODEL  =    0x00000800;
    public static final int NO_WRITE_SHOT   =    0x00001000;
}