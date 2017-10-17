//package jTraverser;
import java.awt.Color;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.JLabel;

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
    final Color CWrite    = new Color(0,128,0);
    final Color CWriteO   = new Color(96,64,0);
    final Color CNoWrite  = new Color(128,0,0);
    final Color CNoWriteO = new Color(192,0,0);
    final Color CMSetup   = new Color(0,0,128);
    final Color CMSetupO  = new Color(96,0,128);
    final Color CSSetup   = new Color(128,0,128);
    final Color CSSetupO  = new Color(128,0,64);
    final Color CNorm     = new Color(0,0,0);
    final Color CNormO    = new Color(96,0,96);
    final Color CInclude  = new Color(0,0,0);
    final Color CExclude  = new Color(128,128,128);
    public TreeNode(Node node, String name, Icon icon, boolean isSelected)
    {
	    super((node.isDefault() ? "("+node.getName()+")" : node.getName()), icon, JLabel.LEFT);
	    this.node = node;
        if (node.getUsage()==NodeInfo.USAGE_SUBTREE)
            setForeground(node.isIncludeInPulse() ? CInclude : CExclude);
        else
        {
            if (node.isNoWriteModel() & node.isNoWriteModel())
                setForeground(CNoWrite);
            else if (node.isNoWriteModel())
                setForeground( jTraverser.model ? (node.isWriteOnce() ? CNoWriteO : CNoWrite) : (node.isWriteOnce() ? CWriteO  : CWrite ));
            else if (node.isNoWriteShot())
                setForeground(!jTraverser.model ? (node.isWriteOnce() ? CNoWriteO : CNoWrite) : (node.isWriteOnce() ? CWriteO  : CWrite ));
            else if (node.isSetup())
                setForeground( jTraverser.model ? (node.isWriteOnce() ? CMSetupO  : CMSetup ) : (node.isWriteOnce() ? CSSetupO : CSSetup));
            else
                setForeground(node.isWriteOnce() ? CNormO : CNorm);
        }
        setFont(node.isOn() ? bold_f : plain_f);
	    setBorder(BorderFactory.createLineBorder(isSelected ? Color.black : Color.white, 1));
    }

    @Override
    public String getToolTipText()
    {
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
            return tagsStr;
        }
        return null;
    }

	public static void copyToClipboard()
	{
        Node currnode = Tree.getCurrentNode();
	    if(currnode == null) return;
	    try {
            Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
            StringSelection content;
            String path = currnode.getFullPath();
            content = new StringSelection(path);
            cb.setContents(content, null);
	    }catch(Exception exc){jTraverser.stderr("Cannot copy fullPath to Clipboard", exc);}
	}

    public static void copy()
    {
        Node currnode = Tree.getCurrentNode();
	    if(currnode == null) return;
        cut = false;
        copied = currnode;
        jTraverser.stdout("copy: "+copied+" from "+copied.parent);
    }

    public static void cut()
    {
        Node currnode = Tree.getCurrentNode();
	    if(currnode == null) return;
        if (jTraverser.isEditable())
        {
            cut = true;
            copied = currnode;
            jTraverser.stdout("cut: "+copied+" from "+copied.parent);
        }
        else
            copy();
    }

    public static void paste()
    {
        Node currnode = Tree.getCurrentNode();
	    if(currnode == null) return;
        if (jTraverser.isEditable())
        {
            jTraverser.stdout((cut ? "moved: " : "copied: ") +copied+ " from " + copied.parent + " to " + currnode);
            if(copied != null && copied != currnode)
            {
                if (cut)
                {
                    if (copied.move(currnode))
                        copied = null;
                }
                else
                    Node.pasteSubtree(copied, currnode, true);
            }
        }
        else
            jTraverser.stdout("Cannot paste "+copied+". Tree not in edit mode.");
    }

    public static void delete()
    {
        Node currnode = Tree.getCurrentNode();
	    if(currnode == null) return;
        if (jTraverser.isEditable())
            Tree.deleteNode(currnode);
        else
            jTraverser.stdout("Cannot delete "+currnode+". Tree not in edit mode.");
    }
}
