package MDSplus;

/**
 * Class TreeNode holds all the node-specific information.
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.50
 */
public class TreeNode extends Data
{

	protected Tree tree;
        protected int nid;
        

        static final int NciM_STATE = 0x00000001,
            NciM_PARENT_STATE = 0x00000002,
            NciM_ESSENTIAL = 0x00000004,
            NciM_CACHED = 0x00000008,
            NciM_VERSIONS = 0x00000010,
            NciM_SEGMENTED = 0x00000020,
            NciM_SETUP_INFORMATION = 0x00000040,
            NciM_WRITE_ONCE = 0x00000080,
            NciM_COMPRESSIBLE = 0x00000100,
            NciM_DO_NOT_COMPRESS = 0x00000200,
            NciM_COMPRESS_ON_PUT = 0x00000400,
            NciM_NO_WRITE_MODEL = 0x00000800,
            NciM_NO_WRITE_SHOT = 0x00001000,
            NciM_PATH_REFERENCE = 0x00002000,
            NciM_NID_REFERENCE = 0x00004000,
            NciM_INCLUDE_IN_PULSE = 0x00008000;
        
        
        static final int  NciTIME_INSERTED = 4,
            NciOWNER_ID = 5,
            NciCLASS = 6,
            NciDTYPE = 7,
            NciLENGTH = 8,
            NciSTATUS = 9,
            NciCONGLOMERATE_ELT = 10,
            NciGET_FLAGS = 12,
            NciNODE_NAME =  13,
            NciPATH = 14,
            NciDEPTH =  15,
            NciPARENT = 16,
            NciBROTHER = 17,
            NciMEMBER = 18,
            NciCHILD =  19,
            NciPARENT_RELATIONSHIP = 20,
              NciK_IS_CHILD = 1,
              NciK_IS_MEMBER = 2,
            NciCONGLOMERATE_NIDS = 21,
            NciORIGINAL_PART_NAME = 22,
            NciNUMBER_OF_MEMBERS = 23,
            NciNUMBER_OF_CHILDREN = 24,
            NciMEMBER_NIDS = 25,
            NciCHILDREN_NIDS = 26,
            NciFULLPATH = 27,
            NciMINPATH = 28,
            NciUSAGE = 29,
            NciPARENT_TREE = 30,
            NciRLENGTH = 31,
            NciNUMBER_OF_ELTS = 32,
            NciDATA_IN_NCI = 33,
            NciERROR_ON_PUT = 34,
            NciRFA = 35,
            NciIO_STATUS = 36,
            NciIO_STV = 37,
            NciDTYPE_STR = 38,
            NciUSAGE_STR = 39,
            NciCLASS_STR = 40,
            NciVERSION = 41;       
 
        static final int TreeUSAGE_ANY = 0,
            TreeUSAGE_STRUCTURE  = 1,
            TreeUSAGE_ACTION   =   2,
            TreeUSAGE_DEVICE    =  3,
            TreeUSAGE_DISPATCH  =  4,
            TreeUSAGE_NUMERIC  =   5,
            TreeUSAGE_SIGNAL   =   6,
            TreeUSAGE_TASK     =   7,
            TreeUSAGE_TEXT     =   8,
            TreeUSAGE_WINDOW   =   9,
            TreeUSAGE_AXIS    =   10,
            TreeUSAGE_SUBTREE  =  11,
            TreeUSAGE_COMPOUND_DATA =  12;

        
        
        public TreeNode(int nid, Data help, Data units, Data error, Data validation) throws MdsException
        {
            super(help, units, error, validation);
            clazz = CLASS_S;
            dtype = DTYPE_NID;
            this.nid = nid;
            tree = Tree.getActiveTree();
	}
        public TreeNode(Data help, Data units, Data error, Data validation) throws MdsException
        {
            super(help, units, error, validation);
            clazz = CLASS_S;
            dtype = DTYPE_NID;
            tree = Tree.getActiveTree();
	}
	public TreeNode(int nid, Tree tree, Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_S;
            dtype = DTYPE_NID;
            this.nid = nid;
            this.tree = tree;
	}

 	public TreeNode(int nid, Tree tree) throws MdsException
        {
            this(nid, tree, null, null, null, null);
	}
 	public TreeNode(int nid)  throws MdsException
        {
            this(nid, null, null, null, null);
	}

        public static TreeNode getData(int nid) throws MdsException
        {
            return getData(nid, null, null, null, null);
        }
        public static TreeNode getData(int nid,  Data help, Data units, Data error, Data validation) throws MdsException
        {
            return new TreeNode(nid,  help,  units, error, validation);
        }
        
        protected void resolveNid() throws MdsException {}
        static native int getNci(int nid, int ctx1, int ctx2, int nciType) throws MdsException;
        static native long getNciLong(int nid, int ctx1, int ctx2, int nciType) throws MdsException;
        static native java.lang.String getNciString(int nid, int ctx1, int ctx2, int nciType) throws MdsException;
        static native void setNciFlag(int nid, int ctx1, int ctx2, int flagType, boolean flag) throws MdsException;
        static native boolean getNciFlag(int nid, int ctx1, int ctx2, int flagType) throws MdsException;
        static native int[]getNciNids(int nid, int ctx1, int ctx2, int nciNumType, int nciType) throws MdsException;
        static native void turnOn(int nid, int ctx1, int ctx2, boolean on) throws MdsException;
        static native boolean isOn(int nid, int ctx1, int ctx2) throws MdsException;
        static native Data getExtendedAttribute(int nid, int ctx1, int ctx2, java.lang.String name) throws MdsException;
        static native void setExtendedAttribute(int nid, int ctx1, int ctx2, java.lang.String name, Data data) throws MdsException;
        static native Data getData(int nid, int ctx1, int ctx2) throws MdsException;
        static native void putData(int nid, int ctx1, int ctx2, Data data) throws MdsException;
        static native void deleteData(int nid, int ctx1, int ctx2) throws MdsException;
        static native void doMethod(int nid, int ctx1, int ctx2, java.lang.String method) throws MdsException; 
        static native java.lang.String[] getTags(int nid, int ctx1, int ctx2) throws MdsException;
        static native void beginSegment(int nid, int ctx1, int ctx2, Data start, Data end, Data dim, Data initData)throws MdsException;
        static native void makeSegment(int nid, int ctx1, int ctx2, Data start, Data end, Data dim, Data initData, int filledRows)throws MdsException;
        static native void putSegment(int nid, int ctx1, int ctx2, Data data, int  offset)throws MdsException;
        static native void updateSegment(int nid, int ctx1, int ctx2, Data start, Data end, Data dim)throws MdsException;
        static native void beginTimestampedSegment(int nid, int ctx1, int ctx2,Data initData)throws MdsException;
        static native void putTimestampedSegment(int nid, int ctx1, int ctx2,Data data, long times[])throws MdsException;
        static native void makeTimestampedSegment(int nid, int ctx1, int ctx2,Data data, long times[])throws MdsException;
        static native void putRow(int nid, int ctx1, int ctx2, Data row, long time, int size)throws MdsException;
        static native int getNumSegments(int nid, int ctx1, int ctx2)throws MdsException;
        static native void acceptSegment(int nid, int ctx1, int ctx2, Data seg, Data time)throws MdsException;
        static native void acceptRow(int nid, int ctx1, int ctx2, Data seg, long time)throws MdsException;
        static native Data getSegmentStart(int nid, int ctx1, int ctx2, int idx)throws MdsException;
        static native Data getSegmentEnd(int nid, int ctx1, int ctx2, int idx)throws MdsException;
        static native Data getSegmentDim(int nid, int ctx1, int ctx2, int idx)throws MdsException;
        static native Data getSegment(int nid, int ctx1, int ctx2, int idx)throws MdsException;
        static native int addNode(int nid, int ctx1, int ctx2, java.lang.String name,  int usage)throws MdsException;
        static native void deleteNode(int nid, int ctx1, int ctx2,  java.lang.String name)throws MdsException;
        static native void renameNode(int nid, int ctx1, int ctx2,  java.lang.String newName)throws MdsException;
        static native void addTag(int nid, int ctx1, int ctx2,  java.lang.String tagname)throws MdsException;
        static native void removeTag(int nid, int ctx1, int ctx2,  java.lang.String tagname)throws MdsException;
        static native int addDevice(int nid, int ctx1, int ctx2,  java.lang.String name,  java.lang.String usage)throws MdsException;
        static native void setSubtree(int nid, int ctx1, int ctx2, boolean isSubtree);
        static native void moveNode(int nid, int ctx1, int ctx2, int parentNid, java.lang.String newName);
        
        /**
	 * Return true if data has to be comressed when written.
	 */
	public boolean isCompressOnPut() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_COMPRESS_ON_PUT);
	}

	/**
	 * Enable compression then writing data
	 * 
	 * @param flag
	 */
	public void setCompressOnPut(boolean flag) throws MdsException
        {
            resolveNid();
            setNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_COMPRESS_ON_PUT, flag);
	}

	/**
	 * Get index of the node in the corresponding conglomerate
	 */
	public int getConglomerateElt() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciCONGLOMERATE_ELT);
	}

	/**
	 * Return the number of the elements for the conglomerate to which the node belongs
	 */
	public int getNumElts() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_ELTS);
	}

	/**
	 * Return the array of nodes corresponding to the elements of the conglomeate to
	 * which the node belongs
	 */
	public TreeNodeArray getConglomerateNodes() throws MdsException
        {
            resolveNid();
            return new TreeNodeArray(getNciNids(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_ELTS, NciCONGLOMERATE_ELT), tree); 
	}

        /**
	 * Return the original path within the conglomerate to thich the node belongs
	 */
	public java.lang.String getOriginalPartName() throws MdsException
        {
            resolveNid();
            return getNciString(nid, tree.getCtx1(), tree.getCtx2(), NciORIGINAL_PART_NAME);
	}

	/**
	 * eeturn data type as a java.lang.String
	 */
	public java.lang.String getDtype() throws MdsException
        {
            resolveNid();
            return getNciString(nid, tree.getCtx1(), tree.getCtx2(), NciDTYPE_STR);
	}

	/**
	 * Return true id the node is essential (action necessary in dispatching)
	 */
	public boolean isEssential() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_ESSENTIAL);
	}

	/**
	 * Set essential flag
	 * 
	 * @param flag
	 */
	public void setEssential(boolean flag) throws MdsException
        {
            resolveNid();
            setNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_ESSENTIAL, flag);
	}

	/**
	 * Get the full node pathname
	 */
	public java.lang.String getFullPath() throws MdsException
        {
            resolveNid();
            return getNciString(nid, tree.getCtx1(), tree.getCtx2(), NciFULLPATH);
	}

	/**
	 * Get the minimum path name for this node
	 */
	public java.lang.String getMinPath() throws MdsException
        {
            resolveNid();
            return getNciString(nid, tree.getCtx1(), tree.getCtx2(), NciMINPATH);
	}

	/**
	 * Get the path name for this node
	 */
	public java.lang.String getPath() throws MdsException
        {
            resolveNid();
            return getNciString(nid, tree.getCtx1(), tree.getCtx2(), NciPATH);
	}

	/**
	 * Get the name of this node (not its path)
	 */
	public java.lang.String getNodeName() throws MdsException
        {
            resolveNid();
            return getNciString(nid, tree.getCtx1(), tree.getCtx2(), NciNODE_NAME);
	}

	/**
	 * Return true if the underlying subtree is included in the pulse file
	 */
	public boolean isIncludedInPulse() throws MdsException
	{
        return isIncludeInPulse();
    }
	public boolean isIncludeInPulse() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_INCLUDE_IN_PULSE);
	}

	/**
	 * Set includeInPulse flag
	 * 
	 * @param flag
	 */
	public void setIncludedInPulse(boolean flag) throws MdsException
	{
        setIncludeInPulse(flag);
    }
	public void setIncludeInPulse(boolean flag) throws MdsException
    {
            resolveNid();
            setNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_INCLUDE_IN_PULSE, flag);
	}

	/**
	 * Return the depth of the node in the tree
	 */
	public int getDepth() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciDEPTH);
	}

	/**
	 * Return true is the node is a child node
	 */
	public boolean isChild() throws MdsException
        {
            resolveNid();
            int flags = getNci(nid, tree.getCtx1(), tree.getCtx2(), NciPARENT_RELATIONSHIP);
            return (flags & NciK_IS_CHILD) != 0;
	}

	/**
	 * Get the first child of this node
	 */
	public TreeNode getChild() throws MdsException
        {
            resolveNid();
            return new TreeNode(getNci(nid, tree.getCtx1(), tree.getCtx2(), NciCHILD), tree);
	}

	/**
	 * Get the number of children for this node
	 */
	public int getNumChildren() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_CHILDREN);
	}

	/**
	 * Get all che child nodes for this node
	 */
	public TreeNodeArray getChildren() throws MdsException
        {
            resolveNid();
            return new TreeNodeArray(getNciNids(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_CHILDREN, NciCHILDREN_NIDS), tree); 
	}

	/**
	 * Return true id the node is a member node
	 */
	public boolean isMember() throws MdsException
        {
            resolveNid();
            int flags = getNci(nid, tree.getCtx1(), tree.getCtx2(), NciPARENT_RELATIONSHIP);
            return (flags & NciK_IS_MEMBER) != 0;
	}

	/**
	 * Get the first member of this node
	 */
	public TreeNode getMember() throws MdsException
        {
            resolveNid();
            return new TreeNode(getNci(nid, tree.getCtx1(), tree.getCtx2(), NciMEMBER), tree);
	}

	/**
	 * Get the number of members for this node
	 */
	public int getNumMembers() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_MEMBERS);
	}

	/**
	 * Return  all the members of this node
	 */
	public TreeNodeArray getMembers() throws MdsException
        {
            resolveNid();
            return new TreeNodeArray(getNciNids(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_MEMBERS, NciMEMBER_NIDS), tree); 
	}

	/**
	 * Get the number of all descendants (members + children)fir this node
	 */
	public int getNumDescendants() throws MdsException
        {
		return getNumMembers() + getNumChildren();
	}

	/**
	 * Get all the descendant (members + children)for this node
	 */
	public TreeNodeArray getDescendants() throws MdsException
        {
            resolveNid();
            int [] memberNids = getNciNids(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_MEMBERS, NciMEMBER_NIDS);
            int [] childrenNids = getNciNids(nid, tree.getCtx1(), tree.getCtx2(), NciNUMBER_OF_CHILDREN, NciCHILDREN_NIDS);
            int descendantNids[] = new int[memberNids.length + childrenNids.length];
            for(int i = 0; i < memberNids.length; i++)
                descendantNids[i] = memberNids[i];
            for(int i = 0; i < childrenNids.length; i++)
                descendantNids[memberNids.length+i] = childrenNids[i];
            return new TreeNodeArray(descendantNids, tree);
	}

	/**
	 * Get the parent of this node
	 */
	public TreeNode getParent() throws MdsException
        {
            resolveNid();
            return new TreeNode(getNci(nid, tree.getCtx1(), tree.getCtx2(), NciPARENT), tree);
	}

	/**
	 * Return next sibling.
	 */
	public TreeNode getBrother() throws MdsException
        {
            resolveNid();
            return new TreeNode(getNci(nid, tree.getCtx1(), tree.getCtx2(), NciBROTHER), tree);
	}

	/**
	 * Return the length in bytes of the contained data
	 */
	public int getLength() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciLENGTH);
	}

	/**
	 * Return the size in bytes of  (possibly compressed) associated data
	 */
	public int getCompressedLength() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciRLENGTH);
	}

	/**
	 * Get Node Identifier
	 */
	public int getNid() throws MdsException
        {
            resolveNid();
            return nid;
	}

	/**
	 * Return true if this node cannot be written in the model
	 */
	public boolean isNoWriteModel() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_NO_WRITE_MODEL);
	}

	/**
	 * Set NoWriteModel flag
	 * 
	 * @param noWriteModel
	 */
	public void setNoWriteModel(boolean noWriteModel) throws MdsException
        {
            resolveNid();
            setNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_NO_WRITE_MODEL, noWriteModel);
	}

	/**
	 * Return true if this node cannot be written in pulse files
	 */
	public boolean isNoWriteShot() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_NO_WRITE_SHOT);
	}

	/**
	 * 
	 * @param noWriteShot
	 */
	public void setNoWriteShot(boolean noWriteShot) throws MdsException
        {
            resolveNid();
            setNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_NO_WRITE_SHOT, noWriteShot);
	}

	/**
	 * Return true if it is only allowed to write once on this node
	 */
	public boolean isWriteOnce() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_WRITE_ONCE);
	}

	/**
	 * Set WriteOnce flag
	 * 
	 * @param writeOnce
	 */
	public void setWriteOnce(boolean writeOnce) throws MdsException
        {
            resolveNid();
            setNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_WRITE_ONCE, writeOnce);
	}

	/**
	 * Return true if the node is On
	 */
	public boolean isOn() throws MdsException
        {
            resolveNid();
            return isOn(nid, tree.getCtx1(), tree.getCtx2());
	}

	/**
	 * Set Extended Attribute
	*/
	public void setExtendedAttribute(java.lang.String name, Data val) throws MdsException
	{
	    resolveNid();
	    setExtendedAttribute(nid, tree.getCtx1(), tree.getCtx2(), name, val);
	}

	/**
	 * Get Extended Attribute
	*/
	public Data getExtendedAttribute(java.lang.String name) throws MdsException
	{
	    resolveNid();
	    return getExtendedAttribute(nid, tree.getCtx1(), tree.getCtx2(), name);
	}

	/**
	 * Set on/off for that node
	 * 
	 * @param on
	 */
	public void setOn(boolean on) throws MdsException
        {
            resolveNid();
            turnOn(nid, tree.getCtx1(), tree.getCtx2(), on);
	}

	/**
	 * Return the Group/Id of the last writer for that node
	 */
	public int getOwnerId() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciOWNER_ID);
	}

	/**
	 * Get Data for this node
	 */
	public Data getData() throws MdsException
        {
            resolveNid();
            Data  data = getData(nid, tree.getCtx1(), tree.getCtx2());
            data.setCtxTree(tree);
            return data;
	}

	/**
	 * Write data for this node
	 * 
	 * @param data
	 */
	public void putData(Data data) throws MdsException
        {
            resolveNid();
            putData(nid, tree.getCtx1(), tree.getCtx2(), data);
	}

	/**
	 * Delete data associaed with this node
	 */
	public void deleteData() throws MdsException
        {
            resolveNid();
            deleteData(nid, tree.getCtx1(), tree.getCtx2());
	}

	/**
	 * Get the time of the last data insertion
	 */
	public long getTimeInserted() throws MdsException
        {
            resolveNid();
            return getNciLong(nid, tree.getCtx1(), tree.getCtx2(), NciTIME_INSERTED);
	}

	/**
	 * Do specified method for this node (valid only if it belongs to a conglomerae)
	 * 
	 * @param method
	 */
	public void doMethod(java.lang.String method) throws MdsException
        {
            resolveNid();
            doMethod(nid, tree.getCtx1(), tree.getCtx2(), method);
	}

	/**
	 * Return true if this is setup data (i.e. present in the the model)
	 */
	public boolean isSetup() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciM_SETUP_INFORMATION);
        }

	/**
	 * Return the status of the completed action (if the node contains ActionData)
	 */
	public int getStatus() throws MdsException
        {
            resolveNid();
            return getNci(nid, tree.getCtx1(), tree.getCtx2(), NciSTATUS);
	}

	/**
	 * Get the node usage as a string
	 */
	public java.lang.String getUsage() throws MdsException
        {
            resolveNid();
            int usage = getNci(nid, tree.getCtx1(), tree.getCtx2(), NciUSAGE);
            switch(usage)  {
		case TreeUSAGE_ACTION: return "ACTION";
		case TreeUSAGE_ANY: return "ANY";
		case TreeUSAGE_AXIS: return "AXIS";
		case TreeUSAGE_COMPOUND_DATA: return "COMPOUND_DATA";
		case TreeUSAGE_DEVICE: return "DEVICE";
		case TreeUSAGE_DISPATCH: return "DISPATCH";
		case TreeUSAGE_STRUCTURE: return "STRUCTURE";
		case TreeUSAGE_NUMERIC: return "NUMERIC";
		case TreeUSAGE_SIGNAL: return "SIGNAL";
		case TreeUSAGE_SUBTREE: return "SUBTREE";
		case TreeUSAGE_TASK: return "TASK";
		case TreeUSAGE_TEXT: return "TEXT";
		case TreeUSAGE_WINDOW: return "WINDOW";
            }
            return "";
	}
        
        
	/**
	 * Get the (list of) tags for this node
	 */
	public java.lang.String[] findTags() throws MdsException
        {
            resolveNid();
            return getTags(nid, tree.getCtx1(), tree.getCtx2());
	}

	/**
	 * Return true if the node contains versions
	 */
	public boolean containsVersions() throws MdsException
        {
            resolveNid();
            return getNciFlag(nid, tree.getCtx1(), tree.getCtx2(), NciVERSION);
	}

	/**
	 * Return true if the node contains segmented data
	 */
	public boolean isSegmented() throws MdsException
        {
            return getNumSegments() == 0;
	}

	/**
	 * Begin a new data segment
	 * 
	 * @param start
	 * @param end
	 * @param dim
	 * @param initData
	 */
	public void beginSegment(Data start, Data end, Data dim, Array initData) throws MdsException
        {
            resolveNid();
            beginSegment(nid, tree.getCtx1(), tree.getCtx2(), start, end, dim, initData);
	}
	public void makeSegment(Data start, Data end, Data dim, Array initData) throws MdsException
        {
            resolveNid();
            makeSegment(nid, tree.getCtx1(), tree.getCtx2(), start, end, dim, initData, dim.getShape()[0]);
	}

	/**
	 * Write (part of) data segment
	 * 
	 * @param data
	 * @param offset
	 */
	public void putSegment(Array data, int offset) throws MdsException
        {
            resolveNid();
            putSegment(nid, tree.getCtx1(), tree.getCtx2(), data, offset);
	}

	/**
	 * Update start, end time and dimension for the specified segment
	 * 
	 * @param start
	 * @param end
	 * @param dim
	 */
	public void updateSegment(Data start, Data end, Data dim) throws MdsException
        {
            resolveNid();
            updateSegment(nid, tree.getCtx1(), tree.getCtx2(), start, end, dim);
	}

	/**
	 * Begin a timestamted segment
	 * 
	 * @param initData
	 */
	public void beginTimestampedSegment(Array initData) throws MdsException
        {
            resolveNid();
            beginTimestampedSegment(nid, tree.getCtx1(), tree.getCtx2(),initData);
	}

	/**
	 * Write (part of)data in a timestamped segment
	 * 
	 * @param data
	 * @param times
	 */
	public void putTimestampedSegment(Data data, long times[]) throws MdsException
        {
            resolveNid();
            putTimestampedSegment(nid, tree.getCtx1(), tree.getCtx2(),data, times);
	}
	public void makeTimestampedSegment(Data data, long times[]) throws MdsException
        {
            resolveNid();
            makeTimestampedSegment(nid, tree.getCtx1(), tree.getCtx2(),data, times);
	}

	/**
	 * Writre a single row of timestamped data
	 * 
	 * @param row
	 * @param time
	 */
        
	public void putRow(Data row, long time, int size) throws MdsException
        {
            resolveNid();
            putRow(nid, tree.getCtx1(), tree.getCtx2(), row, time, size);
	}

        public void putRow(Data row, long time) throws MdsException
        {
            putRow(row, time, 1024);
        }
        
        
	/**
	 * Get the num,ber of segments
	 */
	public int getNumSegments() throws MdsException
        {
            resolveNid();
            return getNumSegments(nid, tree.getCtx1(), tree.getCtx2());
	}

	/**
	 * Get start time fopr that segment
	 * 
	 * @param idx
	 */
	public Data getSegmentStart(int idx) throws MdsException
        {
            resolveNid();
            return getSegmentStart(nid, tree.getCtx1(), tree.getCtx2(), idx);
	}

	/**
	 * Get end time for that segment
	 * 
	 * @param idx
	 */
	public Data getSegmentEnd(int idx) throws MdsException
        {
            resolveNid();
            return getSegmentEnd(nid, tree.getCtx1(), tree.getCtx2(), idx);
	}

	/**
	 * 
	 * @param idx
	 */
	public Data getSegmentDim(int idx) throws MdsException
        {
            resolveNid();
            return getSegmentDim(nid, tree.getCtx1(), tree.getCtx2(), idx);
	}

	/**
	 * Get data form the selectede segment
	 * 
	 * @param idx
	 */
	public Array getSegment(int idx) throws MdsException
        {
            resolveNid();
            return (Array)getSegment(nid, tree.getCtx1(), tree.getCtx2(), idx);
	}

	/**
	 * Get the associated tree instance
	 */
	public Tree getTree()
        {
            return tree;
	}

	/**
	 * set the associated Tree instance
	 * 
	 * @param tree
	 */
	public void setTree(Tree tree)
        {
            this.tree = tree;
	}

	/**
	 * Add a node to the tree open for edit. The name is relative to this node
	 * instance.
	 * 
	 * @param name
	 * @param usage
	 */
	public TreeNode addNode(java.lang.String name, java.lang.String usage) throws MdsException
        {
            resolveNid();
            int newNid = addNode(nid, tree.getCtx1(), tree.getCtx2(), name, Tree.convertUsage(usage));
            return new TreeNode(newNid, tree);
	}

	/**
	 * Removes a node and its descendands from the tree open for edit. The name is
	 * relative to this node instance.
	 * 
	 * @param name
	 */
	public void remove(java.lang.String name) throws MdsException
        {
            resolveNid();
            deleteNode(nid, tree.getCtx1(), tree.getCtx2(), name);
	}

	/**
	 * Rename a node in edit.
	 * 
	 * @param oldName
	 * @param newName
	 */
	public void rename(java.lang.String newName) throws MdsException
        {
            resolveNid();
            renameNode(nid, tree.getCtx1(), tree.getCtx2(), newName);
	}

        public void move(TreeNode parent, java.lang.String newName) throws MdsException
        {
            resolveNid();
            moveNode(nid, tree.getCtx1(), tree.getCtx2(), parent.nid, newName);
        }
        public void move(TreeNode parent) throws MdsException
        {
            resolveNid();
            moveNode(nid, tree.getCtx1(), tree.getCtx2(), parent.nid, getFullPath());
        }
        
	/**
	 * Add a tag to this node in the tree open for edit.
	 * 
	 * @param tagName
	 */
	public void addTag(java.lang.String tagName) throws MdsException
        {
            resolveNid();
            addTag(nid, tree.getCtx1(), tree.getCtx2(), tagName);

	}

	/**
	 * Remove a tag to this node in the tree open for edit.
	 * 
	 * @param tagName
	 */
	public void removeTag(java.lang.String tagName) throws MdsException
        {
            resolveNid();
            removeTag(nid, tree.getCtx1(), tree.getCtx2(), tagName);
	}

	/**
	 * Add a devive intance to the tree open for edit. The name is relative to this
	 * node instance.
	 * 
	 * @param name
	 * @param usage
	 */
	public TreeNode addDevice(java.lang.String name, java.lang.String type) throws MdsException
        {
            resolveNid();
            int devNid = addDevice(nid, tree.getCtx1(), tree.getCtx2(), name, type);
            return new TreeNode(devNid, tree);
	}

	/**
	 * Derclare this node to be a reference to a (separate) subtree in the tree open
	 * for edit.
	 * 
	 * @param isSubtree
	 */
	public void setSubtree(boolean isSubtree) throws MdsException
        {
            resolveNid();
            setSubtree(nid, tree.getCtx1(), tree.getCtx2(), isSubtree);
	}

        public void putData(byte d) throws MdsException {putData(new Int8(d));}
        public void putData(short d) throws MdsException {putData(new Int16(d));}
        public void putData(int d) throws MdsException {putData(new Int32(d));}
        public void putData(long d) throws MdsException {putData(new Int64(d));}
        public void putData(float d) throws MdsException {putData(new Float32(d));}
        public void putData(double d) throws MdsException {putData(new Float64(d));}
        public void putData(java.lang.String d) throws MdsException {putData(new MDSplus.String(d));}
        public void putData(byte []d) throws MdsException {putData(new Int8Array(d));}
        public void putData(short [] d) throws MdsException {putData(new Int16Array(d));}
        public void putData(int []d) throws MdsException {putData(new Int32Array(d));}
        public void putData(long []d) throws MdsException {putData(new Int64Array(d));}
        public void putData(float []d) throws MdsException {putData(new Float32Array(d));}
        public void putData(double []d) throws MdsException {putData(new Float64Array(d));}
        public void putData(java.lang.String []d) throws MdsException {putData(new MDSplus.StringArray(d));}
}