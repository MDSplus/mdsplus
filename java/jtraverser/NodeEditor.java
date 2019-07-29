//package jTraverser;
import javax.swing.JPanel;

public class NodeEditor extends JPanel
{
    Node node;
    Tree tree;
    TreeDialog frame;

    public void setTree(Tree tree)
    {
	this.tree = tree;
    }
    public void setNode(Node node)
    {
	this.node = node;
    }
    public void setFrame(TreeDialog frame)
    {
	this.frame = frame;
    }
}