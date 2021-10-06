package MDSplus;
import java.util.*;

/**
 * Class Tree brings all the tree-wide information.
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.43
 */
public class Tree
{
    static class TreeCtxInfo
    {
        long tid;
        long ctx;
        TreeCtxInfo(long tid, long ctx)
        {
            this.tid = tid;
            this.ctx = ctx;
        }
    }
    
    
	static final int DbiNAME = 1, DbiSHOTID = 2, DbiMODIFIED = 3, DbiOPEN_FOR_EDIT = 4, DbiINDEX = 5,
			DbiNUMBER_OPENED = 6, DbiMAX_OPEN = 7, DbiDEFAULT = 8, DbiOPEN_READONLY = 9, DbiVERSIONS_IN_MODEL = 10,
			DbiVERSIONS_IN_PULSE = 11;
	static public final int TreeUSAGE_ANY = 0, TreeUSAGE_NONE = 1, TreeUSAGE_STRUCTURE = 1, TreeUSAGE_ACTION = 2,
			TreeUSAGE_DEVICE = 3, TreeUSAGE_DISPATCH = 4, TreeUSAGE_NUMERIC = 5, TreeUSAGE_SIGNAL = 6,
			TreeUSAGE_TASK = 7, TreeUSAGE_TEXT = 8, TreeUSAGE_WINDOW = 9, TreeUSAGE_AXIS = 10, TreeUSAGE_SUBTREE = 11,
			TreeUSAGE_COMPOUND_DATA = 12;
	private static Tree active;
	static
	{
		try
		{
			int loaded = 0;
			try
			{
				java.lang.String value = System.getenv("JavaMdsLib");
				if (value == null)
				{
					value = System.getProperty("JavaMdsLib");
				}
				if (value != null)
				{
					System.load(value);
					loaded = 1;
				}
			}
			catch (final Throwable e)
			{}
			if (loaded == 0)
			{
				System.loadLibrary("JavaMds");
			}
		}
		catch (final Throwable e)
		{
			System.out.println("Error loading library javamds: " + e);
			e.printStackTrace();
		}
	}
	private final int shot;
	private final java.lang.String name;
	private java.lang.String mode;
        protected Vector<TreeCtxInfo> treeCtxInfoV = new Vector<TreeCtxInfo>();
	protected long ctx = 0l;
	private boolean open = false;
	private boolean edit = false;
	private static final java.lang.String OPEN_CLOSED = "CLOSED";
	public static final java.lang.String OPEN_NORMAL = "NORMAL";
	public static final java.lang.String OPEN_READONLY = "READONLY";
	public static final java.lang.String OPEN_NEW = "NEW";
	public static final java.lang.String OPEN_EDIT = "EDIT";

	public Tree(java.lang.String name, int shot) throws MdsException
	{
		this(name, shot, OPEN_NORMAL);
	}

	public Tree(java.lang.String name, int shot, java.lang.String mode) throws MdsException
	{
		this.shot = shot;
		this.name = name;
		if (OPEN_READONLY.equalsIgnoreCase(mode))
			readonly();
		else if (OPEN_NORMAL.equalsIgnoreCase(mode))
			this.normal();
		else if (OPEN_EDIT.equalsIgnoreCase(mode))
			edit();
		else if (OPEN_NEW.equalsIgnoreCase(mode))
			_new();
		else
			this.mode = OPEN_CLOSED;
	}

	public void close() throws MdsException
	{
		if (open)
			closeTree(getCtx(), name, shot);
		edit = open = false;
		this.mode = OPEN_CLOSED;
	}

	
	public boolean isOpen()
	{ return open; }

	@Override
	public java.lang.String toString()
	{
		return ("Tree(" + name + ", " + getShot() + ", " + mode + ")");
	}

	/**
	 * Set the oassed tree as the active one
	 *
	 * @param tree
	 */
	public static void setActiveTree(Tree tree) throws MdsException
	{ Tree.active = tree; }

	static Tree getTree(java.lang.String name, int shot) throws MdsException
	{
		return new Tree(name, shot);
	}

	/**
	 * Returns the currently opened and active tree
	 */
	public java.lang.String getName()
	{ return name; }

	public int getShot()
	{
		if (shot != 0)
			return shot;
		else
			try
			{
				return getCurrent();
			}
			catch (final Exception exc)
			{
				return 0;
			}
	}

	public java.lang.String getMode()
	{ return mode; }

	static native int findNode(long ctx, java.lang.String path) throws MdsException;

	private native void openTree(long ctx, java.lang.String name, int shot, boolean readonly) throws MdsException;

	private native void closeTree(long ctx, java.lang.String name, int shot) throws MdsException;

	private native void editTree(long ctx, java.lang.String name, int shot, boolean isNew) throws MdsException;

	private native void writeTree(long ctx, java.lang.String name, int shot) throws MdsException;

	private native void quitTree(long ctx, java.lang.String name, int shot) throws MdsException;

	private static native int[] getWild(long ctx, java.lang.String path, int usage) throws MdsException;

	private static native int getDefaultNid(long ctx) throws MdsException;

	private static native void setDefaultNid(long ctx, int nid) throws MdsException;

	private static native boolean getDbiFlag(long ctx, int dbiType) throws MdsException;

	private static native void setDbiFlag(long ctx, boolean flag, int dbiType) throws MdsException;

	private static native void setTreeViewDate(java.lang.String date) throws MdsException;

	private static native void setTreeTimeContext(long ctx, Data start, Data end, Data delta);

	public static native void setCurrent(java.lang.String name, int shot) throws MdsException;

	private static native int getCurrent(java.lang.String treename) throws MdsException;

	private static native void createPulseFile(long ctx, int pulseSot) throws MdsException;

	private static native void deletePulseFile(long ctx, int pulseSot) throws MdsException;

	private static native java.lang.String[] findTreeTags(long ctx, java.lang.String wild) throws MdsException;

	private static native int addTreeNode(long ctx, java.lang.String name, int usage) throws MdsException;

	private static native void addTreeDevice(long ctx, java.lang.String name, java.lang.String type)
			throws MdsException;

	private static native void deleteTreeNode(long ctx, java.lang.String name) throws MdsException;

	private static native void removeTreeTag(long ctx, java.lang.String tag) throws MdsException;

	private static native long getDatafileSize(long ctx);

	// Thread safe context dependent tdi operations
	private static native Data execute(long ctx, java.lang.String expr, Data[] args);

	private static native Data compile(long ctx, java.lang.String expr, Data[] args);

	private static native Data data(long ctx, Data data);

	private static native Data evaluate(long ctx, Data data);


	/**
	 * Return TreeNode for the data item corresponding to the passed pathname
	 *
	 * @param path
	 */
	public TreeNode getNode(java.lang.String path) throws MdsException
	{
		final TreeNode t = new TreeNode(findNode(getCtx(), path), this);
		t.setCtxTree(this);
		return t;
	}

	/**
	 * Returns the TreeNode for the data item whose internal address (nid) is passed
	 * as argument.
	 *
	 * @param nid
	 */
	public TreeNode getNode(int nid) throws MdsException
	{
		return new TreeNode(nid, this);
	}

	/**
	 * Return the TreeNode for the data itemspecified by the passed TreePath
	 * instance.
	 *
	 * @param path
	 */
	public TreeNode getNode(TreePath path) throws MdsException
	{
		return new TreeNode(findNode(getCtx(), path.getString()), this);
	}

	/**
	 * Return the TreeNode array for all the successor nodes of the specified usage
	 * with respect of the passed path.
	 *
	 * @param path
	 * @param usage
	 */
	public TreeNodeArray getNodeWild(java.lang.String path) throws MdsException
	{
		return getNodeWild(path, -1);
	}

	public TreeNodeArray getNodeWild(java.lang.String path, int usage) throws MdsException
	{
		return new TreeNodeArray(getWild(getCtx(), path, usage), this);
	}

	public TreeNodeArray getNodeWild(java.lang.String path, java.lang.String usage) throws MdsException
	{
		return getNodeWild(path, convertUsage(usage));
	}

	/**
	 * Get the TreeNode for the default node.
	 */
	public TreeNode getDefault() throws MdsException
	{ return new TreeNode(getDefaultNid(getCtx()), this); }

	/**
	 * Set the default position in the tree.
	 *
	 * @param node
	 */
	public void setDefault(TreeNode node) throws MdsException
	{
		setDefaultNid(getCtx(), node.getNid());
	}

	/**
	 * Returns true if the tree supports versioning in model.
	 */
	public boolean versionsInModelEnabled() throws MdsException
	{
		return getDbiFlag(getCtx(), DbiVERSIONS_IN_MODEL);
	}

	/**
	 * Returns true if the tree supports versioning in pulse.
	 */
	public boolean versionsInPulseEnabled() throws MdsException
	{
		return getDbiFlag(getCtx(), DbiVERSIONS_IN_PULSE);
	}

	public void setVersionsInModel(boolean enabled) throws MdsException
	{
		setDbiFlag(getCtx(), enabled, DbiVERSIONS_IN_MODEL);
	}

	public void setVersionsInPulse(boolean enabled) throws MdsException
	{
		setDbiFlag(getCtx(), enabled, DbiVERSIONS_IN_PULSE);
	}

	public boolean isModified() throws MdsException
	{ return getDbiFlag(getCtx(), DbiMODIFIED); }

	public boolean isOpenForEdit() throws MdsException
	{ return getDbiFlag(getCtx(), DbiOPEN_FOR_EDIT); }

	public boolean isReadOnly() throws MdsException
	{ return getDbiFlag(getCtx(), DbiOPEN_READONLY); }

	/**
	 * Set the version date: all read data will refer to the version active to that
	 * date.
	 *
	 * @param date
	 */
	public void setViewDate(java.util.Date date) throws MdsException
	{
		setTreeViewDate(new java.text.SimpleDateFormat("dd-MMM-yyyy HH:mm:ss").format(date));
	}

	/**
	 * Set the current time context. Only the specified portion of segmented data
	 * will be returned by TreeNode.getData() method
	 *
	 * @param start
	 * @param end
	 * @param delta
	 */
	public void setTimeContext(Data start, Data end, Data delta) throws MdsException
	{
		setTreeTimeContext(getCtx(), start, end, delta);
	}

	/**
	 * Set the current tree and shot.
	 *
	 * @param shot
	 * @param treename
	 */
	public void setCurrent() throws MdsException
	{
		setCurrent(name, shot);
	}

	/**
	 * Return the current shot for that tree.
	 *
	 * @param treename
	 */
	public int getCurrent() throws MdsException
	{ return getCurrent(name); }

	/**
	 * Create a new pulse file
	 *
	 * @param shot
	 */
	public void createPulse(int pulseShot) throws MdsException
	{
		createPulseFile(getCtx(), pulseShot);
	}

	/**
	 * Delete a pulse file.
	 *
	 * @param shot
	 */
	public void deletePulse(int pulseShot) throws MdsException
	{
		deletePulseFile(getCtx(), pulseShot);
	}

	/**
	 * Returns the list of tags matching the specified wild name.
	 *
	 * @param wild
	 */
	public java.lang.String[] findTags(java.lang.String wild) throws MdsException
	{
		return findTreeTags(getCtx(), wild);
	}

	/**
	 * Open the tree for editing.
	 */
	public synchronized void normal() throws MdsException
	{
		openTree(ctx, name, shot, false);
		edit = false;
		open = true;
		mode = OPEN_NORMAL;
                treeCtxInfoV.clear();
                treeCtxInfoV.addElement(new TreeCtxInfo(Thread.currentThread().getId(), ctx));
	}

	public synchronized void readonly() throws MdsException
	{
		openTree(ctx, name, shot, true);
		edit = false;
		open = true;
		mode = OPEN_READONLY;
                treeCtxInfoV.clear();
                treeCtxInfoV.addElement(new TreeCtxInfo(Thread.currentThread().getId(), ctx));
               
	}

	public synchronized void edit() throws MdsException
	{
 		if (open)
			closeTree(getCtx(), name, shot);
                ctx = 0;
		editTree(ctx, name, shot, false);
		edit = true;
		open = true;
		mode = OPEN_EDIT;
                treeCtxInfoV.clear();
                treeCtxInfoV.addElement(new TreeCtxInfo(Thread.currentThread().getId(), ctx));
	}

	private synchronized void _new() throws MdsException
	{
		editTree(ctx, name, shot, true);
		edit = true;
		open = true;
		mode = OPEN_EDIT;
                treeCtxInfoV.clear();
                treeCtxInfoV.addElement(new TreeCtxInfo(Thread.currentThread().getId(), ctx));
	}
        
        protected synchronized long getCtx() throws MdsException
	{
           // if(true) return ctx;
            long tid = Thread.currentThread().getId();
            for(int i = 0; i < treeCtxInfoV.size(); i++)
            {
                if (treeCtxInfoV.elementAt(i).tid == tid)
                {
                     return treeCtxInfoV.elementAt(i).ctx;
                }
            }
            //If we arrive here, a new context must be created
            ctx = 0;
            if(edit)
            {
 		editTree(ctx, name, shot, false);
            }
            else
            {
                openTree(ctx, name, shot, mode == OPEN_READONLY);
            }
            treeCtxInfoV.addElement(new TreeCtxInfo(Thread.currentThread().getId(), ctx));
            return ctx;
        }
         

	/**
	 * Write the tree under edit.
	 */
	public void write() throws MdsException
	{
		writeTree(getCtx(), name, shot);
	}

	/**
	 * quit editing session.
	 */
	public void quit() throws MdsException
	{
		if (open)
			quitTree(getCtx(), name, shot);
		edit = open = false;
		this.mode = OPEN_CLOSED;
	}

	static int convertUsage(java.lang.String usageStr)
	{
		final java.lang.String upUsageStr = usageStr.toUpperCase();
		if (upUsageStr.equals("ANY"))
			return TreeUSAGE_ANY;
		if (upUsageStr.equals("STRUCTURE"))
			return TreeUSAGE_STRUCTURE;
		if (upUsageStr.equals("ACTION"))
			return TreeUSAGE_ACTION;
		if (upUsageStr.equals("DEVICE"))
			return TreeUSAGE_DEVICE;
		if (upUsageStr.equals("DISPATCH"))
			return TreeUSAGE_DISPATCH;
		if (upUsageStr.equals("NUMERIC"))
			return TreeUSAGE_NUMERIC;
		if (upUsageStr.equals("SIGNAL"))
			return TreeUSAGE_SIGNAL;
		if (upUsageStr.equals("TASK"))
			return TreeUSAGE_TASK;
		if (upUsageStr.equals("TEXT"))
			return TreeUSAGE_TEXT;
		if (upUsageStr.equals("WINDOW"))
			return TreeUSAGE_WINDOW;
		if (upUsageStr.equals("AXIS"))
			return TreeUSAGE_AXIS;
		if (upUsageStr.equals("SUBTREE"))
			return TreeUSAGE_SUBTREE;
		if (upUsageStr.equals("COMPOUND_DATA"))
			return TreeUSAGE_COMPOUND_DATA;
		return 0;
	}

	/**
	 * Add a new node to the tree open for edit.
	 *
	 * @param name
	 * @param usage
	 */
	public TreeNode addNode(java.lang.String name, java.lang.String usage) throws MdsException
	{
		final int newNid = addTreeNode(getCtx(), name, convertUsage(usage));
		return new TreeNode(newNid, this);
	}

	/**
	 * Add a new device instance to the tree open for edit.
	 *
	 * @param name
	 * @param type
	 */
	public void addDevice(java.lang.String name, java.lang.String type) throws MdsException
	{
		addTreeDevice(getCtx(), name, type);
	}

	/**
	 * Delete the node and its descendants from the tree open for edit.
	 *
	 * @param name
	 */
	public void deleteNode(java.lang.String name) throws MdsException
	{
		deleteTreeNode(getCtx(), name);
	}

	public void removeTag(java.lang.String tag) throws MdsException
	{
		removeTreeTag(getCtx(), tag);
	}

	public long getDatafileSize() throws MdsException
	{
		final long size = getDatafileSize(getCtx());
		if (size == -1)
			throw new MdsException("Cannot get Datafile Size");
		return size;
	}

	public Data tdiCompile(java.lang.String expr, Data... args)
	{
		if (expr == null || expr.isEmpty())
			return new Data();
                try {
                    final Data retData = compile(getCtx(), expr, args);
                    retData.setCtxTree(this);
                    return retData;
                }catch(Exception exc){return new Data();}
	}

	public Data tdiExecute(java.lang.String expr, Data... args)
	{
		if (expr == null || expr.isEmpty())
			return new Data();
                try {
                    final Data retData = execute(getCtx(), expr, args);
                    retData.setCtxTree(this);
                    return retData;
               }catch(Exception exc){return new Data();}
	}

	public Data tdiData(Data data)
	{
                try {
                    final Data retData = data(getCtx(), data);
                    retData.setCtxTree(this);
                    return retData;
               }catch(Exception exc){return new Data();}
	}

	public Data tdiEvaluate(Data data)
	{
                try {
                    final Data retData = evaluate(getCtx(), data);
                    retData.setCtxTree(this);
                    return retData;
              }catch(Exception exc){return new Data();}
	}

	public static Tree getActiveTree()
	{ return Tree.active; }
}
