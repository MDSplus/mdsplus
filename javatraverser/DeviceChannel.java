import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import java.awt.*;
public class DeviceChannel extends DeviceComponent
{
    public String labelString = null;
    public boolean borderVisible = false;
    public boolean inSameLine = false;
    protected JCheckBox    checkB = null;
    protected Vector device_components = null;
    public int lines = 1, columns = 0; //if columns == 0 FlowLayout is assumed
    protected boolean initializing = false;
    protected JPanel componentsPanel;

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
        else
            componentsPanel.setLayout(new FlowLayout());
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
        checkB.setText(labelString);
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
        initializing = false;
    }    
    protected void initializeData(Data data, boolean is_on)
    {
        checkB.setText(labelString);
        checkB.setSelected(is_on);
        checkB.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e)
            {
                propagateState(checkB.isSelected());
            }
        });
    }
    
    
    protected void displayData(Data data, boolean is_on)
    {
        checkB.setSelected(is_on);
    }
    protected Data getData(){return null;}
    protected boolean getState()
    {
        return checkB.isSelected();
    }
    
    private void propagateState(boolean state)
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
        int size = device_components.size();
        for(int i = 0; i < size; i++)
            ((DeviceComponent)device_components.elementAt(i)).setEnabled(state);
    }

    public void setEnabled(boolean state)
    {
        if(checkB != null)
            checkB.setEnabled(state);
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

}
