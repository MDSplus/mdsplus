import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.awt.datatransfer.*;


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
    boolean binary = false;
    JPopupMenu popM = null;
    JMenuItem copyRowI, copyColI, copyI, pasteRowI, pasteColI, pasteI,
        propagateToRowI, propagateToColI;

    protected int preferredColumnWidth = 30;
    protected int preferredHeight = 70;
    protected JScrollPane scroll;
    protected JTable table;
    protected JLabel label;
    protected String items[] = new String[9];
    static String copiedColItems[], copiedRowItems[], copiedItems[];

    public void setNumRows(int numRows)
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

    public void setBinary(boolean binary){this.binary = binary;}
    public boolean getBinary(){return binary;}


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
        if(rowNames.length > 0)
          displayRowNumber = true;
        table = new JTable();
        scroll = new JScrollPane(table);
        table.setPreferredScrollableViewportSize(new Dimension(200, 70));
        label = new JLabel();
        setLayout(new BorderLayout());
        JPanel jp = new JPanel();
        jp.add(label);
        add(jp, "North");
        add(scroll, "Center");

        table.addMouseListener(new MouseAdapter()  {
          public void mousePressed(MouseEvent e) {
            if ( (e.getModifiers() & Event.META_MASK) != 0) { //If MB3
              showPopup(e.getX(), e.getY());
            }
          }
        });
        initializing = false;
    }

    void showPopup(int x, int y)
    {
      if(popM == null)
      {
        popM = new JPopupMenu();
        copyRowI = new JMenuItem("Copy row");
        copyRowI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {copyRow(table.getSelectedRow());}
        });
        popM.add(copyRowI);
        copyColI = new JMenuItem("Copy column");
        copyColI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {copyCol(table.getSelectedColumn());}
        });
        popM.add(copyColI);
        copyI = new JMenuItem("Copy table");
        copyI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {copy();}
        });
        popM.add(copyI);
        pasteRowI = new JMenuItem("Paste row");
        pasteRowI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {pasteRow(table.getSelectedRow());}
        });
        popM.add(pasteRowI);
        pasteColI = new JMenuItem("Paste column");
        pasteColI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {pasteCol(table.getSelectedColumn());}
        });
        popM.add(pasteColI);
        pasteI = new JMenuItem("Paste table");
        pasteI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {paste();}
        });
        popM.add(pasteI);
        JMenuItem copyClipboardI = new JMenuItem("Clipboard Copy");
        copyClipboardI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {writeToClipboard();}
        });
        popM.add(copyClipboardI);
        JMenuItem pasteClipboardI = new JMenuItem("Clipboard Paste");
        pasteClipboardI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {readFromClipboard();}
        });
        popM.add(pasteClipboardI);
        JMenuItem propagateToRowI = new JMenuItem("Propagate to Row");
        propagateToRowI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {propagateToRow(table.getSelectedRow() ,table.getSelectedColumn());}
        });
        popM.add(propagateToRowI);
        JMenuItem propagateToColI = new JMenuItem("Propagate to Column");
        propagateToColI.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {propagateToCol(table.getSelectedRow(), table.getSelectedColumn());}
        });
        popM.add(propagateToColI);
      }
      if(copiedRowItems == null)
        pasteRowI.setEnabled(false);
      else
        pasteRowI.setEnabled(true);

      if(copiedColItems == null)
        pasteColI.setEnabled(false);
      else
        pasteColI.setEnabled(true);

      if(copiedItems == null)
       pasteI.setEnabled(false);
     else
       pasteI.setEnabled(true);

       popM.show(table, x, y);
    }


    void copy()
    {
      copiedItems = new String[items.length];
      for(int i = 0; i < items.length; i++)
        copiedItems[i] = items[i];
    }

    void paste()
    {
      try {
        for (int i = 0; i < items.length; i++)
          items[i] = copiedItems[i];
      }catch(Exception exc){}
      table.repaint();
    }

    void copyRow(int row)
    {
      if(row == -1) return;
      copiedRowItems = new String[numCols];
      for(int i = 0; i < numCols; i++)
        copiedRowItems[i] = items[row * numCols + i];
    }

    void pasteRow(int row)
    {
      if(row == -1) return;
      try {
        for (int i = 0; i < numCols; i++)
          items[row * numCols + i] = copiedRowItems[i];
      }catch(Exception exc){}
      table.repaint();
    }
    void copyCol(int col)
    {
      if(displayRowNumber) col--;
      copiedColItems = new String[numRows];
      for(int i = 0; i < numRows; i++)
        copiedColItems[i] = items[col + i * numCols];
    }

    void pasteCol(int col)
    {
      if(displayRowNumber) col--;
      try
      {
        for (int i = 0; i < numRows; i++)
          items[col  + i * numCols] = copiedColItems[i];
      }catch(Exception exc){}
      table.repaint();
    }

    void propagateToRow(int row, int col)
    {
      if(displayRowNumber) col--;
      if(row == -1 || col == -1) return;
      try
      {
        for (int i = 0; i < numCols; i++)
          items[row * numCols +i] = items[row*numCols + col];
      }catch(Exception exc){}
      table.repaint();
    }

    void propagateToCol(int row, int col)
    {
      if(displayRowNumber) col--;
      if(row == -1 || col == -1) return;
      try
      {
        for (int i = 0; i < numCols; i++)
          items[col  + i * numCols] = items[row*numCols + col];
      }catch(Exception exc){}
      table.repaint();
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
        if(!decompiled.startsWith("["))
          st.nextToken();
        while( idx < numCols * numRows && st.hasMoreTokens())
            items[idx++] = st.nextToken();
        label.setText(labelString);


        table.setModel(new AbstractTableModel() {
            public int getColumnCount()
            {
                if(displayRowNumber || (rowNames != null && rowNames.length > 0))
                {

                  return numCols + 1;
                }
                else
                    return numCols;
            }
            public int getRowCount() {return numRows; }
            public String getColumnName(int idx)
            {
              try {
                if (displayRowNumber || (rowNames != null && rowNames.length > 0)) {
                  if (idx == 0)
                    return "";
                  else
                    return columnNames[idx - 1];
                }
                else
                  return columnNames[idx];
              }catch(Exception exc){return "";}
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
                      String retItem = items[row * numCols + col - 1];
                      if(!binary)
                        return retItem;
                      else if (retItem.trim().equals("0"))
                        return new Boolean(false);
                      else return new Boolean(true);
                    }
                    catch (Exception exc) {
                      return null;
                    }
                  }
                }
                else if(displayRowNumber)
                {
                  if (col == 0)
                    return "" + (row + 1);
                  else {
                    try {
                      String retItem = items[row * numCols + col - 1];
                      if(!binary)
                        return retItem;
                      else if (retItem.trim().equals("0"))
                        return new Boolean(false);
                      else return new Boolean(true);
                    }
                    catch (Exception exc) {
                      return null;
                    }
                  }
                }

                else
                {
                    try {
                      String retItem = items[row * numCols + col];
                      if(!binary)
                        return retItem;
                      else if (retItem.trim().equals("0"))
                        return new Boolean(false);
                      else return new Boolean(true);
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
              int itemIdx;
                if((rowNames != null && rowNames.length > 0) || displayRowNumber)
                  itemIdx = row * numCols + col - 1;
                else
                  itemIdx = row * numCols + col;

                if(binary)
                {
                  boolean isOn = ((Boolean)value).booleanValue();
                  items[itemIdx] = (isOn)?"1":"0";
                }
                else
                  items[itemIdx] = (String)value;
                fireTableCellUpdated(row, col);
            }

            public Class getColumnClass(int c) {
              if (!binary)
                return String.class;
              if ((rowNames.length > 0 || displayRowNumber) && c == 0)
                return String.class;
              return Boolean.class;
            }
          });


          if(binary)
            table.setRowSelectionAllowed(false);

          table.setPreferredScrollableViewportSize(new Dimension(preferredColumnWidth * numCols,preferredHeight));
          table.revalidate();
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
    public void apply() throws Exception
    {
      CellEditor ce = table.getCellEditor();
      if(ce != null)
        ce.stopCellEditing();
      super.apply();
    }

    void readFromClipboard()
    {
      Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
      Transferable transferable = clipboard.getContents(null);
      if (transferable == null)
        return;
      if (transferable.isDataFlavorSupported(DataFlavor.stringFlavor)) {
        try {
          String tableText = (String) transferable.getTransferData(DataFlavor.
              stringFlavor);
          StringTokenizer st = new StringTokenizer(tableText, " ,\n");
          int idx = 0;
          while (st.hasMoreTokens() && items.length > idx)
            items[idx++] = st.nextToken();
        }
        catch (Exception exc) {
          System.err.println("Error reading from clipboard: " + exc);
        }
        table.repaint();
      }
    }

    void writeToClipboard()
    {
      Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
      String tableText = "";
      int idx = 0;
      for(int i = 0; i < numRows; i++)
      {
        for (int j = 0; j < numCols; j++)
          tableText += " " + items[idx++];
          tableText += "\n";
      }
      clipboard.setContents(new StringSelection(tableText), null);

    }




}

