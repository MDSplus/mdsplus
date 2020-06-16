/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package MDSplus;

/**
 *
 * @author manduchi
 */
public class TreeNodeArray
{
	TreeNode[] treeNodes;
	Tree tree;

	public TreeNodeArray(int[] nids, Tree tree) throws MdsException
	{
		treeNodes = new TreeNode[nids.length];
		for (int i = 0; i < nids.length; i++)
			treeNodes[i] = new TreeNode(nids[i], tree);
	}

	public int size()
	{
		if (treeNodes == null)
			return 0;
		return treeNodes.length;
	}

	public TreeNode getElementAt(int idx)
	{
		return treeNodes[idx];
	}

	public java.lang.String[] getPath() throws MdsException
	{
		final java.lang.String path[] = new java.lang.String[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			path[i] = treeNodes[i].getPath();
		return path;
	}

	public java.lang.String[] getFullPath() throws MdsException
	{
		final java.lang.String path[] = new java.lang.String[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			path[i] = treeNodes[i].getFullPath();
		return path;
	}

	public int[] getNid() throws MdsException
	{
		final int nids[] = new int[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			nids[i] = treeNodes[i].getNid();
		return nids;
	}

	public boolean[] isOn() throws MdsException
	{
		final boolean on[] = new boolean[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			on[i] = treeNodes[i].isOn();
		return on;
	}

	public void setOn(boolean[] on) throws MdsException
	{
		for (int i = 0; i < treeNodes.length; i++)
			treeNodes[i].setOn(on[i]);
	}

	public int[] getLength() throws MdsException
	{
		final int sizes[] = new int[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			sizes[i] = treeNodes[i].getLength();
		return sizes;
	}

	public int[] getCompressedLength() throws MdsException
	{
		final int sizes[] = new int[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			sizes[i] = treeNodes[i].getCompressedLength();
		return sizes;
	}

	public boolean[] isSetup() throws MdsException
	{
		final boolean setup[] = new boolean[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			setup[i] = treeNodes[i].isSetup();
		return setup;
	}

	public boolean[] isWriteOnce() throws MdsException
	{
		final boolean wonce[] = new boolean[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			wonce[i] = treeNodes[i].isWriteOnce();
		return wonce;
	}

	public void setWriteOnce(boolean[] wonce) throws MdsException
	{
		for (int i = 0; i < treeNodes.length; i++)
			treeNodes[i].setWriteOnce(wonce[i]);
	}

	public boolean[] isCompressOnPut() throws MdsException
	{
		final boolean compressible[] = new boolean[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			compressible[i] = treeNodes[i].isCompressOnPut();
		return compressible;
	}

	public void setCompressOnPut(boolean[] compress) throws MdsException
	{
		for (int i = 0; i < treeNodes.length; i++)
			treeNodes[i].setCompressOnPut(compress[i]);
	}

	public boolean[] isNoWriteModel() throws MdsException
	{
		final boolean wmodel[] = new boolean[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			wmodel[i] = treeNodes[i].isNoWriteModel();
		return wmodel;
	}

	public void isNoWriteModel(boolean[] wmodel) throws MdsException
	{
		for (int i = 0; i < treeNodes.length; i++)
			treeNodes[i].setNoWriteModel(wmodel[i]);
	}

	public boolean[] isNoWriteShot() throws MdsException
	{
		final boolean wshot[] = new boolean[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			wshot[i] = treeNodes[i].isNoWriteShot();
		return wshot;
	}

	public void isNoWriteShot(boolean[] wshot) throws MdsException
	{
		for (int i = 0; i < treeNodes.length; i++)
			treeNodes[i].setNoWriteShot(wshot[i]);
	}

	public java.lang.String[] getUsage() throws MdsException
	{
		final java.lang.String usages[] = new java.lang.String[treeNodes.length];
		for (int i = 0; i < treeNodes.length; i++)
			usages[i] = treeNodes[i].getUsage();
		return usages;
	}
}
