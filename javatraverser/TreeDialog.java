package jTraverser;
import java.awt.event.*;
import javax.swing.*;
import java.awt.*;

class TreeDialog extends JFrame
{
    boolean in_use;
    NodeEditor node_editor;
	
    public TreeDialog(NodeEditor editor)
    {
	in_use = true;
	getContentPane().add(editor);
	node_editor = editor;
    }
    public final boolean inUse() {return in_use; }
    public final NodeEditor getEditor() {return node_editor; }
    public void dispose()
    {
	in_use = false;
	setVisible(false);
    }
    public  void repack()
    {
	if(!isVisible())
	    return;
	Point origin = getLocationOnScreen();
	setLocation(origin);
	pack();
    }
    public final void setUsed(boolean used)
    {
	in_use = used;
    }  
}
