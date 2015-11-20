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
	    }catch(Exception exc){jTraverser.stderr("Cannot copy fullPath to Clipboard", exc);}
	}

    public static void copy()
    {
        cut = false;
        copied = Tree.curr_node;
        jTraverser.stdout("copy: "+copied+" from "+copied.parent);
    }

    public static void cut()
    {
        if (jTraverser.isEditable())
        {
            cut = true;
            copied = Tree.curr_node;
            jTraverser.stdout("cut: "+copied+" from "+copied.parent);
        }
        else
            copy();
    }

    public static void paste()
    {
        if (jTraverser.isEditable())
        {
            jTraverser.stdout((cut ? "moved: " : "copied: ") +copied+ " from " + copied.parent + " to " + Tree.curr_node);
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
        else
            jTraverser.stdout("Cannot paste "+copied+". Tree not in edit mode.");
    }

    public static void delete()
    {
        if (jTraverser.isEditable())
            Tree.deleteNode(Tree.curr_node);
        else
            jTraverser.stdout("Cannot delete "+Tree.curr_node+". Tree not in edit mode.");
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
	    setText(node.getName()+"      ");
        setForeground(node.isDefault() ? Color.red : Color.black);
        setFont(node.isOn() ? bold_f : plain_f);  
	    setBorder(BorderFactory.createLineBorder((Tree.curr_node == node) ? Color.black : Color.white, 1));
	    super.paint(g);
    }
}
