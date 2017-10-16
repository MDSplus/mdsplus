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

public class DeviceLabelCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceLabel bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField labelString, identifier, numCols;
    Choice nids;
    Button doneButton;
//    Checkbox showState;
    Checkbox textOnly;
//    Checkbox editable;
    Checkbox displayEvaluated;

    public DeviceLabelCustomizer()
    {
    }
    public void setObject(Object o)
    {

        bean = (DeviceLabel)o;

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

//        jp1.add(showState = new Checkbox("Show state: ", bean.getShowState()));
        jp1.add(textOnly = new Checkbox("Text only: ", bean.getTextOnly()));
//        jp1.add(editable = new Checkbox("Editable: ", bean.getEditable()));

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
        jp1.add(displayEvaluated = new Checkbox("Display Evaluated: ", bean.getDisplayEvaluated()));
        jp1.add(new Label("Opt. identifier: "));
        jp1.add(identifier = new TextField(bean.getIdentifier(), 20));
        jp.add(jp1);

        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e)
        {
            if(bean == null) return;
            String oldLabel = bean.getLabelString();
            bean.setLabelString(labelString.getText());
            listeners.firePropertyChange("labelString", oldLabel, bean.getLabelString());

            String colStr = numCols.getText();
            int oldCols = bean.getNumCols();
            bean.setNumCols(Integer.parseInt(colStr));
            listeners.firePropertyChange("numCols", oldCols, bean.getNumCols());

            boolean oldTextOnly = bean.getTextOnly();
            bean.setTextOnly(textOnly.getState());
            listeners.firePropertyChange("textOnly", oldTextOnly, bean.getTextOnly());

/*
            boolean oldEditable = bean.getEditable();
            bean.setEditable(editable.getState());
            listeners.firePropertyChange("editable", oldEditable, bean.getEditable());
            boolean oldShowState = bean.getShowState();
            bean.setShowState(showState.getState());
            listeners.firePropertyChange("showState", oldShowState, bean.getShowState());
*/
            boolean oldDisplayEvaluated = bean.getDisplayEvaluated();
            bean.setDisplayEvaluated(displayEvaluated.getState());
            listeners.firePropertyChange("displayEvaluated", oldDisplayEvaluated, bean.getDisplayEvaluated());
            int oldOffsetNid = bean.getOffsetNid();
            bean.setOffsetNid(nids.getSelectedIndex() + 1);
            listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
            String oldIdentifier = bean.getIdentifier();
            bean.setIdentifier(identifier.getText());
            listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
            DeviceLabelCustomizer.this.repaint();
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

