import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.lang.reflect.*;
import java.awt.event.*;

public class DeviceWaveCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceWave bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField identifier, updateIdentifier, updateExpression;
    
    Choice nids;
    Button doneButton;
    Checkbox minXVisible;
    Checkbox maxXVisible;
    Checkbox minYVisible;
    Checkbox maxYVisible;
    
    public DeviceWaveCustomizer()
    {
    }        
    public void setObject(Object o)
    {

        bean = (DeviceWave)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new GridLayout(3, 1));
        Panel jp1 = new Panel();
        jp1.add(new Label("Opt. identifier: "));
        jp1.add(identifier = new TextField(bean.getIdentifier(), 20));
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
        jp1.add(minXVisible = new Checkbox("Min X Visible: ", bean.getMinXVisible()));
        jp1.add(maxXVisible = new Checkbox("Max X Visible: ", bean.getMaxXVisible()));
        jp1 = new Panel();
        jp1.add(minYVisible = new Checkbox("Min Y Visible: ", bean.getMinYVisible()));
        jp1.add(maxYVisible = new Checkbox("Max Y Visible: ", bean.getMaxYVisible()));
        jp.add(jp1);
        
        jp1 = new Panel();
        jp1.add(new Label("Update id: "));
        jp1.add(updateIdentifier = new TextField(bean.getUpdateIdentifier(), 10));
        jp1.add(new Label("Update expr: "));
        jp1.add(updateExpression = new TextField(bean.getUpdateExpression(), 30));
        jp.add(jp1);
        
        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                bean.setMinXVisible(minXVisible.getState());
                bean.setMaxXVisible(maxXVisible.getState());
                bean.setMinYVisible(minYVisible.getState());
                bean.setMaxYVisible(maxYVisible.getState());
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                bean.setIdentifier(identifier.getText());
                bean.setUpdateIdentifier(updateIdentifier.getText().trim());
                bean.setUpdateExpression(updateExpression.getText().trim());
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
        
