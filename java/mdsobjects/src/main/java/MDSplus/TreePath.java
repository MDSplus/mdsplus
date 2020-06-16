/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package MDSplus;

/**
 *
 * @author manduchi
 */
public class TreePath extends TreeNode
{
	java.lang.String path;

	public TreePath(java.lang.String path, Data help, Data units, Data error, Data validation) throws MdsException
	{
		super(help, units, error, validation);
		clazz = CLASS_S;
		dtype = DTYPE_PATH;
		this.path = path;
	}

	public TreePath(java.lang.String path, Tree tree, Data help, Data units, Data error, Data validation)
			throws MdsException
	{
		super(help, units, error, validation);
		clazz = CLASS_S;
		dtype = DTYPE_PATH;
		this.path = path;
		this.ctxTree = tree;
	}

	public TreePath(java.lang.String path, Tree tree) throws MdsException
	{
		this(path, tree, null, null, null, null);
	}

	public TreePath(java.lang.String path) throws MdsException
	{
		this(path, null, null, null, null);
	}

	public static Data getData(java.lang.String path, Data help, Data units, Data error, Data validation)
			throws MdsException
	{
		return new TreePath(path, help, units, error, validation);
	}

	@Override
	public java.lang.String getString()
	{ return path; }

	@Override
	protected void resolveNid() throws MdsException
	{
		nid = Tree.findNode(ctxTree.getCtx(), path);
	}
}
