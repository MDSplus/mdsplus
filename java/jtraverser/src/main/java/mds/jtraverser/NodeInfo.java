package mds.jtraverser;
//package jTraverser;

/** class NodeInfo carries all the NCI information */
import java.io.*;

public class NodeInfo implements Serializable
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	public static final byte USAGE_ANY = 0;
	public static final byte USAGE_NONE = 1;
	public static final byte USAGE_STRUCTURE = 1;
	public static final byte USAGE_ACTION = 2;
	public static final byte USAGE_DEVICE = 3;
	public static final byte USAGE_DISPATCH = 4;
	public static final byte USAGE_NUMERIC = 5;
	public static final byte USAGE_SIGNAL = 6;
	public static final byte USAGE_TASK = 7;
	public static final byte USAGE_TEXT = 8;
	public static final byte USAGE_WINDOW = 9;
	public static final byte USAGE_AXIS = 10;
	public static final byte USAGE_SUBTREE = 11;
	public static final byte USAGE_COMPOUND_DATA = 12;
	public static final byte USAGE_MAXIMUM = 12;
	public static final int STATE = 1 << 0;
	public static final int PARENT_STATE = 1 << 1;
	public static final int ESSENTIAL = 1 << 2;
	public static final int CACHED = 1 << 3;
	public static final int VERSION = 1 << 4;
	public static final int SEGMENTED = 1 << 5;
	public static final int SETUP = 1 << 6;
	public static final int WRITE_ONCE = 1 << 7;
	public static final int COMPRESSIBLE = 1 << 8;
	public static final int DO_NOT_COMPRESS = 1 << 9;
	public static final int COMPRESS_ON_PUT = 1 << 10;
	public static final int NO_WRITE_MODEL = 1 << 11;
	public static final int NO_WRITE_SHOT = 1 << 12;
	public static final int PATH_REFERENCE = 1 << 13;
	public static final int NID_REFERENCE = 1 << 14;
	public static final int INCLUDE_IN_PULSE = 1 << 15;
	public static final int COMPRESS_SEGMENTS = 1 << 16;

	public static final NodeInfo getNodeInfo(String dclass, String dtype, String usage, int flags, int owner,
			int length, int conglomerate_nids, int conglomerate_elt, String date_inserted, String name, String fullpath,
			String minpath, String path, int numSegments)
	{
		return new NodeInfo(dclass, dtype, usage, flags, owner, length, conglomerate_nids, conglomerate_elt,
				date_inserted, name, fullpath, minpath, path, numSegments);
	}

	private final String date_inserted, name, fullpath, minpath, path;
	private final String dtype, dclass;
	private final String usage;
	private final int owner, length, conglomerate_elt, conglomerate_nids;
	private int flags;
	private final int numSegments;

	public NodeInfo(String dclass, String dtype, String usage, int flags, int owner, int length, int conglomerate_nids,
			int conglomerate_elt, String date_inserted, String name, String fullpath, String minpath, String path,
			int numSegments)
	{
		this.dclass = dclass;
		this.dtype = dtype;
		this.usage = usage;
		this.flags = flags;
		this.owner = owner;
		this.length = length;
		this.conglomerate_nids = conglomerate_nids;
		this.conglomerate_elt = conglomerate_elt;
		this.date_inserted = date_inserted;
		this.name = name.trim();
		this.fullpath = fullpath;
		this.minpath = minpath;
		this.path = path;
		this.numSegments = numSegments;
	}

	public final int getConglomerateElt()
	{ return conglomerate_elt; }

	public final int getConglomerateNids()
	{ return conglomerate_nids; }

	public final String getDate()
	{ return date_inserted; }

	public final String getDClass()
	{ return dclass; }

	public final String getDType()
	{ return dtype; }

	public final int getFlags()
	{ return flags; }

	public final String getFullPath()
	{ return fullpath; }

	public final int getLength()
	{ return length; }

	public final String getMinPath()
	{ return minpath; }

	public final String getName()
	{ return name; }

	public final int getNumSegments()
	{ return numSegments; }

	public final int getOwner()
	{ return owner; }

	public final String getPath()
	{ return path; }

	public final String getUsage()
	{ return usage; }

	public final boolean isCached()
	{ return (flags & CACHED) != 0; }

	public final boolean isCompressible()
	{ return (flags & COMPRESSIBLE) != 0; }

	public final boolean isCompressOnPut()
	{ return (flags & COMPRESS_ON_PUT) != 0; }

	public final boolean isCompressSegments()
	{ return (flags & COMPRESS_SEGMENTS) != 0; }

	public final boolean isDoNotCompress()
	{ return (flags & DO_NOT_COMPRESS) != 0; }

	public final boolean isEssential()
	{ return (flags & ESSENTIAL) != 0; }

	public final boolean isIncludeInPulse()
	{ return (flags & INCLUDE_IN_PULSE) != 0; }

	public final boolean isNidReference()
	{ return (flags & NID_REFERENCE) != 0; }

	public final boolean isNoWriteModel()
	{ return (flags & NO_WRITE_MODEL) != 0; }

	public final boolean isNoWriteShot()
	{ return (flags & NO_WRITE_SHOT) != 0; }

	public final boolean isOn()
	{ return !isState(); }

	public final boolean isParentOn()
	{ return !isParentState(); }

	public final boolean isParentState()
	{ return (flags & PARENT_STATE) != 0; }

	public final boolean isPathReference()
	{ return (flags & PATH_REFERENCE) != 0; }

	public final boolean isSegmented()
	{ return (flags & SEGMENTED) != 0; }

	public final boolean isSetup()
	{ return (flags & SETUP) != 0; }

	public final boolean isState()
	{ return (flags & STATE) != 0; }

	public final boolean isVersion()
	{ return (flags & VERSION) != 0; }

	public final boolean isWriteOnce()
	{ return (flags & WRITE_ONCE) != 0; }

	public final void setFlags(int flags)
	{ this.flags = flags; }
}
