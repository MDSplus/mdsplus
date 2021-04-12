package mds.jtraverser;

//package jTraverser;
import javax.swing.JPanel;

public class NodeEditor extends JPanel
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	Node node;
	Tree tree;
	TreeDialog frame;

	public void setFrame(TreeDialog frame)
	{ this.frame = frame; }

	public void setNode(Node node)
	{ this.node = node; }

	public void setTree(Tree tree)
	{ this.tree = tree; }
}
