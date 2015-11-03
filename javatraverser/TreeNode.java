//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

public class TreeNode extends JLabel
{
    Node node;
    static Node copied;
    static boolean cut;
    static Font plain_f, bold_f;
    static {
	    plain_f = new Font("Serif", Font.PLAIN, 12);
	    bold_f = new Font("Serif" ,Font.BOLD, 12);
    }


	public static void copyToClipboard()
	{
	    try {
            Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
            String []tags = Tree.curr_node.getTags();
            StringSelection content;
            String path = Tree.curr_node.getFullPath();
            content = new StringSelection(path);
            cb.setContents(content, null);
	    }catch(Exception exc){System.err.println("Cannot copy fullPath to Clipboard");}
	}
    public static void copy()
    {
        cut = false;
        copied = Tree.curr_node;
        System.out.println("copied: "+copied+" from "+copied.parent);
    }

    public static void cut()
    {
        cut = true;
        copied = Tree.curr_node;
        System.out.println("cut: "+copied+" from "+copied.parent);
    }
    public static void paste()
    {
        System.out.println((cut ? "moved " : "copied") +copied+ " from " + copied.parent + " to " + Tree.curr_node);
        if(copied != null && copied != Tree.curr_node)
        {
            if (cut)
            {
                if (copied.move(Tree.curr_node))
                    copied = null;
            }
            else
                Node.pasteSubtree(copied, Tree.curr_node, true);
        }
    }

    public static void delete()
    {
        Tree.deleteNode(Tree.curr_node);
    }

    public TreeNode(Node node, String name, Icon icon)
    {
	    super("MMMMMMMMMMMMMMMM", icon, JLabel.LEFT);
	    this.node = node;
	    setFont(bold_f);
	    setBorder(BorderFactory.createLineBorder(Color.white, 1));
        String tags[] = node.getTags();
        if(tags.length > 0)
        {
            String tagsStr = "";
            for(int i = 0; i < tags.length; i++)
            {
                tagsStr += tags[i];
                if(i < tags.length - 1)
                    tagsStr += "\n";
            }
            setToolTipText(tagsStr);
        }
    }

    public void paint(Graphics g)
    {
	    setText(node.getName());
        setForeground(node.isDefault() ? Color.red : Color.black);
        setFont(node.isOn() ? bold_f : plain_f);  
	    setBorder(BorderFactory.createLineBorder((Tree.curr_node == node) ? Color.black : Color.white, 1));
	    super.paint(g);
    }
}
