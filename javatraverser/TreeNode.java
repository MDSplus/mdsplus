//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

public class TreeNode extends JLabel
{
    Node node;
    static Node selected;
    static Node copied;
    static Font plain_f, bold_f;
    static {
	plain_f = new Font("Serif", Font.PLAIN, 12);
	bold_f = new Font("Serif" ,Font.BOLD, 12);
    }

    public static void setSelectedNode(Node sel)
    {
	    selected = sel;
 	    
	    
    }
    
	public static void copyToClipboard()
	{
	    try {
	        String fullPath = selected.getInfo().getFullPath();
	        Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
	        StringSelection content = new StringSelection(fullPath);
	        cb.setContents(content, null);
	    }catch(Exception exc){System.err.println("Cannot copy fullPath to Clibboard");}
	}
    public static void copy()
    {
        copied = selected;
    }
    
    public static void paste()
    {
        if(copied != null && copied != selected)
        {
            Node.pasteSubtree(copied, selected, true);
        }
    }
    
    public static void delete()
    {
        Tree.deleteNode(selected);
    }
    
    public TreeNode(Node _node, String name, Icon icon)
    {
	super(name+"                ", icon, JLabel.LEFT);
	node = _node;
	if(node.isOn())
	    setFont(bold_f);
	else
	    setFont(plain_f);
	setForeground(Color.black);
	if(node.isDefault())
	    //setText(node.getName().trim() + "                                   ");
	    //setForeground(Color.red);
	    setBorder(BorderFactory.createLineBorder(Color.black, 1));
	else
	    setBorder(BorderFactory.createLineBorder(Color.white, 1));
	    //setText(node.getName().trim() + "                                   ");
	    //setForeground(Color.black);
    }
    
    public void paint(Graphics g)
    {
	setText(node.getName());
	if(node.isDefault())
	    setForeground(Color.red);
	else
	    setForeground(Color.black);
    
    	if(node.isOn())
	    setFont(bold_f);
	else
	    setFont(plain_f);
	    
	if(selected == node)
	    setBorder(BorderFactory.createLineBorder(Color.black, 1));
	else
	    setBorder(BorderFactory.createLineBorder(Color.white, 1));
	super.paint(g);
    }
}
