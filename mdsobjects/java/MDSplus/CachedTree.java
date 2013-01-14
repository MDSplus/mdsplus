package MDSplus;

/**
 * Class CachedTree describes a tree whose node contends can be mapped on memory.
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.22
 */
public class CachedTree extends Tree 
{
        native void cachedTreeOpen(java.lang.String name, int shot) throws MdsException;
        native static void cacheConfigure(boolean isShared, int cacheDim) throws MdsException;
        native static void cacheSynch() throws MdsException;
    
	public CachedTree(java.lang.String name, int shot, boolean isShared, int cacheDim) throws MdsException
        {
            super(name, shot);
            cacheConfigure(isShared, cacheDim);
            cachedTreeOpen(name, shot);
        }
	public CachedTree(java.lang.String name, int shot) throws MdsException
        {
            this(name, shot, false, 1000000);
	}

	public CachedTree(java.lang.String name, int shot, int size) throws MdsException
        {
            this(name, shot, false, size);
	}

	/**
	 * Configure cache memory, in particular:
	 * 
	 * - whether the memory is shared or private
	 * - the size of the allocated memory (is shared, this is valid only for the first
	 * memory instantiation)
	 * 
	 * @param size
	 * @param shared
	 */
	public void configure(int size, boolean shared) throws MdsException
        {
            cacheConfigure(shared, size);
	}

	/**
	 * Get an instance of tree node for which caching data in memory is enabled.
	 * 
	 * @param path
	 */
	public CachedTreeNode getCachedNode(java.lang.String path) throws MdsException
        {
            return new CachedTreeNode(getNode(path));
	}

	/**
	 * Get an instance of CachedTreeNode for that tree node, based on the passed nid.
	 * 
	 * @param nid
	 */
	public CachedTreeNode getCachedNode(int nid) throws MdsException
        {
            return new CachedTreeNode(new TreeNode(nid, this));
	}

	/**
	 * Wait for all pending flush operations.
	 */
	public void synch() throws MdsException
        {
            cacheSynch();
	}
}