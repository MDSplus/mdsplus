import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.lang.reflect.*;
import java.awt.event.*;

public class DeviceFieldCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceField bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField labelString, identifier, numCols;
    Choice nids;
    Button doneButton;
    Checkbox showState;
    Checkbox textOnly;
    Checkbox editable;
    
    public DeviceFieldCustomizer()
    {
    }        
    public void setObject(Object o)
    {

        bean = (DeviceField)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new GridLayout(3, 1));
        Panel jp1 = new Panel();
        jp1.add(new Label("Label: "));
        jp1.add(labelString = new TextField(30));
        labelString.setText(bean.getLabelString());
        jp1.add(new Label("Num. Columns: "));
        jp1.add(numCols = new TextField(4));
        int cols = bean.getNumCols();
        numCols.setText((new Integer(cols)).toString());
        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(showState = new Checkbox("Show state: ", bean.getShowState()));
        jp1.add(textOnly = new Checkbox("Text only: ", bean.getTextOnly()));
        jp1.add(editable = new Checkbox("Editable: ", bean.getEditable()));
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
        jp1.add(new Label("Opt. identifier: "));
        jp1.add(identifier = new TextField(bean.getIdentifier(), 20));
        jp.add(jp1);
        
        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                bean.setLabelString(labelString.getText());
                String colStr = numCols.getText();
                bean.setNumCols(Integer.parseInt(colStr));
                bean.setTextOnly(textOnly.getState());
                bean.setEditable(editable.getState());
                bean.setShowState(showState.getState());
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                bean.setIdentifier(identifier.getText());
                listeners.firePropertyChange(null, null, null);
            }
        });
        add(jp,"South");
    }
    
    public void addPropertyChangeListener(PropertyChangeListener l)
    {
		JOptionPane.showMessageDialog(null, "Aggiunto listener", 
		    "", JOptionPane.WARNING_MESSAGE);
        listeners.addPropertyChangeListener(l);
    }
    
    public void removePropertyChangeListener(PropertyChangeListener l)
    {
        listeners.removePropertyChangeListener(l);
    }
  }
        
