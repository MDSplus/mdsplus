package MDSplus;

/**
 * CachedTreeNode represents a placeholder for all node-specific operations
 * (including segments) referred to cached data.
 * @author manduchi
 * @version 1.0
 * @created 30-mar-2009 13.40.28
 */
public class CachedTreeNode extends TreeNode 
{
        public static final int WRITE_THROUGH = 1,
            WRITE_BACK = 2,
            WRITE_BUFFER = 3,
            WRITE_LAST = 4;

	/**
	 * Specifies the caching policy, i.e.
	 * WRITE_BACK:data stored in memory until explitly flushed
	 * WRITE_THROUGH:data stored on disk by a separate thead
	 * WRITE_BUFFER:to be used when appending rows individually: keep segment data in
	 * memory and flushes the segment once it has been filled.
	 */
	private int cachePolicy;
        static native void treeCacheFlush(int ctx1, int ctx2, int nid) throws MdsException;
        
	public CachedTreeNode(TreeNode treeNode) throws MdsException
        {
            super(treeNode.nid, treeNode.tree);
	}

        protected boolean isCached() {return true;}
	public int getCachePolicy()
        {
		return cachePolicy;
	}

	/**
	 * 
	 * @param policy
	 */
	public void setCachePolicy(int policy)
        {
            this.cachePolicy = cachePolicy;
	}

	/**
	 * Flush cached data on disk.
	 */
	public void flush() throws MdsException
        {
            treeCacheFlush(tree.getCtx1(), tree.getCtx2(), nid);
	}

	/**
	 * Write the last row, thus forcing writing the (possibly partially filled)
	 * segment to disk.
	 * 
	 * @param time
	 * @param data
	 */
	public void putLastRow(Data row, long time) throws MdsException
        {
            putRow(nid, tree.getCtx1(), tree.getCtx2(), row, time, 1024, true, WRITE_LAST);
	}
}