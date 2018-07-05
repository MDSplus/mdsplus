import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Choice;
import java.awt.Label;
import java.awt.Panel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.Customizer;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

public class DeviceWaveDisplayCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceWaveDisplay bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    Choice nids;
    Button doneButton;

    public DeviceWaveDisplayCustomizer()
    {
    }
    public void setObject(Object o)
    {

        bean = (DeviceWaveDisplay)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.add(new Label("Offset nid: "));
        jp.add(nids = new Choice());

        String names[] = getDeviceFields();

        if(names != null)
        for(int i = 0; i < names.length; i++)
            nids.addItem(names[i]);
        int offsetNid = bean.getOffsetNid();
        if(offsetNid > 0) offsetNid--;
        nids.select(offsetNid);
        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e)
        {
            if(bean == null) return;
            int oldOffsetNid = bean.getOffsetNid();
            bean.setOffsetNid(nids.getSelectedIndex() + 1);
            listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
            DeviceWaveDisplayCustomizer.this.repaint();
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

