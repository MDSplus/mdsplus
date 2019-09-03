import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
public class DeviceChannel extends DeviceComponent
{
    //Keep copied data for channel components: indexed by the offset between this nid and the component nid
    static Hashtable componentHash = new Hashtable();


    public String labelString = null;
    public boolean borderVisible = false;
    public boolean inSameLine = false;

    protected JCheckBox    checkB = null;
    protected Vector device_components = null;
    public int lines = 1, columns = 0; //if columns == 0 FlowLayout is assumed

    public String showVal;
    public boolean showState = true;
    private boolean initial_state;
    private boolean reportingChange = false;
    private JPopupMenu copyPastePopup;

    protected boolean initializing = false;
    protected JPanel componentsPanel;

    public void copy()
    {
	buildComponentList();
	for(int i = 0; i < device_components.size(); i++)
	{
	    DeviceComponent currComponent = (DeviceComponent)device_components.elementAt(i);
	    int intOffset = currComponent.getOffsetNid() - getOffsetNid();
	    componentHash.put(new Integer(intOffset), currComponent.getFullData());
	}
    }

    public void paste()
    {
	buildComponentList();
	for(int i = 0; i < device_components.size(); i++)
	{
	   DeviceComponent currComponent = (DeviceComponent)device_components.elementAt(i);
	   int intOffset = currComponent.getOffsetNid() - getOffsetNid();
	   Object currData = componentHash.get(new Integer(intOffset));
	   if(currData != null)
	       currComponent.dataChanged(currComponent.getOffsetNid(), currData);
       }
    }

    public void propagate()
    {
	copy();
	Container parent = getParent();
	Component components[] = parent.getComponents();
	for(int i = 0; i < components.length; i++)
	{
	    if(components[i] instanceof DeviceChannel && components[i] != this)
	    {
	        ((DeviceChannel)components[i]).paste();
	    }

	}
     }

    public Container getContainer() {
	   //JOptionPane.showMessageDialog(null, "GET CONTAINER", "", JOptionPane.INFORMATION_MESSAGE);
	   return componentsPanel;
    }

    public void setShowVal(String showVal)
    {
	this.showVal = showVal;
    }
    public String getShowVal() {return showVal;}
    public void setShowState(boolean showState)
    {
	this.showState = showState;
    }
    public boolean getShowState() {return showState;}


    public void setInSameLine(boolean inSameLine)
    {
	this.inSameLine = inSameLine;
	if(checkB != null)
	{
	    remove(checkB);
	    if(inSameLine)
	        add(checkB, "West");
	    else
	        add(checkB, "North");
	}
    }

    public boolean getInSameLine() {return inSameLine; }


    public void setLines(int lines)
    {
	initializing = true;
	this.lines = lines;
	if(lines != 0 && columns != 0)
	    componentsPanel.setLayout(new GridLayout(lines, columns));
	else if(lines == 0 && columns == 0)
	    componentsPanel.setLayout(new FlowLayout());
	else
	    componentsPanel.setLayout(new BorderLayout());
	initializing = false;
    }
    public int getLines() {return lines; }
    public int getColumns() {return columns; }
    public void setColumns(int columns)
    {
	initializing = true;
	this.columns = columns;
	if(lines != 0 && columns != 0)
	    componentsPanel.setLayout(new GridLayout(lines, columns));
	else
	    componentsPanel.setLayout(new FlowLayout());
	initializing = false;
    }
    public void setLabelString(String labelString)
    {
	this.labelString = labelString;
	if(checkB != null) checkB.setText(labelString);
	redisplay();
    }
    public String getLabelString() {return labelString; }
    public void setBorderVisible(boolean borderVisible)
    {
	this.borderVisible = borderVisible;

	if(borderVisible)
	    //componentsPanel.setBorder(new LineBorder(Color.black, 1));
	    setBorder(new LineBorder(Color.black, 1));
	else
	    //componentsPanel.setBorder(null);
	    setBorder(null);
    }
    public boolean getBorderVisible(){return borderVisible;}

    public DeviceChannel()
    {
	initializing = true;
	mode = STATE;
	setLayout(new BorderLayout());
	componentsPanel = new JPanel();
	add(checkB = new JCheckBox(), "North");
	checkB.setText(labelString);
	add(componentsPanel, "Center");

	copyPastePopup = new JPopupMenu();
	JMenuItem copyI = new JMenuItem("Copy");
	copyI.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        copy();
	    }
	});
	copyPastePopup.add(copyI);
	JMenuItem pasteI = new JMenuItem("Paste");
	pasteI.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        paste();
	    }
	});
	copyPastePopup.add(pasteI);
	JMenuItem propagateI = new JMenuItem("Propagate");
	propagateI.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        propagate();
	    }
	});
	copyPastePopup.add(propagateI);
	copyPastePopup.pack();
	copyPastePopup.setInvoker(this);
	addMouseListener(new MouseAdapter()
	{
	    public void mousePressed(MouseEvent e)
	    {
	        if ( (e.getModifiers() & Event.META_MASK) != 0)
	        {
	             copyPastePopup.setInvoker(DeviceChannel.this);
	             copyPastePopup.show(DeviceChannel.this, e.getX(), e.getY());
	        }
	    }
});

	initializing = false;
    }
    protected void initializeData(Data data, boolean is_on)
    {
	if(!showState)
	{
	    remove(checkB);
	    checkB = null;
	}
	else
	{
	    checkB.setText(labelString);
	    checkB.setSelected(is_on);
	    checkB.addChangeListener(new ChangeListener() {
	        public void stateChanged(ChangeEvent e)
	        {
	          reportingChange = true;
	          reportStateChanged(checkB.isSelected());
	          reportingChange = false;
	          propagateState(checkB.isSelected());
	        }
	    });
	}
	propagateState(is_on);
    }

    protected void stateChanged(int offsetNid, boolean state)
    {
      if(this.offsetNid != offsetNid || reportingChange)
	return;
      if(checkB != null)
	checkB.setSelected(state);
    }

    protected void displayData(Data data, boolean is_on)
    {
	initial_state = is_on;
	if(checkB != null) checkB.setSelected(is_on);
	propagateState(is_on);
    }
    protected Data getData(){return null;}
    protected boolean getState()
    {
	if(!showState || checkB == null)
	    return initial_state;
	else
	    return checkB.isSelected();
    }


    private void buildComponentList()
    {
	if(device_components == null)
	{
	    device_components = new Vector();
	    java.util.Stack search_stack = new java.util.Stack();
	    search_stack.push(this);
	    do
	    {
	        Component [] curr_components = ((Container)search_stack.pop()).getComponents();
	        if(curr_components == null) continue;
	        for(int i = 0; i < curr_components.length; i++)
	        {
	            if(curr_components[i] instanceof DeviceComponent)
	                device_components.addElement(curr_components[i]);
	            else if(curr_components[i] instanceof Container)
	                search_stack.push(curr_components[i]);
	        }
	    }while(!search_stack.empty());
	}
    }



    private void propagateState(boolean state)
    {
	buildComponentList();
	int size = device_components.size();
	for(int i = 0; i < size; i++)
	    ((DeviceComponent)device_components.elementAt(i)).setEnabled(state);
    }

    public void setEnabled(boolean state)
    {
	if(checkB != null)
	    checkB.setEnabled(state);
	buildComponentList();
	if(device_components != null)
	{
	    int size = device_components.size();
	    for(int i = 0; i < size; i++)
	        ((DeviceComponent)device_components.elementAt(i)).setEnabled(state);
	}
    }
    public void setLayout(LayoutManager layout)
    {
	if(!initializing) return;
	super.setLayout(layout);
    } //Do not accept interferences

    public Component add(Component c)
    {
	if(!initializing)
	    return componentsPanel.add(c);
	return super.add(c);
    }

    public Component add(String name, Component c)
    {
	if(!initializing)
	    return componentsPanel.add(c);
	return super.add(c);
    }

    public Component add(Component c, int intex)
    {
	if(!initializing)
	    return componentsPanel.add(c);
	return super.add(c);
    }

    public void fireUpdate(String updateId, Data newExpr)
    {
	if(updateId == null || !updateIdentifier.equals(updateId))
	    return;
	String newVal = newExpr.toString();
	newVal = newVal.substring(1, newVal.length() - 1);
	if(showVal != null && showVal.equals(newVal))
	{
	    setEnabledAll(true);
	    LayoutManager layout = getParent().getLayout();
	    if(layout != null && (layout instanceof CardLayout))
	        ((CardLayout)layout).show(getParent(), showVal);
	    //Display this component using showVal as constraint
	}
	else
	    setEnabledAll(false);
    }

    protected boolean supportsState()
    {
      return showState;
    }
    protected void setEnabledAll(boolean enabled)
    {
	buildComponentList();
	if(device_components != null)
	{
	    int size = device_components.size();
	    for(int i = 0; i < size; i++)
	    {
	        if(enabled)
	            ((DeviceComponent)device_components.elementAt(i)).setEnable();
	        else
	            ((DeviceComponent)device_components.elementAt(i)).setDisable();
	    }
	}
    }
    public void postConfigure()
    {
      propagateState(curr_on);
    }


}
