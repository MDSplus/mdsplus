import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.lang.reflect.*;
import java.awt.event.*;

public class DeviceChannelCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceChannel bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField labelString, lines, columns, updateId, showVal;
    Choice nids;
    Button doneButton;
    Checkbox showBorder, inSameLine, showState;

    public DeviceChannelCustomizer()
    {
    }
    public void setObject(Object o)
    {
        bean = (DeviceChannel)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new GridLayout(4, 1));
        Panel jp1 = new Panel();
        jp1.add(new Label("Label: "));
        jp1.add(labelString = new TextField(30));
        labelString.setText(bean.getLabelString());
        jp1.add(showState = new Checkbox("Show state: ", bean.getShowState()));
        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(showBorder = new Checkbox("Border visible: ", bean.getBorderVisible()));
        jp1.add(new Label("Offset nid: "));
        jp1.add(nids = new Choice());

        String names[] = getDeviceFields();

        if(names != null)
        for(int i = 0; i < names.length; i++)
            nids.addItem(names[i]);
        int offsetNid = bean.getOffsetNid();
        if(offsetNid > 0) offsetNid--;
        nids.select(offsetNid);

        jp.add(jp1);

        jp1 = new Panel();
        jp1.add(new Label("Lines: "));
        jp1.add(lines = new TextField(4));
        lines.setText((new Integer(bean.getLines())).toString());
        jp1.add(new Label("Columns: "));
        jp1.add(columns = new TextField(4));
        columns.setText((new Integer(bean.getColumns())).toString());
        jp1.add(inSameLine = new Checkbox("Same line: ", bean.getInSameLine()));
        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(new Label("Show Id: "));
        jp1.add(updateId = new TextField(8));
        updateId.setText(bean.getUpdateIdentifier());
        jp1.add(new Label("Show value: "));
        jp1.add(showVal = new TextField(8));
        showVal.setText(bean.getShowVal());
        jp.add(jp1);

        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                bean.setLabelString(labelString.getText());
                bean.setBorderVisible(showBorder.getState());
                bean.setShowState(showState.getState());
                bean.setInSameLine(inSameLine.getState());
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                bean.setLines((new Integer(lines.getText())).intValue());
                bean.setColumns((new Integer(columns.getText())).intValue());
                bean.setUpdateIdentifier(updateId.getText());
                bean.setShowVal(showVal.getText());
                listeners.firePropertyChange(null, null, null);
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

