/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.lang.reflect.*;
import java.awt.event.*;

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

