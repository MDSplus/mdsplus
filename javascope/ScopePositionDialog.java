import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

class ScopePositionDialog extends Dialog implements ActionListener, KeyListener, 
					    ItemListener, FocusListener, AdjustmentListener {

    ScopePositionDialog(Frame dw, String title, boolean flag)
    {
	super(dw, title, flag);
    }

    private Point findCoord(Component ob)
    {
	Point p = new Point();
	
	while(ob != null)
	{
	    p.x += ob.getLocation().x;
	    p.y += ob.getLocation().y;
	    if(ob instanceof ScopePositionDialog)
		break;
	    ob = ob.getParent();
	}
	return p;	
    }
 
    public void setPosition(Component ob)
    {
	Component comp = ob;
	Point r = findCoord(comp);
	setLocation(r.x + ob.getBounds().width/2 - getBounds().width/2,
			r.y + ob.getBounds().height/2 - getBounds().height/2);
    }

    public void actionPerformed(ActionEvent e) 
    {}
    public void keyReleased(KeyEvent e)
    {}
    public void keyTyped(KeyEvent e)
    {}
    public void keyPressed(KeyEvent e)
    {}
    public void itemStateChanged(ItemEvent e)
    {}
    public void focusGained(FocusEvent e)
    {}
    public void focusLost(FocusEvent e)
    {}      		     
    public void adjustmentValueChanged(AdjustmentEvent e)
    {}      		     
} 
