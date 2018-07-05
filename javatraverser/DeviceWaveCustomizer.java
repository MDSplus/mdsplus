import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Checkbox;
import java.awt.Choice;
import java.awt.GridLayout;
import java.awt.Label;
import java.awt.Panel;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.Customizer;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

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
                boolean oldMinXVisible = bean.getMinXVisible();
                bean.setMinXVisible(minXVisible.getState());
                listeners.firePropertyChange("minXVisible", oldMinXVisible, bean.getMinXVisible());
                boolean oldMaxXVisible = bean.getMaxXVisible();
                bean.setMaxXVisible(maxXVisible.getState());
                listeners.firePropertyChange("maxXVisible", oldMaxXVisible, bean.getMaxXVisible());
                boolean oldMinYVisible = bean.getMinYVisible();
                bean.setMinYVisible(minYVisible.getState());
                listeners.firePropertyChange("minYVisible", oldMinYVisible, bean.getMinYVisible());
                boolean oldMaxYVisible = bean.getMaxYVisible();
                bean.setMaxYVisible(maxYVisible.getState());
                listeners.firePropertyChange("maxYVisible", oldMaxYVisible, bean.getMaxYVisible());
                int oldOffsetNid = bean.getOffsetNid();
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
                String oldIdentifier = bean.getIdentifier();
                bean.setIdentifier(identifier.getText());
                listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
                String oldUpdateIdentifier = bean.getIdentifier();
                bean.setUpdateIdentifier(updateIdentifier.getText().trim());
                listeners.firePropertyChange("updateIdentifier", oldUpdateIdentifier, bean.getUpdateIdentifier());
                String oldUpdateExpression = bean.getUpdateExpression();
                bean.setUpdateExpression(updateExpression.getText().trim());
                listeners.firePropertyChange("updateExpression", oldUpdateExpression, bean.getUpdateExpression());
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

