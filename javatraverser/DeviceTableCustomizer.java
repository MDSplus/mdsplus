import java.beans.*;
import java.util.*;

import java.awt.*;
import java.awt.event.*;

public class DeviceTableCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceTable bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField labelString, identifier, numCols, numRows, columnNames;
    Choice nids;
    Button doneButton;
    Checkbox displayRowNumC;

    public DeviceTableCustomizer()
    {
    }
    public void setObject(Object o)
    {
        bean = (DeviceTable)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new GridLayout(4, 1));
        Panel jp1 = new Panel();
        jp1.add(new Label("Label: "));
        jp1.add(labelString = new TextField(20));
        labelString.setText(bean.getLabelString());
        jp1.add(new Label("Num. Rows: "));
        jp1.add(numRows = new TextField(4));
        int rows = bean.getNumRows();
        numRows.setText((new Integer(rows)).toString());

        jp1.add(new Label("Num. Columns: "));
        jp1.add(numCols = new TextField(4));
        int cols = bean.getNumCols();
        numCols.setText((new Integer(cols)).toString());

        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(new Label("Offset nid: "));
        jp1.add(nids = new Choice());

        String names[] = getDeviceFields();


        if(names != null)
        for(int i = 0; i < names.length; i++)
            nids.addItem(names[i]);
        int offsetNid = bean.getOffsetNid();
        if(offsetNid > 0) offsetNid--;
        try {
            nids.select(offsetNid);
        }catch(Exception exc){}
        jp1.add(new Label("Opt. identifier: "));
        jp1.add(identifier = new TextField(bean.getIdentifier(), 15));
        jp1.add(displayRowNumC = new Checkbox("Display row num.", bean.getDisplayRowNumber()));


        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(new Label("Column Names: "));
        //jp1.add(columnNames = new TextField(bean.getIdentifier(), 40));
        columnNames = new TextField(40);
        String []colNamesArray = bean.getColumnNames();
        String cnames = "";
        if(colNamesArray != null)
        {
            for(int i = 0; i < colNamesArray.length; i++)
                cnames += colNamesArray[i] + " ";
        }
        columnNames.setText(cnames);

        jp1.add(columnNames);

        jp.add(jp1);

        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                String oldLabelString = bean.getLabelString();
                bean.setLabelString(labelString.getText());
                listeners.firePropertyChange("labelString", oldLabelString, bean.getLabelString());
                int oldNumCols = bean.getNumCols();
                bean.setNumCols(Integer.parseInt(numCols.getText()));
                listeners.firePropertyChange("numCols", oldNumCols, bean.getNumCols());
                int oldNumRows = bean.getNumRows();
                bean.setNumRows(Integer.parseInt(numRows.getText()));
                listeners.firePropertyChange("numRows", oldNumRows, bean.getNumRows());
                int oldOffsetNid = bean.getOffsetNid();
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
                String oldIdentifier = bean.getIdentifier();
                bean.setIdentifier(identifier.getText());
                listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
                boolean oldDisplayRowNumber = displayRowNumC.getState();
                bean.setDisplayRowNumber(displayRowNumC.getState());
                listeners.firePropertyChange("displayRowNumber", oldDisplayRowNumber, bean.getDisplayRowNumber());
                StringTokenizer st = new StringTokenizer(columnNames.getText(), " ,");
                String colNames[] = new String[st.countTokens()];
                int idx = 0;
                while(st.hasMoreTokens())
                    colNames[idx++] = st.nextToken();
                String []oldColumnNames = bean.getColumnNames();
                bean.setColumnNames(colNames);
                listeners.firePropertyChange("columnNames", oldColumnNames, bean.getColumnNames());
            }
        });
        add(jp,"South");
    }

    public void addPropertyChangeListener(PropertyChangeListener l)
    {
        listeners.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l)
    {
        listeners.removePropertyChangeListener(l);
    }
  }

