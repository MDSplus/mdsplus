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
    String columnNames[] = new String[0];
    String rowNames[] = new String[0];
    boolean state = true;
    String labelString = "";
    boolean initializing = false;
    boolean editable = true;
    boolean displayRowNumber = false;


    protected int preferredColumnWidth = 30;
    protected int preferredHeight = 70;
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
    }
    public int getNumRows(){ return numRows; }

    public void setNumCols(int numCols)
    {
        this.numCols = numCols;
        table.setPreferredScrollableViewportSize(new Dimension(preferredColumnWidth * numCols, preferredHeight));
        redisplay();
    }
    public int getNumCols() {return numCols; }

    public void setPreferredColumnWidth(int preferredColumnWidth)
    {
      this.preferredColumnWidth = preferredColumnWidth;
    }
    public int getPreferredColumnWidth(){return preferredColumnWidth;}


    public int getPreferredHeight(){return preferredHeight;}

    public void setPreferredHeight(int preferredHeight)
    {
      this.preferredHeight = preferredHeight;
    }

    public void setColumnNames(String [] columnNames)
    {
        this.columnNames = columnNames;
        redisplay();
    }

    public String [] getColumnNames() {return columnNames; }

    public void setRowNames(String [] rowNames)
    {
        this.rowNames = rowNames;
        redisplay();
    }

    public String [] getRowNames() {return rowNames; }

    public void setLabelString(String labelString)
    {
        this.labelString = labelString;
        redisplay();
    }

    public String getLabelString() {return labelString;}

    public void setEditable(boolean state) {editable = state;}
    public boolean getEditable() {return editable;}

    public void setDisplayRowNumber(boolean displayRowNumber)
    {
        this.displayRowNumber = displayRowNumber;
    }

    public boolean getDisplayRowNumber()
    {
        return displayRowNumber;
    }



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
        String decompiled = "";
        try {
            decompiled = Tree.dataToString(subtree.evaluateData(data, 0));
        }catch(Exception exc){System.err.println(exc);}
        StringTokenizer st = new StringTokenizer(decompiled, " ,[]");
        items = new String[numCols * numRows];
        int idx = 0;
        while( idx < numCols * numRows && st.hasMoreTokens())
            items[idx++] = st.nextToken();
        label.setText(labelString);



        table.setModel(new AbstractTableModel() {
            public int getColumnCount()
            {
                if(displayRowNumber)
                {

                  return numCols + 1;
                }
                else
                    return numCols;
            }
            public int getRowCount() {return numRows; }
            public String getColumnName(int idx)
            {
                if(displayRowNumber || (rowNames != null && rowNames.length > 0))
                {
                    if(idx == 0)
                        return "";
                    else
                        return columnNames[idx - 1];
                }
                else
                    return columnNames[idx];
            }

            public Object getValueAt(int row, int col)
            {
               if(rowNames != null && rowNames.length > 0)
                {
                  if (col == 0) {
                    try {
                      return rowNames[row];
                    }
                    catch (Exception exc) {
                      return "";
                    }
                  }
                  else {
                    try {
                      return items[row * numCols + col - 1];
                    }
                    catch (Exception exc) {
                      return "";
                    }
                  }
                }
                else if(displayRowNumber)
                {
                  if (col == 0)
                    return "" + (row + 1);
                  else {
                    try {
                      return items[row * numCols + col - 1];
                    }
                    catch (Exception exc) {
                      return "";
                    }
                  }
                }

                else
                {
                    try {
                        return items[row * numCols + col];
                    }catch(Exception exc) {return null; }
                }
            }
            public boolean isCellEditable(int row, int col)
            {
                if(displayRowNumber && col == 0)
                    return false;
                else
                    return editable;
            }

            public void setValueAt(Object value, int row, int col)
            {
                if(rowNames.length > 0)
                    items[row * numCols + col - 1] = (String)value;
                 else if(displayRowNumber)
                      items[row * numCols + col - 1] = (String)value;

                else
                    items[row * numCols + col] = (String)value;
                fireTableCellUpdated(row, col);
            }});
        table.setPreferredScrollableViewportSize(new Dimension(preferredColumnWidth * numCols,preferredHeight));
        initializing = false;
    }



    public void displayData(Data data, boolean is_on)
    {
        state = is_on;
        String decompiled = Tree.dataToString(data);
        StringTokenizer st = new StringTokenizer(decompiled, " ,[]");
        items = new String[numCols * numRows];
        int idx = 0;
        while( idx < numCols * numRows && st.hasMoreTokens())
            items[idx++] = st.nextToken();
        for(int i = 0; i < numCols; i++)
        {
            table.getColumnModel().getColumn(i).setMinWidth(6);
            table.getColumnModel().getColumn(i).setPreferredWidth(6);
            table.getColumnModel().getColumn(i).setWidth(6);
            System.out.println(table.getColumnModel().getColumn(i).getWidth());
        }
        table.repaint();
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
                if(i < n_data - 1 && items[i+1] != null && !items[i+1].equals(""))
                    dataString += ",";
                else
                   if(i == n_data - 1)
                     dataString += "]";
            }
            else
                dataString += ",";
        }
        return Tree.dataFromExpr(dataString);
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

