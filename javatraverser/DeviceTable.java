import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;
import java.awt.*;
import java.util.*;


public class DeviceTable extends DeviceComponent
{
//    int preferredWidth = 200;
//    int preferredHeight = 100;
    int numRows = 3;
    int numCols = 3;
    String columnNames[];
    boolean state = true;
    String labelString = "";
    boolean initializing = false;
            
    protected JScrollPane scroll;
    protected JTable table;
    protected JLabel label;
    protected String items[] = new String[9];
    
    
    
  /*  public void setPreferredWidth(int preferredWidth)
    {
        this.preferredWidth = preferredWidth;
    }
    
    public int getPreferredWidth(){ return preferredWidth;}
    public void setPreferredHeight(int preferredHeight)
    {
        this.preferredHeight = preferredHeight;
        redisplay();
    }
    
    public int getPreferredHeight(){return preferredHeight;}
*/    public void setNumRows(int numRows)
    {
        this.numRows = numRows;
        items = new String[numRows * numCols];
        redisplay();
    }
    public int getNumRows(){ return numRows; }

    public void setNumCols(int numCols) 
    {
        this.numCols = numCols;
        table.setPreferredScrollableViewportSize(new Dimension(50 * numCols, 70));
        redisplay();
    }
    public int getNumCols() {return numCols; }

    public void setColumnNames(String [] columnNames)
    {
        this.columnNames = columnNames;
        redisplay();
    }
    
    public String [] getColumnNames() {return columnNames; }
    
    public void setLabelString(String labelString)
    {
        this.labelString = labelString;
        redisplay();
    }
    
    public String getLabelString() {return labelString;}
    
    
    
    public DeviceTable()
    {
        initializing = true;
        table = new JTable();
        scroll = new JScrollPane(table);
        table.setPreferredScrollableViewportSize(new Dimension(200, 70));
        label = new JLabel();
        setLayout(new BorderLayout());
        JPanel jp = new JPanel();
        jp.add(label);
        add(jp, "North");
        add(scroll, "Center");
        initializing = false;
    }
    public void initializeData(Data data, boolean is_on)
    {
        initializing = true;
        String decompiled = data.toString();
        StringTokenizer st = new StringTokenizer(decompiled, " ,[]");
        items = new String[numCols * numRows];
        int idx = 0;
        while( idx < numCols * numRows && st.hasMoreTokens())
            items[idx++] = st.nextToken();
        label.setText(labelString);
        table.setModel(new AbstractTableModel() {
            public int getColumnCount() {return numCols; }
            public int getRowCount() {return numRows; }
            public String getColumnName(int idx)
            {
                return columnNames[idx];
            }
            
            public Object getValueAt(int row, int col)
            {
                int id = row * numCols + col;
                 try {
                    return items[id];
                 }catch(Exception exc) {return null; }
            }
            public boolean isCellEditable(int row, int col) {return  true; }
            public void setValueAt(Object value, int row, int col) 
            {
                items[row * numCols + col] = (String)value;
                fireTableCellUpdated(row, col);
            }});
        //table.setPreferredScrollableViewportSize(new Dimension(preferredWidth, preferredHeight));
        initializing = false;
    }

    
    
    public void displayData(Data data, boolean is_on)
    {
        state = is_on;
        String decompiled = data.toString();
        StringTokenizer st = new StringTokenizer(decompiled, " ,[]");
        items = new String[numCols * numRows];
        int idx = 0;
        while( idx < numCols * numRows && st.hasMoreTokens())
            items[idx++] = st.nextToken();
        redisplay();
    }
    public boolean getState(){return state; }
        
    public Data getData()
    {
        int n_data = items.length;
        String dataString = "[";
        for(int i = 0; i < n_data; i++)
        {
            if(items[i] == null || items[i].equals(""))
            {
                dataString += "]";
                break;
            }
               
            if(i % numCols == 0)
                dataString += "[";
            dataString += items[i];
            if(i % numCols == numCols - 1)
            {
                dataString += "]";
                if(items[i+1] != null && !items[i+1].equals(""))
                    dataString += ",";
            }
            else
                dataString += ",";
        }
        return Data.fromExpr(dataString);
    }
    public Component add(Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }
        
    public Component add(String name, Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }
        
    public Component add(Component c, int intex)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }


}
    
