package MDSplus;

/**
 * Class Tree brings all the tree-wide information.
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.43
 */
public class Tree {

   
static final int  DbiNAME = 1,
                DbiSHOTID = 2,	
                DbiMODIFIED = 3,
                DbiOPEN_FOR_EDIT = 4,
                DbiINDEX = 5,
                DbiNUMBER_OPENED = 6,
                DbiMAX_OPEN = 7,
                DbiDEFAULT = 8,
                DbiOPEN_READONLY =  9,
                DbiVERSIONS_IN_MODEL = 10,
                DbiVERSIONS_IN_PULSE = 11;

static public final int  TreeUSAGE_ANY  = 0,
                TreeUSAGE_NONE = 1 ,
                TreeUSAGE_STRUCTURE = 1,
                TreeUSAGE_ACTION = 2,
                TreeUSAGE_DEVICE = 3,
                TreeUSAGE_DISPATCH = 4,
                TreeUSAGE_NUMERIC = 5,
                TreeUSAGE_SIGNAL = 6,
                TreeUSAGE_TASK = 7,
                TreeUSAGE_TEXT = 8,
                TreeUSAGE_WINDOW = 9,
                TreeUSAGE_AXIS = 10,
                TreeUSAGE_SUBTREE = 11,
                TreeUSAGE_COMPOUND_DATA = 1;
  
        static {
            try {
              int loaded = 0;
              try {
                java.lang.String value = System.getenv("JavaMdsLib");
                if (value == null) {
                  value = System.getProperty("JavaMdsLib");
                }
                if (value != null) {
                  System.load(value);
                  loaded = 1;
                }
              } catch (Throwable e) {
              }
              if (loaded == 0) {
                System.loadLibrary("JavaMds");
              }
	    }catch(Throwable e)
            {
                  System.out.println("Error loading library javamds: "+e);
                  e.printStackTrace();
            }
	}
    
    
        private int shot;
	private java.lang.String name;
        private java.lang.String mode;
        private int ctx1, ctx2;
        private boolean open = false;
        
	public Tree(java.lang.String name, int shot) throws MdsException
        {
            this.shot = shot;
            this.name = name;
            mode = "NORMAL";
            openTree(name, shot, false);
            open = true;
            
	}

	public Tree(java.lang.String name, int shot, java.lang.String mode) throws MdsException
        {
            this.shot = shot;
            this.name = name;
            java.lang.String upMode = mode.toUpperCase();
            this.mode = upMode;
            if(upMode.equals("NORMAL"))
                openTree(name, shot, false);
            if(upMode.equals("READONLY"))
                openTree(name, shot, true);
            if(upMode.equals("NEW"))
                editTree(name, shot, true);
            if(upMode.equals("EDIT"))
                editTree(name, shot, false);
            open = true;
            
	}

        public void close() throws MdsException
        {
            closeTree(ctx1, ctx2, name, shot);
            open = false;
        }

        protected void finalize() throws Throwable {
            try{
                if (this.isOpen())
                    this.close();
            }finally{
                super.finalize();
            }
        }

        public boolean isOpen() { return open;}
        public java.lang.String toString()
        {
            return("Tree("+ name + ", " +getShot() + ", " + mode + ")");
        }

	/**
	 * Set the oassed tree as the active one
	 * 
	 * @param tree
	 */
	public static void setActiveTree(Tree tree) throws MdsException
        {
            switchDbid(tree.ctx1, tree.ctx2);
	}

        
        static Tree getTree(java.lang.String name, int shot) throws MdsException
        {
            return new Tree(name, shot);
        }
	/**
	 * Returns the currently opened and active tree
	 */
        public java.lang.String getName(){return name;}
        public int getShot()
        {
            if(shot != 0)
                return shot;
            else
                try {
                    return getCurrent();
                }catch(Exception exc){return 0;}
        }
        public java.lang.String getMode() { return mode;}
 	public native static Tree getActiveTree() throws MdsException;
        native void openTree(java.lang.String name, int shot, boolean readonly) throws MdsException;
        native void closeTree(int ctx1, int ctx2, java.lang.String name, int shot) throws MdsException;
        native void editTree(java.lang.String name, int shot, boolean isNew) throws MdsException;
        native static void writeTree(int ctx1, int ctx2, java.lang.String name, int shot) throws MdsException;
        native static void quitTree(int ctx1, int ctx2, java.lang.String name, int shot) throws MdsException;
        static native int findNode(int ctx1, int ctx2, java.lang.String path) throws MdsException;
        static native void switchDbid(int ctx1, int ctx2) throws MdsException;
        static native int [] getWild(int ctx1, int ctx2, java.lang.String path, int usage) throws MdsException;
        static native int getDefaultNid(int ctx1, int ctx2) throws MdsException;
        static native void setDefaultNid(int ctx1, int ctx2, int nid) throws MdsException;
        static native boolean getDbiFlag(int ctx1, int ctx2, int dbiType) throws MdsException;
        static native void setDbiFlag(int ctx1, int ctx2, boolean flag, int dbiType) throws MdsException;
        static native void setTreeViewDate(int ctx1, int ctx2, java.lang.String date) throws MdsException;
        static native void setTreeTimeContext(int ctx1, int ctx2, Data start, Data end, Data delta);
	public static native void setCurrent(java.lang.String name, int shot) throws MdsException;
	public static native int getCurrent(java.lang.String treename) throws MdsException;
        public static native void createPulseFile(int ctx1, int ctx2, int pulseSot) throws MdsException;
        public static native void deletePulseFile(int ctx1, int ctx2, int pulseSot) throws MdsException;
        public static native java.lang.String[] findTreeTags(int ctx1, int ctx2, java.lang.String wild) throws MdsException;
        public static native void  addTreeNode(int ctx1, int ctx2, java.lang.String name, int usage) throws MdsException;
        public static native void addTreeDevice(int ctx1, int ctx2, java.lang.String name, java.lang.String type) throws MdsException;
        public static native void deleteTreeNode(int ctx1, int ctx2, java.lang.String name) throws MdsException;
        public static native void removeTreeTag(int ctx1, int ctx2, java.lang.String tag) throws MdsException;
        public static native long getDatafileSize(int ctx1, int ctx2);
        
        
        public int getCtx1(){return ctx1;}
        public int getCtx2() {return ctx2;}
        
	/**
	 * Return TreeNode for the data item corresponding to the passed pathname
	 * 
	 * @param path
	 */
	public TreeNode getNode(java.lang.String path) throws MdsException
        {
            TreeNode t = new TreeNode(findNode(ctx1, ctx2, path), this);
            t.setCtxTree(this);
            return t;
	}

	/**
	 * Returns the TreeNode for the data  item whose internal address (nid) is passed
	 * as argument.
	 * 
	 * @param nid
	 */
	public TreeNode getNode(int nid) throws MdsException
        {
            return new TreeNode(nid, this);
	}

	/**
	 * Return the TreeNode for the data itemspecified by the passed TreePath instance.
	 * 
	 * @param path
	 */
	public TreeNode getNode(TreePath path) throws MdsException
        {
            return new TreeNode(findNode(ctx1, ctx2, path.getString()), this);
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
            return new TreeNodeArray(getWild(ctx1, ctx2, path, usage), this);
	}
	public TreeNodeArray getNodeWild(java.lang.String path, java.lang.String usage) throws MdsException
        {
            return getNodeWild(path, convertUsage(usage));
	}
	/**
	 * Get the TreeNode for the default node.
	 */
	public TreeNode getDefault() throws MdsException
        {
            return new TreeNode(getDefaultNid(ctx1, ctx2), this);
	}

	/**
	 * Set the default position in the tree.
	 * 
	 * @param node
	 */
	public void setDefault(TreeNode node) throws MdsException
        {
            setDefaultNid(ctx1, ctx2, node.getNid());
	}

	/**
	 * Returns true if the tree supports versioning in model.
	 */
	public boolean versionsInModelEnabled() throws MdsException
        {
            return getDbiFlag(ctx1, ctx2, DbiVERSIONS_IN_MODEL);
	}
	/**
	 * Returns true if the tree supports versioning in pulse.
	 */
	public boolean versionsInPulseEnabled() throws MdsException
        {
            return getDbiFlag(ctx1, ctx2, DbiVERSIONS_IN_PULSE);
	}

        
        public void setVersionsInModel(boolean enabled) throws MdsException
        {
            setDbiFlag(ctx1, ctx2, enabled, DbiVERSIONS_IN_MODEL);
        }
        
        public void setVersionsInPulse(boolean enabled) throws MdsException
        {
            setDbiFlag(ctx1, ctx2, enabled, DbiVERSIONS_IN_MODEL);
        }
        
        public boolean isModified() throws MdsException
        {
            return getDbiFlag(ctx1, ctx2, DbiMODIFIED);
	}
        public boolean isOpenForEdit() throws MdsException
        {
            return getDbiFlag(ctx1, ctx2, DbiOPEN_FOR_EDIT);
	}
        public boolean isReadOnly() throws MdsException
        {
            return getDbiFlag(ctx1, ctx2, DbiOPEN_READONLY);
	}
                
	/**
	 * Set the version date: all read data will refer to the version active to that
	 * date.
	 * 
	 * @param date
	 */
	public void setViewDate(java.util.Date date) throws MdsException
        {
            setTreeViewDate(ctx1, ctx2, ""+date);
	}

	/**
	 * Set the current time context. Only the specified portion of segmented data will
	 * be returned by TreeNode.getData() method
	 * 
	 * @param start
	 * @param end
	 * @param delta
	 */
	public void setTimeContext(Data start, Data end, Data delta) throws MdsException
        {
            setTreeTimeContext(ctx1, ctx2, start, end, delta);
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
        {
            return getCurrent(name);
	}

	/**
	 * Create a new pulse file
	 * 
	 * @param shot
	 */
	public void createPulse(int pulseShot) throws MdsException
        {
            createPulseFile(ctx1, ctx2, pulseShot);
	}

	/**
	 * Delete a pulse file.
	 * 
	 * @param shot
	 */
	public void deletePulse(int pulseShot) throws MdsException
        {
            deletePulseFile(ctx1, ctx2, pulseShot);
	}

	/**
	 * Returns the list of tags matching the specified wild name.
	 * 
	 * @param wild
	 */
	public java.lang.String[] findTags(java.lang.String wild) throws MdsException
        {
            return findTreeTags(ctx1, ctx2, wild);
	}

	/**
	 * Open the tree for editing.
	 */
	public void edit() throws MdsException
        {
            editTree(name, shot, false);
	}

	/**
	 * Write the tree under edit.
	 */
	public void write() throws MdsException
        {
            writeTree(ctx1, ctx2, name, shot);
	}

	/**
	 * quit editing session.
	 */
	public void quit() throws MdsException
        {
            quitTree(ctx1, ctx2, name, shot);
	}

        
        static int convertUsage(java.lang.String usageStr)
        {
            java.lang.String upUsageStr = usageStr.toUpperCase();
            if(upUsageStr.equals("ANY")) return TreeUSAGE_ANY;
            if(upUsageStr.equals("STRUCTURE")) return TreeUSAGE_STRUCTURE;
            if(upUsageStr.equals("ACTION")) return TreeUSAGE_ACTION;
            if(upUsageStr.equals("DEVICE")) return TreeUSAGE_DEVICE;
            if(upUsageStr.equals("DISPATCH")) return TreeUSAGE_DISPATCH;
            if(upUsageStr.equals("NUMERIC")) return TreeUSAGE_NUMERIC;
            if(upUsageStr.equals("SIGNAL")) return TreeUSAGE_SIGNAL;
            if(upUsageStr.equals("TASK")) return TreeUSAGE_TASK;
            if(upUsageStr.equals("TEXT")) return TreeUSAGE_TEXT;
            if(upUsageStr.equals("WINDOW")) return TreeUSAGE_WINDOW;
            if(upUsageStr.equals("AXIS")) return TreeUSAGE_AXIS;
            if(upUsageStr.equals("SUBTREE")) return TreeUSAGE_SUBTREE;
            if(upUsageStr.equals("COMPOUND_DATA")) return TreeUSAGE_COMPOUND_DATA;
            return 0;
        }
	/**
	 * Add a new node to the tree open for edit.
	 * 
	 * @param name
	 * @param usage
	 */
	public void addNode(java.lang.String name, java.lang.String usage) throws MdsException
        {
            addTreeNode(ctx1, ctx2, name, convertUsage(usage));
	}

	/**
	 * Add a new device instance to the tree open for edit.
	 * 
	 * @param name
	 * @param type
	 */
	public void addDevice(java.lang.String name, java.lang.String type) throws MdsException
        {
            addTreeDevice(ctx1, ctx2, name, type);
	}

	/**
	 * Delete the node and its descendants from the tree open for edit.
	 * 
	 * @param name
	 */
	public void deleteNode(java.lang.String name) throws MdsException
        {
            deleteTreeNode(ctx1, ctx2, name);
	}
        
        public void removeTag(java.lang.String tag) throws MdsException
        {
            removeTreeTag(ctx1, ctx2, tag);
        }
        public long getDatafileSize() throws MdsException
        {
            long size = getDatafileSize(ctx1, ctx2);
            if(size == -1)
                throw new MdsException("Cannot get Datafile Size");
            return size;
        }

}
